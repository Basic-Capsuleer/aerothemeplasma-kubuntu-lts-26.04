#include "stylesheet.h"
#include "private/customqstyle.h"

namespace Aero {

void registerStylesheet(QApplication *app)
{
    if (app->style()->name() != "borealis") // Borealis supplements the QSS stylesheet that's used under libaeroqt-kvantum
        app->setStyle(new Private::CustomQStyle());
}

} // namespace Aero
