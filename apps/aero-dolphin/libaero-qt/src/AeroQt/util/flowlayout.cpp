#include <QtWidgets>
#include "flowlayout.h"

FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing)
    : m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0))) {
        // Automatically delete the sub-items/sub-layouts when the FlowLayout is destroyed
        if (item->layout()) {
            QLayoutItem *child;
            while ((child = item->layout()->takeAt(0))) {
                if (child->widget())
                    child->widget()->deleteLater();
                delete child;
            }
            item->layout()->deleteLater();
        } else if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

void FlowLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);

    // If a parent widget exists for this layout, automatically adopt the new item
    // so it enters the proper Qt object and visual hierarchy.
    if (QWidget *pw = parentWidget()) {
        if (QWidget *wid = item->widget()) {
            wid->setParent(pw);
        } else if (QLayout *lay = item->layout()) {
            // If it's a nested layout, recursively parent all its child widgets
            for (int i = 0; i < lay->count(); ++i) {
                if (QWidget *childWid = lay->itemAt(i)->widget()) {
                    childWid->setParent(pw);
                }
            }
        }
    }
}

int FlowLayout::horizontalSpacing() const
{
    if (m_hSpace >= 0) {
        return m_hSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

int FlowLayout::verticalSpacing() const
{
    if (m_vSpace >= 0) {
        return m_vSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}

int FlowLayout::count() const
{
    return itemList.size();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    return itemList.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    return nullptr;
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return { };
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    return doLayout(QRect(0, 0, width, 0), true);
}

void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    QSize size;
    for (const QLayoutItem *item : std::as_const(itemList))
        size = size.expandedTo(item->minimumSize());

    const QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

int FlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    for (QLayoutItem *item : std::as_const(itemList)) {
        const QWidget *wid = item->widget();

        // Null-safe layout spacing fallback for nested sub-layouts
        int spaceX = horizontalSpacing();
        if (spaceX == -1) {
            spaceX = (wid) ? wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal)
                           : smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
        }

        int spaceY = verticalSpacing();
        if (spaceY == -1) {
            spaceY = (wid) ? wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical)
                           : smartSpacing(QStyle::PM_LayoutVerticalSpacing);
        }

        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    } else {
        return static_cast<QLayout *>(parent)->spacing();
    }
}
