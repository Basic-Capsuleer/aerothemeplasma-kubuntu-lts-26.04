#include "browser.h"

#include <QtWidgets>
#include <QFrame>
#include <QVBoxLayout>
#include <QWidget>
#include <QUrlQuery>

#include <AeroQt/util/scopefn.h>
#include <AeroQt/util/objecteventlistener.h>
#include <AeroQt/private/misc.h>

#include <iostream>

using namespace Aero;

// FIXME: Un-vibe-code / clean up this class

static void noTrailing(QString &path)
{
    if (path.endsWith("/"))
        path.chop(1);
}

class CrumbBar : public QWidget
{
    Q_OBJECT

    Browser *m_br;
    QHBoxLayout *m_pathLayout;

public:
    CrumbBar(Browser *br, QWidget *parent = nullptr);

    void setPath(QString path);
};

CrumbBar::CrumbBar(Browser *br, QWidget *parent) :
    QWidget(parent),
    m_br(br)
{
    setStyleSheet(
        "Aero--CrumbBar { border-image: url(:/AeroQt/transpbg/entry/normal.png) 4 4 4 4 repeat; border-width: 4px; }"
        "Aero--CrumbBar:hover { border-image: url(:/AeroQt/transpbg/entry/hover.png) 4 4 4 4 repeat; }"
    );
    this->setFixedHeight(24);

    setLayout(m_pathLayout = new QHBoxLayout(this) + also {
        it->setContentsMargins(4, 0, 4, 0);
        it->setSpacing(4);
    });
}

void CrumbBar::setPath(QString path)
{
    QStringList crumbs = path.split("/", Qt::SkipEmptyParts);

    // Clear existing crumb widgets.
    while (QLayoutItem *item = m_pathLayout->takeAt(0)) {
        if (QWidget *w = item->widget())
            w->deleteLater();
        delete item;
    }

    auto addArrow = [this]() {
        m_pathLayout->addWidget(new QLabel + also {
            it->setStyleSheet("background: transparent;");
            it->setPixmap(arrowPixmap(Qt::RightArrow, QColor(0x66, 0x66, 0x66), 6));
        });
    };

    auto addCrumb = [&](const QString &crumbPath) {
        if (crumbPath != "/")
            addArrow();

        QLabel *label = nullptr;

        if (QAction *act = m_br->actionForPath(crumbPath)) {
            label = new QLabel(act->text().isEmpty() ? crumbPath.section('/', -1) : act->text());

            if (crumbPath == "/") {
                label->setToolTip(label->text().toHtmlEscaped());
                label->setText(QString());
                label->setPixmap(act->icon().pixmap(16, 16));
            } else {
                label->setToolTip(actualToolTip(act));
            }

            if (m_br->m_pages[crumbPath].make) {
                label->setCursor(Qt::PointingHandCursor);

                underlineOnHover(
                    label,
                    [=]{ return true; },
                    palette().color(QPalette::Text),
                    palette().color(QPalette::Link)
                );

                onEvent<QMouseEvent>(label, QEvent::MouseButtonPress, [=](QMouseEvent *ev) {
                    if (ev->button() == Qt::LeftButton) {
                        m_br->navigateTo(crumbPath);
                        ev->accept();
                    }
                });
            }
        } else {
            label = new QLabel(crumbPath.section('/', -1));
        }

        m_pathLayout->addWidget(label);
    };

    addCrumb("/");

    QString cumulativePath;
    const QStringList parts = path.split('/', Qt::SkipEmptyParts);

    for (const QString &part : parts) {
        cumulativePath += "/" + part;
        addCrumb(cumulativePath);
    }

    m_pathLayout->addStretch();
}

class BrowserHistory : public QObject
{
    Q_OBJECT

    QList<QString> m_history;
    int m_historyIndex;

public:
    Q_PROPERTY(QString currentPath READ currentPath() WRITE recordPath NOTIFY positionChanged)
    Q_PROPERTY(bool canBack READ canBack NOTIFY positionChanged)
    Q_PROPERTY(bool canForward READ canForward NOTIFY positionChanged)

    BrowserHistory(QString initialPath, QObject *parent = nullptr);

    QString currentPath();
    void recordPath(QString path);

    bool canBack();
    bool canForward();

    inline QList<QString> history() { return m_history; }

Q_SIGNALS:
    void positionChanged();

public Q_SLOTS:
    void goBack();
    void goForward();
};

BrowserHistory::BrowserHistory(QString initialPath, QObject *parent) :
    QObject(parent),
    m_historyIndex(-1)
{
    m_history.append(initialPath);
}

QString BrowserHistory::currentPath()
{
    return m_history[m_history.length() + m_historyIndex];
}

void BrowserHistory::recordPath(QString path)
{
    m_history = m_history.first(m_history.length() + m_historyIndex + 1);
    m_historyIndex = -1;

    m_history.append(path);
    Q_EMIT positionChanged();
}

void BrowserHistory::goBack()
{
    if (canBack()) {
        m_historyIndex--;
        Q_EMIT positionChanged();
    }
}

void BrowserHistory::goForward()
{
    if (canForward()) {
        m_historyIndex++;
        Q_EMIT positionChanged();
    }
}

bool BrowserHistory::canBack()
{
    return -m_history.length() < m_historyIndex;
}

bool BrowserHistory::canForward()
{
    return m_historyIndex < -1;
}


QPair<QString, QMap<QString, QString>> separateArgs(QUrl url)
{
    auto out = QMap<QString, QString>() + also {
        for (const auto& [key, val]: QUrlQuery(url).queryItems()) {
            it[key] = val;
        }
    };

    return { url.path(), out };
}

