// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "customqstyle.h"

#include <QApplication>

#include <QtWidgets>
#include <QPainter>
#include <QStyleFactory>
#include <QStyleOption>
#include <QFile>

#include <KStyle>
#include <KStyleExtensions>
#include <KMessageWidget>

#include <iostream>

#include "../wizard.h"
#include "../branding.h"
#include "../util/image9.h"
#include "../util/scopefn.h"
#include "aerothemesettings.h"
#include "themewatcher.h"

using namespace Aero;
using namespace Aero::Private;

CustomQStyle::CustomQStyle()
    : QProxyStyle("kvantum")
{
    m_kstyle = new MyKStyle;
    m_kstyle->setParent(this);
}

QIcon CustomQStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *option, const QWidget *_w) const
{
	if (standardIcon == SP_CommandLink) {
		// Unfortunately Kvantum mutates this into an SP_ArrowRight, which just isn't the same under aero. So we'll have to hard code it until we get bOrealis where we can then use Button/COMMANDLINKGLYPH from the MSStyle
		// https://github.com/tsujan/Kvantum/blob/dc344a719fad3beea9225c250027f06e40f609c1/Kvantum/style/standardIcons.cpp#L383

		if (option->state & QStyle::State_Sunken)
			return QIcon(":/AeroQt/commandLinkButton/glyph/pressed.png");
		else if (!(option->state & QStyle::State_Enabled))
			return QIcon(":/AeroQt/commandLinkButton/glyph/disabled.png");
		else
			return QIcon(":/AeroQt/commandLinkButton/glyph/normal.png");
	}
    if (standardIcon == SP_DialogCloseButton) {
        return QIcon::fromTheme("window-close");
    }
    if (standardIcon == SP_VistaShield)     // elevated priviledge shield
        return QIcon::fromTheme("gtk-dialog-authentication");

	return QProxyStyle::standardIcon(standardIcon, option, _w);
}

void CustomQStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *_w) const
{
    if (_w) {
        if (element == QStyle::PE_Widget) {
            if (option == &_amGettingThemed_dummy) {    // used by amGettingThemed()
                _amGettingThemed_dummy.version = -10;

                return;
            }
            if (_w->inherits("Aero::InfoStrip")) {
                static Image9 texture = Image9() + also {
                    it.loadAndroidFormat(QImage(":/AeroQt/infostrip.9.png"));
                };

                texture.paint(painter, option->rect);
                return;
            }
            if (_w->parentWidget() && _w->parentWidget()->inherits("Aero::Sidebar")) {

                auto rect = option->rect;
                auto build = Branding::reportedBuild();

                /* Gradient */

                const char *color1, *color2;

                if		(build < 6000) { color1 = "#80a0da"; color2 = "#6275d5"; }
                else if (build < 7000) { color1 = "#416fab"; color2 = "#6bb76c"; }
                else if (build < 7600) { color1 = "#315C89"; color2 = "#315C89"; }
                else if (build < 8400) { color1 = "#dce6f4"; color2 = "#e9f3ff"; }
                else				   { color1 = "#dce6f4"; color2 = "#e9f3ff"; }

                painter->fillRect(
                    rect,
                    QLinearGradient(rect.topLeft(), rect.bottomLeft()) + also {
                        it.setColorAt(0.0, QColor(color1));
                        it.setColorAt(1.0, QColor(color2));
                    }
                );

                /* Overlay */

                if (build < 6000) {
                }
                else if (build < 7000) {
                    Image9 overlay {
                        QImage(":/AeroQt/sidebar/6000.632.png"),
                        QMargins(200, 0, 0, 331),
                        true, true
                    };

                    overlay.paint(painter, rect);
                }
                else if (build < 7600) {
                    Image9 overlay {
                        QImage(":/AeroQt/sidebar/7000.633.png"),
                        QMargins(0, 0, 199, 550)
                    };

                    overlay.paint(painter, rect);
                }
                else if (build < 8400) {
                    Image9 overlay {
                        QImage(":/AeroQt/sidebar/7600.632.png"),
                        QMargins(0, 542, 0, 0),
                        true, false
                    };

                    overlay.paint(painter, rect);
                }
                else {
                }

                return;
            }
        }
    }

    QProxyStyle::drawPrimitive(element, option, painter, _w);
}

