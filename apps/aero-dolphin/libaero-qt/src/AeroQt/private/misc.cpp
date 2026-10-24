#include "misc.h"

#include <QtWidgets>

#include <AeroQt/util/scopefn.h>
#include <AeroQt/util/objecteventlistener.h>

QString actualToolTip(QAction *ac)
{
    if (ac->toolTip() != ac->text())
        return ac->toolTip();
    else
        return QString();
}

void underlineOnHover(QLabel *link, std::function<bool ()> shouldUnderline, QColor normal, QColor hot)
{
    onEvent(link, QEvent::Enter, [=](QEvent *) {
        link->setFont(link->font() + also {
            it.setUnderline(shouldUnderline());
        });
        link->setPalette(link->palette() + also {
            it.setColor(QPalette::WindowText, hot);
        });
    });

    onEvent(link, QEvent::Leave, [=](QEvent *) {
        link->setFont(link->font() + also {
            it.setUnderline(false);
        });
        link->setPalette(link->palette() + also {
            it.setColor(QPalette::WindowText, normal);
        });
    }, true);
}

// TODO: un-vibecode and remove this
QPixmap arrowPixmap(Qt::ArrowType dir, const QColor &color, int width)
{
    // The one arrow glyph used across the app: the small solid triangle Windows 7
    // draws for command-bar dropdowns ("Organize"), view menus, breadcrumbs and
    // expanders. `width` is the triangle's base; the point sits centred on the
    // opposite side with Win7's roughly 7:4 proportions. Rendered at 2x device
    // ratio so the antialiased edges stay crisp.

    const int w = width;
    const int h = qMax(3, (w * 4 + 3) / 7);
    const bool vertical = (dir == Qt::UpArrow || dir == Qt::DownArrow);
    const QSize size = vertical ? QSize(w, h) : QSize(h, w);

    QPixmap pm(size * 2);
    pm.setDevicePixelRatio(2);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(color);

    QPolygonF poly;
    switch (dir) {
    case Qt::UpArrow:
        poly << QPointF(0, h) << QPointF(w, h) << QPointF(w / 2.0, 0);
        break;
    case Qt::LeftArrow:
        poly << QPointF(h, 0) << QPointF(h, w) << QPointF(0, w / 2.0);
        break;
    case Qt::RightArrow:
        poly << QPointF(0, 0) << QPointF(0, w) << QPointF(h, w / 2.0);
        break;
    case Qt::DownArrow:
    default:
        poly << QPointF(0, 0) << QPointF(w, 0) << QPointF(w / 2.0, h);
        break;
    }
    p.drawPolygon(poly);
    return pm;
}
