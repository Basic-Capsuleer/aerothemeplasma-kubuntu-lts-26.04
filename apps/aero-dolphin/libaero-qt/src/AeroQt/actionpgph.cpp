#include "actionpgph.h"

#include <QtWidgets>
#include <KUrlLabel>
#include <QGridLayout>

#include <AeroQt/private/misc.h>
#include <AeroQt/util/deepbind.h>

#include <functional>

namespace Aero {

ActionPgph::ActionPgph(QWidget *parent) :
    KTitleWidget{parent},
    m_titleAction(nullptr),
    m_lastSep(0),
    m_form(nullptr)
{
    QLabel *title = findChild<QLabel *>();

    setStyleSheet("KTitleWidget { color: #1E7B1E; }");
    underlineOnHover(title, [=]{ return m_titleAction != nullptr; }, QColor("#1E7B1E"), QColor("#1E7B1E"));

    onEvent(title, QEvent::MouseButtonRelease, [=](QEvent *) {
        if (m_titleAction)
            m_titleAction->trigger();
    });

    // Chidren

    m_internalGrid = findChild<QGridLayout *>();
    m_internalGrid->setHorizontalSpacing(8);

    //

    m_widg = new QWidget(this);
    m_widg->setLayout(m_flow = new FlowLayout());
    setIcon(QIcon());   // This places m_widg

    DeepSource(this)
        .prop("titleAction", &ActionPgph::titleActionChanged)
        .prop("text", &QAction::changed)
        .bind(this, "text");
    DeepSource(this)
        .prop("titleAction", &ActionPgph::titleActionChanged)
        .prop("toolTip", &QAction::changed)
        .bind(this, "toolTip");

    connect(this, &ActionPgph::titleActionChanged, [=] {
        if (titleAction())
            setIcon(titleAction()->icon());
    });
}

ActionPgph::ActionPgph(QString text, QIcon icon, bool longText, QWidget *parent) :
    ActionPgph(parent)
{
    setText(text);
    setIcon(icon);

    if (longText) {
        delete m_widg->layout();

        m_widg->setLayout(m_form = new QFormLayout + also {
            it->setLabelAlignment(Qt::AlignTop | Qt::AlignHCenter);
        });
    }
}

void ActionPgph::setIcon(const QIcon &icon)
{
    KTitleWidget::setIcon(icon, KTitleWidget::ImageLeft);

    m_internalGrid->removeWidget(m_widg);

    if (!icon.isNull()) {
        m_internalGrid->addWidget(m_widg, 1, 1);
    }
    else {
        m_internalGrid->addWidget(m_widg, 1, 1);

        qobject_cast<QLabel *>(m_internalGrid->itemAtPosition(0, 0)->widget())
            ->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    }
}

static QWidget *makeSep(QWidget *p = nullptr) {
    auto *s = new QLabel("|", p);
    QFont f = s->font();
    f.setPointSize(9);
    s->setFont(f);
    s->setStyleSheet("color: #B0B0B0;");
    return s;
}

void ActionPgph::actionEvent(QActionEvent *ev)
{
    auto *ac = ev->action();

    if (ev->type() == QEvent::ActionAdded) {
        auto *icon = new QLabel(m_widg);
        bind_prop(ac, "icon", icon, "pixmap", &QAction::changed, true, [=](QVariant icon) -> QVariant {
            QIcon ic;

            if (!icon.value<QIcon>().isNull())
                ic = icon.value<QIcon>();
            else if (auto p = ac->property("elevatedPriv"); !p.isNull() && p.value<bool>())
                ic = style()->standardIcon(QStyle::SP_VistaShield);

            return ic.pixmap(QSize(16, 16));
        });
        connect(ac, &QAction::changed, icon, [=]() {
            icon->setVisible(!ac->icon().isNull());
        } + also { it(); });

        auto *link = new QLabel(m_widg) + also {
            it->setWordWrap(longText());
            it->setTextFormat(Qt::RichText);    // tooltip text may have <i> etc
        };

        underlineOnHover(link, [=]{ return true; }, QColor("#0033AA"), QColor("#1F4E99"));

        // link->setPalette(link->palette() + also {
        //     it.setColor(QPalette::Link, QColor("#1F4E99"));
        // });

        if (!longText()) {
            bind_prop(ac, "text", link, "text", &QAction::changed, true, [=](QVariant inp) -> QVariant {
                return inp.toString().toHtmlEscaped();
            });
            bind_prop(ac, "toolTip", link, "toolTip", &QAction::changed, true, [=](QVariant icon) -> QVariant {
                return actualToolTip(ac);
            });
        }
        else {
            bind_prop(ac, "toolTip", link, "text", &QAction::changed, true);
        }

        onEvent(link, QEvent::MouseButtonRelease, [=](QEvent *) { ac->trigger(); });

        if (!longText()) {
            auto *lay = new QHBoxLayout + also {
                it->setSpacing(4);
            };

            lay->addWidget(icon);
            lay->addWidget(link + also { it->show(); });

            if (m_lastSep)
                m_lastSep->show();
            lay->addWidget(m_lastSep = makeSep(m_widg) + also { it->hide(); });

            m_flow->addItem(lay);
        }
        else {
            m_form->addRow(icon, link);
        }
    }
    else if (ev->type() == QEvent::ActionRemoved) {
    }
}

} // namespace Aero