void CustomQStyle::drawControl(QStyle::ControlElement controlElement, const QStyleOption *option, QPainter *painter, const QWidget *_w) const
{
	// if (controlElement == CE_PushButton) {
		// const auto buttonOption = static_cast<const QStyleOptionButton *>(option);

		// auto element = Union::Element::create();
		// element->setType(QStringLiteral("Button"));
		// element->setStates(statesFromOption(option));
		// element->setColorSet(Union::Element::ColorSet::Button);
		// element->setHint(QStringLiteral("flat"), buttonOption->features.testFlag(QStyleOptionButton::ButtonFeature::Flat));

		// const auto style = Union::StyleRegistry::instance()->defaultStyle();
		// const auto query = std::make_unique<Union::ElementQuery>(style);

		// query->setElements({element});
		// query->execute();

		// const auto properties = query->properties();

		// // Shrink the widget rect by the insets
		// // TODO: we may have to keep it as a QRectF here
		// QRect rect = option->rect;
		// if (const auto layout = properties->layout()) {
		//	   if (layout->inset()) {
		//		   rect -= layout->inset()->toMargins().toMargins();
		//	   }
		// }

		// // Make sure to take out the space left for the visual focus rect
		// rect.setLeft(rect.left() + 2);
		// rect.setRight(rect.right() - 2);
		// rect.setTop(rect.top() + 2);
		// rect.setBottom(rect.bottom() - 2);

		// drawBackground(painter, rect, properties);

		// QStyleOptionButton labelOption(*buttonOption);
		// labelOption.palette.setColor(QPalette::ButtonText, properties->text()->color().value().toQColor());

		// Draw label
		// TODO: union-ize better
	//	   QProxyStyle::drawControl(CE_PushButtonLabel, option, painter, widget);

	//	   return;
	// }
	QProxyStyle::drawControl(controlElement, option, painter, _w);
}

QSize CustomQStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *widget) const
{
	return QProxyStyle::sizeFromContents(ct, opt, contentsSize, widget);
}

int CustomQStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *_w) const
{
	if (_w) {
		if (_w->inherits("QCommandLinkButton") && metric == QStyle::PM_ButtonShiftHorizontal) // https://code.qt.io/cgit/qt/qtbase.git/tree/src/widgets/widgets/qcommandlinkbutton.cpp#n177
			return 0;
	}

	return QProxyStyle::pixelMetric(metric, option, _w);
}

void CustomQStyle::applyQss(QWidget *widget)
{
    static QString qss;

    if (qss.isEmpty()) {
        QFile f(":/AeroQt/style.css");
        f.open(QFile::ReadOnly | QFile::Text);
        qss = f.readAll();
    }

    if (widget && widget->styleSheet().isEmpty())
        widget->setStyleSheet(qss);
}

static int nthChild(QLayout *ly, QWidget *that, const char *className, int *out_totalCount)	 /// Includes any derivatives of the class in the search
{
	int n = -1;
	*out_totalCount = 0;

	for (int i = 0; i < ly->count(); i++) {
		if (QWidget *child = ly->itemAt(i)->widget()) {
			if (child->inherits( className)) {
				if (child == that)
					n = *out_totalCount;

				(*out_totalCount)++;
			}
		}
	}

	return n;
}

