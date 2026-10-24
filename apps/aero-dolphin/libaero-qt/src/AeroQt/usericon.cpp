#include "usericon.h"

#include <QPainter>
#include <KStyle>
#include <QStyleOption>

#include <AeroQt/util/scopefn.h>
#include <AeroQt/util/objecteventlistener.h>
#include <AeroQt/private/themewatcher.h>

using namespace Aero;
using namespace Aero::Private;

UserIcon::UserIcon(Size sz, QWidget *parent)
    : Private::ImageButton(
        sz == Size::Size_48 ? ":/AeroQt/UserIcon/80.normal.png" : ":/AeroQt/UserIcon/100.normal.png",
        sz == Size::Size_48 ? ":/AeroQt/UserIcon/80.hot.png"    : ":/AeroQt/UserIcon/100.hot.png"   ,
        sz == Size::Size_48 ? ":/AeroQt/UserIcon/80.active.png" : ":/AeroQt/UserIcon/100.active.png",
        sz == Size::Size_48 ? ":/AeroQt/UserIcon/80.normal.png" : ":/AeroQt/UserIcon/100.normal.png",
        50,
        parent
    )
{
    m_sz = sz;

    connect(this, &UserIcon::iconChanged, [=]() {
        update();
        m_pixmap = icon().pixmap((m_sz == Size::Size_48) ? QSize(48, 48) : QSize(60, 60));
    });
}

void UserIcon::paintEvent(QPaintEvent *pe)
{
    auto imgMarg = (m_sz == Size::Size_48) ? QMargins(16, 15, 16, 17) : QMargins(20, 19, 20, 21);
    auto iconRect = rect().marginsRemoved(imgMarg);

    QPainter p(this);

    if (amGettingThemed(this)) {
        p.fillRect(iconRect, Qt::white);
        p.drawPixmap(iconRect, m_pixmap);
        Private::ImageButton::paintEvent(pe);
    }
    else {
        QStyleOptionButton opt;
        opt.initFrom(this);
        opt.state |= QStyle::State_Raised;
        opt.rect = iconRect + QMargins(7, 7, 7, 7);

        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);

        opt.state = QStyle::State_Sunken;
        opt.rect = iconRect;
        opt.palette = opt.palette + also {
            // it.setColor(QPalette::Button, it.color(QPalette::Mid));
        };

        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);

        p.drawPixmap(iconRect, m_pixmap);
    }
}

