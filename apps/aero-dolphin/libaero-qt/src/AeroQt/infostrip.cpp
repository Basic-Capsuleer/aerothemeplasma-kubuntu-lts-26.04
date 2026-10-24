#include "infostrip.h"

#include <QtWidgets>
#include <KStyle>
#include <AeroQt/util/scopefn.h>
#include <AeroQt/private/themewatcher.h>

#define ICON_SZ QSize(32, 32)

using namespace Aero;
using namespace Aero::Private;

InfoStrip::InfoStrip(QWidget *parent)
    : QWidget{parent}
{
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    QHBoxLayout *lay = new QHBoxLayout + also {
        it->setContentsMargins(16, 10, 16, 10);
        it->setSpacing(16);

        setLayout(it);
    };

    m_iconLabel = new QLabel + also {
        it->setFixedSize(ICON_SZ);
    };
    lay->addWidget(m_iconLabel);

    bind_prop(this, "icon", m_iconLabel, "pixmap", &InfoStrip::iconChanged, true, [=](QVariant _icon) -> QVariant {
        if (QIcon icon = _icon.value<QIcon>(); !icon.isNull())
            return icon.pixmap(ICON_SZ);
        else
            return QPixmap();
    });

    lay->addLayout(m_childLayout = new QHBoxLayout + also {
        it->setContentsMargins(0, 0, 0, 0);
        it->setSpacing(9);
    }, 1);
}

void InfoStrip::paintEvent(QPaintEvent *) {
    // Ask the style to paint us. Ordinary QWidgets make no calls to QStyle.

    QPainter p(this);

    if (amGettingThemed(this)) {
        QStyleOption opt;
        opt.initFrom(this);

        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
    else {
        // Palette shade scale: Dark, Mid(dark), Button, Midlight, Light

        p.fillRect(rect(), palette().color(QPalette::Midlight));

        p.setPen(palette().color(QPalette::Mid));
        p.drawLine(rect().topLeft(), rect().topRight());
    }
}

bool InfoStrip::event(QEvent *event)
{
    if (event->type() == QEvent::Polish) {
        // Collect direct children with the "_ui_childRole" == "contents" property
        const auto childrenList = findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);

        for (auto *child : childrenList) {
            if (child->property("_ui_childRole").toString() == "contents") {
                // Ensure it isn't already managed by m_childLayout
                if (m_childLayout->indexOf(child) == -1) {
                    // Reset static geometries injected by setupUi()
                    child->setGeometry(QRect());
                    child->setMinimumSize(QSize(0, 0));
                    child->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
                    child->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

                    m_childLayout->addWidget(child, 1);
                }
            }
        }

        if (this->layout()) {
            this->layout()->invalidate();
            this->layout()->activate();
        }
    }

    return QWidget::event(event);
}
