#pragma once

#include <QProxyStyle>
#include <KStyle>

namespace Aero {
namespace Private {
    class MyKStyle;

    class CustomQStyle : public QProxyStyle
	{
		Q_OBJECT

        MyKStyle *m_kstyle;     // Only used for drawing KDE widgets

	public:
		CustomQStyle();

		QIcon standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const override;

		void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
		void drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

		QSize sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *widget = nullptr) const override;

		int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;

        void polish(QWidget *) override;

        static void applyQss(QWidget *widget);
    };

    class MyKStyle : public KStyle
    {
        Q_OBJECT
        Q_CLASSINFO("X-KDE-CustomElements", "true")

    public:
        MyKStyle();

        void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

    private:
        QStyle::ControlElement CE_CapacityBar;
    };
}
}