static QWidget *replaceFrameChild(QFrame *parentFrame, QWidget *newChildWidget);

Browser::Browser(QString initialPath, QWidget *parent) :
    QObject{parent},
    m_pages(),
    m_page(nullptr)
{
    noTrailing(initialPath);

    m_crumbs = new CrumbBar(this, nullptr);
    m_hist = new BrowserHistory(initialPath, this);
    m_navBut = new Aero::NavButtons(nullptr);   // FIXME: delete these upon destruction if they don't get parented by the app

    connect(m_navBut->back(), &QAbstractButton::clicked, m_hist, &BrowserHistory::goBack);
    connect(m_navBut->forward(), &QAbstractButton::clicked, m_hist, &BrowserHistory::goForward);

    bind_prop(m_hist, "canBack", m_navBut->back(), "enabled", &BrowserHistory::positionChanged, true);
    bind_prop(m_hist, "canForward", m_navBut->forward(), "enabled", &BrowserHistory::positionChanged, true);

    connect(m_hist, &BrowserHistory::positionChanged, [=]() {
        delete m_navBut->menu();

        m_navBut->setMenu(new QMenu(m_navBut) + also {
            const auto h = m_hist->history();

            if (h.length() >= 2)
                for (auto path = h.rbegin() + 1; path != h.rend(); ++path)
                    it->addAction(actionForPath(*path));
        });
    } + also { it(); });
    connect(m_hist, &BrowserHistory::positionChanged, [=]() {
        displayPage(m_hist->currentPath());
    });
    onEvent(m_navBut, QEvent::Polish, [=](auto _) {    // All pages should have been added by then so the path will exist
        displayPage(initialPath);
    });

    m_frame = new QFrame + also {
        it->setAutoFillBackground(true);
    };

    onEvent<QMouseEvent>(m_frame, QEvent::MouseButtonPress, [=](QMouseEvent *ev) {
        // Mouse side buttons: XButton1 = back, XButton2 = forward.
        if (ev->button() == Qt::XButton1) { m_hist->goBack();    ev->accept(); }
        if (ev->button() == Qt::XButton2) { m_hist->goForward(); ev->accept(); }
    });
}

QAction *Browser::addPage(QString path, MakeFn makePage)
{
    noTrailing(path);

    if (path != "" && !path.startsWith("/"))
        return nullptr;

    QAction *act;

    m_pages[path] = PgInfo {
        act = new QAction(this) + also {
            it->setEnabled(makePage != nullptr);
        },
        makePage
    };

    if (makePage)
        connect(act, &QAction::triggered, [=]() {
            this->navigateTo(path);
        });

    return act;
}

QAction *Browser::actionForPath(QString path)
{
    noTrailing(path);

    if (path != "" && !path.startsWith("/"))
        return nullptr;

    if (m_pages.contains(path))
        return m_pages[path].act;

    return nullptr;
}

bool isChild(QString path, QString of)
{
    noTrailing(path);
    noTrailing(of);

    if (path == of)
        return false;

    return path.left(path.lastIndexOf("/")) == of;
}

QList<QAction *> Browser::children(QString basePath)
{
    noTrailing(basePath);

    QList<QAction *> out;

    for (auto it = m_pages.begin(); it != m_pages.end(); ++it) {
        if (isChild(it.key(), basePath))
            out += it.value().act;
    }

    return out;
}

QList<QAction *> Browser::children(QAction *act)
{
    for (auto it = m_pages.begin(); it != m_pages.end(); ++it)
        if (it.value().act == act)
            return children(it.key());

    return {};
}

bool Browser::navigateTo(QString _path)
{
    noTrailing(_path);

    if (displayPage(_path)) {
        m_hist->recordPath(_path);

        return true;
    }
    else {
        auto [ path, args ] = separateArgs(QUrl("tags:" + _path));
        Q_EMIT pageNotFound(path, args);

        return false;
    }
}

bool Browser::displayPage(QString _path)
{
    auto [ path, args ] = separateArgs(QUrl("tags:" + _path));

    if (m_pages.contains(path)) {
        if (!m_pages[path].make)
            return false;

        m_page = replaceFrameChild(m_frame, m_pages[path].make(args));
        m_crumbs->setPath(path);

        Q_EMIT pageChanged();

        return true;
    }
    else
        return false;
}

void Aero::Browser::reload()
{
    displayPage(path());
}

QWidget *Browser::addressBar()
{
    return m_crumbs;
}

QString Browser::path()
{
    return m_hist->currentPath();
}

QWidget *Browser::page()
{
    return m_page;
}

static QWidget *replaceFrameChild(QFrame *parentFrame, QWidget *newChildWidget) {

    // 1. Get or create the layout manager of the frame
    QLayout *layout = parentFrame->layout();
    if (!layout) {
        // If no layout exists yet, create one (e.g., Vertical Box Layout)
        layout = new QVBoxLayout(parentFrame);
        layout->setContentsMargins(0, 0, 0, 0); // Flush fit
    }

    // 2. Safely remove and delete the old child widget if one exists
    // Layouts use QLayoutItem wrappers around widgets
    if (QLayoutItem *oldItem = layout->takeAt(0)) {
        if (QWidget *oldWidget = oldItem->widget()) {
            oldWidget->hide();
            oldWidget->deleteLater(); // Safe memory reclamation in Qt
        }
        delete oldItem; // Delete the wrapper item structure
    }

    // 3. Mount the brand new child widget into the frame slot
    layout->addWidget(newChildWidget);
    newChildWidget->show();

    return newChildWidget;
}

#include "browser.moc"  // Cuz we define a QObject in a .cpp
