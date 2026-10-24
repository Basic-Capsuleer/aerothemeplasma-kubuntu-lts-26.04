#include "page.h"

#include <KTitleWidget>

#include "util/scopefn.h"
#include "util/objecteventlistener.h"

namespace Aero {

Page::Page(QString title, QWidget *parent) :
    Page(parent)
{
    layout()->addWidget(new KTitleWidget() + also {
        it->setText(title);
    });
}

Page::Page(QWidget *parent) :
    QFrame{parent},
    m_layout(0)
{
    setFrameShape(QFrame::NoFrame);

    setAutoFillBackground(true);
    setPalette(palette() + also {
        it.setColor(QPalette::Window, Qt::white);
    });

    onEvent(this, QEvent::Polish, [=](QEvent *) {
        layout();   // Add margins if not there yet
    });
}

QVBoxLayout *Page::layout()
{
    if (!m_layout) {
        if (auto *l = QWidget::layout())
            m_layout = qobject_cast<QVBoxLayout *>(l);      // This whole spiel happens so that a the widget can be embedded in, and its layout set from .ui files
        else
            setLayout(new QVBoxLayout + also {
                it->setContentsMargins(0,0,0,0);
                it->setSpacing(0);

                it->addLayout(m_layout = new QVBoxLayout);
                it->addStretch(1);
            });

        m_layout->setContentsMargins(28, 18, 28, 20);
        m_layout->setSpacing(18);
    }

    return m_layout;
}

} // namespace Aero
