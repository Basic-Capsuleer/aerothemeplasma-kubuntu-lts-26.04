#include "section.h"

#include "util/scopefn.h"

#include <QtWidgets>
#include <QStyleOptionComboBox>

using namespace Aero;

class ChevronButton : public QToolButton {
public:
    explicit ChevronButton(bool interactive = true, QWidget *parent = nullptr)
        : QToolButton(parent)
    {
        setCheckable(true);
        setCursor(Qt::PointingHandCursor);
        setFixedSize(20, 20);
        setFocusPolicy(Qt::NoFocus);
        if (!interactive)
            setAttribute(Qt::WA_TransparentForMouseEvents);
        setStyleSheet(
            "QToolButton { border: 1px solid #C0CEDA; border-radius: 10px;"
            " background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            " stop:0 #FDFEFF, stop:1 #E8F1F8); }"
            "QToolButton:hover { background: #E4EEF7; }"
            "QToolButton:pressed { background: #D6E4F0; }");
    }

protected:
    void paintEvent(QPaintEvent *e) override
    {
        QToolButton::paintEvent(e);   // the stylesheet's round bevel

        QPainter p(this);
        QStyleOptionComboBox o;
        o.initFrom(this);

        o.rect = QRect(QPoint(), rect().center()) + also {
            it.moveCenter(rect().center());
        };

        o.palette.setColor(QPalette::WindowText, QColor("#1F4E99"));
        o.palette.setColor(QPalette::ButtonText, QColor("#1F4E99"));

        style()->drawPrimitive(
            isChecked() ? QStyle::PE_IndicatorArrowUp : QStyle::PE_IndicatorArrowDown,
            &o,
            &p
        );
    }
};

Section::Section(QWidget *parent) :
    QGroupBox{parent},
    m_header(nullptr)
{
    // We can't actually make the header until we are about to be shown because some non-listenable properties that might be set in code after the constructor are needed to determine how to create it.
    onEvent(this, QEvent::Polish, [=](QEvent *) {
        m_header = makeHeader();
        setContentsMargins(QMargins(0, m_header->height(), 0, 0));

        if (!isCheckable() || isChecked())
            onUncollapse();
        else
            onCollapse();
    });

    onEvent(this, QEvent::Resize, [=](QEvent *) {
        if (m_header)
            m_header->setGeometry(this->rect() + also {
                it.setHeight(m_header->height());
            });
    });

    connect(this, &QGroupBox::toggled, [=] {
        if (isChecked())
            onUncollapse();
        else
            onCollapse();

        updateGeometry();
    });
}

void Section::toggleCollapsed()
{
    setChecked(!isChecked());
}

void Section::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event); // Skip the GroupBox frame & title painting
}

void Section::onCollapse()
{
    for(auto *w: findChildren<QWidget *>(Qt::FindDirectChildrenOnly))
        if (w != m_header)
            w->hide();

    m_header->setEnabled(true); // QGroupBox disables the header when unchecked
}

void Section::onUncollapse()
{
    for(auto *w: findChildren<QWidget *>(Qt::FindDirectChildrenOnly))
        if (w != m_header)
            w->show();
}

QWidget *Section::makeHeader()
{
    bool is_big = this->metaObject() != &SubSection::staticMetaObject;

    auto *header = new QWidget(this) + also {
        if (isCheckable())
            it->setCursor(Qt::PointingHandCursor);
        it->setMinimumHeight(36);
    };

    onEvent<QMouseEvent>(header, QEvent::MouseButtonRelease, [=](QMouseEvent *ev) {
        if (ev->button() == Qt::LeftButton)
            toggleCollapsed();
    });

    auto *headerH = new QHBoxLayout(header) + also {
        it->setContentsMargins(0, 4, 8, 4);
        it->setSpacing(6);
    };

    headerH->addWidget(
        new QLabel() + also {
            if (is_big) {
                it->setFont(it->font() + also {
                    it.setPointSize(13);
                });
                it->setStyleSheet("color: #1A5FB4");
            }

            it->setText(title());
        },
        0,
        Qt::AlignVCenter
    );

    if (is_big)
        headerH->addStretch(1);
    else {
        headerH->addWidget(new QLabel + also {
            onEvent(it, QEvent::Paint, [=](QEvent *) {
                QPainter p(it);

                p.setPen(palette().color(QPalette::Mid));

                QPoint start = it->rect().bottomLeft() / 2;
                p.drawLine(start, start + QPoint(it->rect().width(), 0));
            });
        }, 1, Qt::AlignVCenter);
    }

    if (isCheckable())
        headerH->addWidget(
            new ChevronButton(/*interactive=*/false) + also {
                bind_prop(this, "checked", it, "checked", &QGroupBox::toggled, true);
            },
            0,
            Qt::AlignVCenter
        );

    // Underline
    if (is_big)
        onEvent(header, QEvent::Paint, [=](QEvent *) {
            QPainter p(header);

            p.setPen(QColor("#DCDCDC"));
            p.drawLine(header->rect().bottomLeft(), header->rect().bottomRight());
        });

    return header;
}

SubSection::SubSection(QWidget *parent) : Section(parent) {}
