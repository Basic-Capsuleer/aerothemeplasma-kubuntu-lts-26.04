#pragma once

#include <QLabel>
#include <functional>

void underlineOnHover(QLabel *link, std::function<bool()> shouldUnderline, QColor normal, QColor hot);
QString actualToolTip(QAction *ac);

QPixmap arrowPixmap(Qt::ArrowType dir,
                    const QColor &color = QColor(0x3C, 0x3C, 0x3C),
                    int width = 7);