void CustomQStyle::polish(QWidget *_w)
{
	QProxyStyle::polish(_w);

	if (!_w) return;

    /* Apply QSS to widgets that it affects.
     * We don't want to set an application-wide stylesheet as that would disable things like widgets honoring their QPalette */

    for (auto className : {
        "QCommandLinkButton",
        "QToolBar",
        // "Aero::Page",
        "SARibbonWidget",
        "SARibbonBar"
    })
        if (_w->inherits(className))
            applyQss(_w);

    if (auto *w = qobject_cast<QWizard *>(_w))
           Aero::Wizard::convert(w);	// FIXME: ::polish() is too late to call Aero::Wizard::convert(). Needs to be called around constructor time

    if (_w->metaObject()->className() == QString("KTitleWidget")) {
        QTimer::singleShot(0, _w, [=]{
            _w->setStyleSheet("KTitleWidget { color: #1A5FB4; }");
        });

        for (auto *label : _w->findChildren<QLabel *>(Qt::FindDirectChildrenOnly))
            label->setTextInteractionFlags(label->textInteractionFlags() & ~(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard));
    }

	if (_w->inherits("QCommandLinkButton")) {
		_w->setProperty("_qt_usingVistaStyle", true);	// https://code.qt.io/cgit/qt/qtbase.git/tree/src/widgets/widgets/qcommandlinkbutton.cpp#n176
	}

	if (_w->inherits("SARibbonButtonGroupWidget")) {
		_w->layout()->setSpacing(0);
		_w->layout()->setContentsMargins(QMargins());
	}

	if (_w->inherits("SARibbonPanel")) {
		_w->layout()->setSpacing(2);
		_w->layout()->setContentsMargins(QMargins(2, 2, 2, 2));
	}

    if (_w->inherits("KMessageWidget")) {
        auto *w = qobject_cast<KMessageWidget *>(_w);

        // Paint
        overrideEvent(_w, QEvent::Paint, [=](QEvent *) {
            QPainter p(w);

            // QStyleOption opt;
            // opt.initFrom(w);
            // w->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, w);

            p.fillRect(w->rect(), QColor("#FFFFE1"));

            p.setPen(QColor("#E3C86B"));
            if (w->position() == KMessageWidget::Footer)
                p.drawLine(w->rect().topLeft(), w->rect().topRight());
            else if (w->position() == KMessageWidget::Header)
                p.drawLine(w->rect().bottomLeft(), w->rect().bottomRight());
        });

        // Not selectable
        for (QLabel *label : w->findChildren<QLabel *>())
            label->setTextInteractionFlags(label->textInteractionFlags() & ~(Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard));

        // Change padding
        if (auto *hbox = _w->findChild<QHBoxLayout *>())
            hbox->setContentsMargins(hbox->contentsMargins() + also {
                it.setTop(1);
                it.setBottom(1);
            });
    }

    if (_w->metaObject()->className() == QString("KTitleWidget")) {
        // _w->setPalette(_w->palette() + also {
        //     it.setColor(QPalette::WindowText, QColor("#1A3C7A"));
        // });
        // _w->setStyleSheet("KTitleWidget { color: #1A3C7A; }");
    }

	/* Add properties used by QSS rules */

	if (_w->parentWidget() && _w->parentWidget()->inherits("SARibbonButtonGroupWidget") && _w->inherits("QToolButton")) {
		int total, posN = nthChild(_w->parentWidget()->layout(), _w, "QToolButton", &total);

		if (posN == 0)
			_w->setProperty("_Aero_childPosition", "first");
		else if (posN == total - 1)
			_w->setProperty("_Aero_childPosition", "last");
		else if (posN > 0)
			_w->setProperty("_Aero_childPosition", "middle");
	}

	/* Styling the toolbar for specific apps */

	if (_w->inherits("QToolBar")) {
		static QList<ToolbarRule *> tbRules = AeroThemeSettings::self()->toolbarOverrides();	// FIXME: gotta free the pointers at some pt

		for (auto *rule: tbRules)
			if (rule->appName() == qApp->applicationName())
				if (rule->objName().isEmpty() || rule->objName() == _w->objectName()) {
					if (!rule->color().isEmpty())
						_w->setProperty("_Aero_styleclass", rule->color());
					if (rule->flat())
						_w->setProperty("_Aero_flat", "true");
				}
	}

    /* Make KDE widgets that use KStyle custom elements use the puppet KStyle instead of this QStyle */
    for (auto className : {
        "KCapacityBar",
    }) {
        if (_w->inherits(className)) {
            _w->setStyle(m_kstyle);

            QEvent evt(QEvent::StyleChange);
            QCoreApplication::sendEvent(_w, &evt);
        }
    }
}

MyKStyle::MyKStyle()
    : KStyle()
{
    CE_CapacityBar = newControlElement("CE_CapacityBar");
}

void MyKStyle::drawControl(ControlElement element, const QStyleOption *_opt, QPainter *painter, const QWidget *widget) const
{

    if (element == CE_CapacityBar) {
        auto *opt = qstyleoption_cast<const QStyleOptionProgressBar *>(_opt);

        static Image9 bg    = Image9() + also {
            it.loadAndroidFormat(QImage(":/AeroQt/KCapacityBar/bg.9.png"));
        };
        static Image9 blue  = Image9() + also {
            it.loadAndroidFormat(QImage(":/AeroQt/KCapacityBar/blue.9.png"));
        };
        static Image9 red   = Image9() + also {
            it.loadAndroidFormat(QImage(":/AeroQt/KCapacityBar/red.9.png"));
        };

        bg.paint(painter, _opt->rect);

        Image9& fill = opt->progress < 90 ? blue : red;
        QRect contents = _opt->rect.marginsRemoved(bg.margins);
        QRect fillRect = QRect(contents) + also {
            it.setWidth((opt->progress * contents.width()) / 100);
        };

        fill.paint(painter, fillRect);

        return;
    }

    KStyle::drawControl(element, _opt, painter, widget);
}
