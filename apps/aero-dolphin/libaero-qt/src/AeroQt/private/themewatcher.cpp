#include "themewatcher.h"

#include <KSharedConfig>
#include <QWidget>
#include <QPainter>
#include <QStyleOption>

#include <iostream>

#include "customqstyle.h"

namespace Aero {
namespace Private {

static bool comboIsAero(QString library, QString theme)
{
	// Aero:	org.smod.smod	SMOD
	// Reactionary: org.kde.kwin.aurorae	__aurorae__svg__Reactionary

	return true;//(library == "org.smod.smod" && theme == "SMOD");
}

ThemeWatcher::ThemeWatcher(QObject *parent)
	: QObject{parent}
{
	auto config = KSharedConfig::openConfig("kwinrc");
	m_watcher = KConfigWatcher::create(config);

	QObject::connect(
		m_watcher.get(), &KConfigWatcher::configChanged,
		[this](const KConfigGroup &group, const QByteArrayList &names)
		{
			if (group.name() == "org.kde.kdecoration2") {
				QString library = group.readEntry("library");
				QString theme	= group.readEntry("theme");

				m_themeIsAero = comboIsAero(library, theme);
				emit themeChanged(themeIsAero());
			}
		}
	);

	KConfigGroup deco(config, "org.kde.kdecoration2");

	QString library = deco.readEntry("library");
	QString theme	= deco.readEntry("theme");

	m_themeIsAero = comboIsAero(library, theme);
}

ThemeWatcher *ThemeWatcher::instance()
{
	static ThemeWatcher inst;
	return &inst;
}

bool ThemeWatcher::themeIsAero()
{
	return m_themeIsAero;
}

QStyleOption _amGettingThemed_dummy;

bool amGettingThemed(QWidget *_this)
{
    /* The QStyleSheetStyle that is applied to widgets styled with QSS unfortunately does not publicly expose
     * what style it is overriding, so we have to use this dirty, non-thread-safe hack to find out
     */

    _amGettingThemed_dummy.version = 0;

    {
        QPainter p;
        QWidget w;

        _this->style()->drawPrimitive(QStyle::PE_Widget, &_amGettingThemed_dummy, &p, &w);    // If style() or one of its base styles is the Aero style, it will set _amGettingThemed_dummy->wasStyledByAero to true.
    }

    return _amGettingThemed_dummy.version == -10;
}

} // namespace Private
} // namespace Aero
