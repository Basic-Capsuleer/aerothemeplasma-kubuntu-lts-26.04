#include "dolphinwindowheader.h"

#include <QFontMetrics>

DolphinWindowHeader::DolphinWindowHeader(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DolphinWindowHeader)
{
    ui->setupUi(this);

    ui->searchBar->addAction(ui->actSearchOpts, QLineEdit::TrailingPosition);

    /* Force all three textboxes to be the same height */
    ui->primaryNavHole->setContentsMargins(QMargins(0, 0, 0, 0));
    ui->secondaryNavHole->setContentsMargins(QMargins(0, 0, 0, 0));
    /* The original 26 px leaves 18 px for text inside the 4 px borders; with a bigger system font
       the text was cut off, so make the bars as tall as the font needs (at least 26 px) */
    const int barHeight = qMax(26, QFontMetrics(ui->searchBar->font()).height() + 8);
    ui->searchBar->setFixedHeight(barHeight);
    ui->primaryNavHole->setFixedHeight(barHeight);
    ui->secondaryNavHole->setFixedHeight(barHeight);
}

DolphinWindowHeader::~DolphinWindowHeader()
{
    delete ui;
}
