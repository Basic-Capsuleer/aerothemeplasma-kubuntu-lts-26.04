#ifndef INFOSTRIP_H
#define INFOSTRIP_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>
#include <AeroQt/util/props.h>

namespace Aero {

class InfoStrip : public QWidget
{
    /// Usage as a promoted widget from .ui files: Add a child widget without a layout, and give it a dynamic property "_ui_childRole": "contents"

    Q_OBJECT

    QLabel *m_iconLabel;
    QHBoxLayout *m_childLayout;

    MAKE_PROPERTY(QIcon, icon, Icon);

public:
    explicit InfoStrip(QWidget *parent = nullptr);

    inline QHBoxLayout *childLayout() { return m_childLayout; }

Q_SIGNALS:

protected:
    void paintEvent(QPaintEvent *) override;
    bool event(QEvent *event) override;   // To support adding widgets from .ui files. Add a child widget without a layout, and give it a dynamic property "_ui_childRole": "contents"
};

} // namespace Aero

#endif // INFOSTRIP_H
