#ifndef USERICON_H
#define USERICON_H

#include <QWidget>

#include "private/imagebutton.h"
#include "util/props.h"

namespace Aero {

class UserIcon : public Private::ImageButton
{
    Q_OBJECT

    MAKE_PROPERTY(QIcon, icon, Icon);

public:
    enum Size {
        Size_48,
        Size_60,
    };

    explicit UserIcon(Size sz, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) override;

    Size m_sz;
    QPixmap m_pixmap;
};

} // namespace Aero

#endif // USERICON_H
