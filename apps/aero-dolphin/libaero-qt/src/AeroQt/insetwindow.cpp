#include <QtWidgets>

#ifdef Q_OS_LINUX
#include <KF6/KWindowSystem/kwindoweffects.h>
#elif defined(Q_OS_WIN)
#include <windows.h>
#include <dwmapi.h>
#endif

#include "insetwindow.h"
#include "private/themewatcher.h"
#include "private/customqstyle.h"
#include "util/objecteventlistener.h"

namespace Aero {
using namespace Private;

static void shouldDrawFrame(QWidget *that, QWidget *top, QWidget *bottom, bool *drawN, bool *drawE, bool *drawS, bool *drawW)
{
	*drawN = !that->isFullScreen() || top;
	*drawW = !(that->isMaximized() || that->isFullScreen());
	*drawE = !(that->isMaximized() || that->isFullScreen());
	*drawS = (!(that->isMaximized() || that->isFullScreen())) || bottom;
}

static void drawWindowFrame(QWidget *that, QRect rect, QWidget *top, QWidget *bottom)
{
	bool drawN, drawE, drawS, drawW;
	shouldDrawFrame(that, top, bottom, &drawN, &drawE, &drawS, &drawW);

	QPainter painter(that);
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

	painter.fillRect(rect.adjusted(1, 1, 0, 0), that->palette().color(that->backgroundRole()));

	painter.setPen(QPen(QColor(0, 0, 0, 192), 1));
	auto rectB = rect.adjusted(0, 0, 1, 1);

	if (drawW) painter.drawLine(rectB.topLeft()	  ,	 rectB.bottomLeft() ); // left edge
	if (drawN) painter.drawLine(rectB.topLeft()	  ,	 rectB.topRight()	); // top edge
	if (drawE) painter.drawLine(rectB.topRight()  ,	 rectB.bottomRight()); // right edge
	if (drawS) painter.drawLine(rectB.bottomLeft(),	 rectB.bottomRight()); // bottom edge

	painter.setPen(QPen(QColor(255, 255, 255, 127), 1));
	auto rectW = rect.adjusted(-1, -1, 2, 2);

	if (drawW) painter.drawLine(rectW.topLeft()		+ QPoint(0, 1),	 rectW.bottomLeft()	 - QPoint(0, 1)); // left edge
	if (drawN) painter.drawLine(rectW.topLeft()		+ QPoint(1, 0),	 rectW.topRight()	 - QPoint(1, 0)); // top edge
	if (drawE) painter.drawLine(rectW.topRight()	+ QPoint(0, 1),	 rectW.bottomRight() - QPoint(0, 1)); // right edge
	if (drawS) painter.drawLine(rectW.bottomLeft()	+ QPoint(1, 0),	 rectW.bottomRight() - QPoint(1, 0)); // bottom edge
}

static QMargins calcContentsMargins(QWidget *win, QWidget *top, QWidget *bottom)
{
	int b = (ThemeWatcher::instance()->themeIsAero()) ? 2 : 0;	// Inner border width

	bool drawN, drawE, drawS, drawW;
	shouldDrawFrame(win, top, bottom, &drawN, &drawE, &drawS, &drawW);

	return QMargins(
		!drawW ? 0 : b,
		!drawN ? 0 : (b + (top ? top->sizeHint().height() : 0)),
		!drawE ? 0 : b,
		!drawS ? 0 : (b + (bottom ? bottom->sizeHint().height() : 0))
	);
}

void makeInsetWindow(QWidget *that, QWidget *contents, QWidget *top, QWidget *bottom)
{
    CustomQStyle::applyQss(top);
    CustomQStyle::applyQss(bottom);

	/* Tweak inset container widgets */

	if (top) {
		top->setProperty("_Aero_transpbg", QVariant(true));
		onEvent<QMouseEvent>(top, QEvent::MouseButtonPress, [=](QMouseEvent *event) {
			if (event->button() == Qt::LeftButton) {
				that->window()->windowHandle()->startSystemMove();
			}
			// if (event->button() == Qt::RightButton) {
			//	   if (auto w = that->window()->windowHandle())
			//		   w->showSystemMenu(event->globalPosition().toPoint());
			// }
		});
		onEvent<QMouseEvent>(top, QEvent::MouseButtonDblClick, [=](QMouseEvent *event) {
			if (event->button() == Qt::LeftButton) {
				if (that->isMaximized())
					that->showNormal();
				else
					that->showMaximized();

				event->accept();
			}
		});
	}
	if (bottom)
		bottom->setProperty("_Aero_transpbg", QVariant(true));

	/* Restructure window */

	if (top)
	{
		top->setParent(that);

		// Respond to parent resizes
		onEvent<QResizeEvent>(that, QEvent::Resize, [=](QResizeEvent *ev) {
			if (ev->size() != ev->oldSize())
				top->setGeometry(0, 0, ev->size().width(), top->sizeHint().height());
		});

		// Respond to self resizes
		onEvent(top, QEvent::LayoutRequest, [=](QEvent *) {
			top->setGeometry(0, 0, that->size().width(), top->sizeHint().height());
			that->setContentsMargins(calcContentsMargins(that, top, bottom));
		});
	}
	if (bottom)
	{
		bottom->setParent(that);

		// Respond to parent resizes
		onEvent<QResizeEvent>(that, QEvent::Resize, [=](QResizeEvent *ev) {
			if (ev->size() != ev->oldSize())
				bottom->setGeometry(0, ev->size().height() - bottom->sizeHint().height(), ev->size().width(), bottom->sizeHint().height());
		});

		// Respond to self resizes
		onEvent(bottom, QEvent::LayoutRequest, [=](QEvent *) {
			bottom->setGeometry(0, that->size().height() - bottom->sizeHint().height(), that->size().width(), bottom->sizeHint().height());
			that->setContentsMargins(calcContentsMargins(that, top, bottom));
		});
	}

	if (QMainWindow *that_Mw = qobject_cast<QMainWindow *>(that))
	{
	}
	else
	{
		// The drawWindowFrame() is drawn around the contentsRect(). Unless the window has a layout set, that will include the layoutless top and bottom widgets. Hence, we need to add a layout.

		if (that->layout() == nullptr)
		{
			QVBoxLayout* windowLayout = new QVBoxLayout(that);
			windowLayout->setContentsMargins(0, 0, 0, 0);
			windowLayout->setSpacing(0);

			windowLayout->addWidget(contents);
		}
	}

	/* Frame drawing */
	onEvent<QPaintEvent>(that, QEvent::Paint, [=](QPaintEvent *ev) {
		if (ThemeWatcher::instance()->themeIsAero())
		{
			drawWindowFrame(that, that->contentsRect().adjusted(-1, -1, 0, 0), top, bottom);
		}
	});
	onEvent<QWindowStateChangeEvent>(that, QEvent::WindowStateChange, [=](QWindowStateChangeEvent *ev) {	// Some parts of the content frame are not drawn in maximized windows
		that->setContentsMargins(calcContentsMargins(that, top, bottom));
	});

	/* Window attributes */

	auto onThemeChanged = [=](bool isAero) {
        that->setAttribute(Qt::WA_TranslucentBackground, isAero);
        that->setAttribute(Qt::WA_NoSystemBackground, isAero);

		that->setContentsMargins(calcContentsMargins(that, top, bottom));
	};

	onThemeChanged(ThemeWatcher::instance()->themeIsAero());

	QObject::connect(
		ThemeWatcher::instance(),
		&ThemeWatcher::themeChanged,
		that, // Context object, connection ceases when this is destroyed
		onThemeChanged
	);

    onEvent(that, QEvent::Show, [=](QEvent *) {
		// reference: https://gitgud.io/wackyideas/aero-glass-pane/-/blob/master/mainwindow.cpp
        KWindowEffects::enableBlurBehind(that->windowHandle() /*QGuiApplication::topLevelWindows().at(0)*/, true, QRegion(0,0,0,0));
	});
}

} // namespace Aero
