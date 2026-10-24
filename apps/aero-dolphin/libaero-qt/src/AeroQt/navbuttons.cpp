#include "navbuttons.h"

#include <QtWidgets>

#include "util/objecteventlistener.h"
#include "private/aerothemesettings.h"

using namespace Aero;
using namespace Aero::Private;

static constexpr qreal scale = 1.25;

NavButtons::NavButtons(QWidget *parent) :
	m_background(":/AeroQt/navButtons/background.png"),
	m_menu(nullptr)
{
	m_back	= makeBack(this);
	m_forward	= makeForward(this);
	m_menuButton	= new ImageButton(
		":/AeroQt/navButtons/normal.png",
		":/AeroQt/navButtons/hover.png",
		":/AeroQt/navButtons/pressed.png",
		":/AeroQt/navButtons/disabled.png",
		0,
		this
	);
	m_menuButton->setVisible(false);

	// Draw the buttons at 125% to go with a bigger system font (there are only 100% images, so they
	// are smoothly scaled)
	m_background = m_background.scaled(m_background.size() * scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	for (QPushButton *button : {m_back, m_forward, m_menuButton})
		static_cast<ImageButton *>(button)->setScale(scale);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setFixedSize(minimumSizeHint());

	m_back->move(qRound(-1 * scale), qRound(1 * scale));
	m_forward->move(qRound(28 * scale), qRound(1 * scale));
	m_menuButton->move(qRound(51 * scale), qRound(3 * scale));

	//

	connect(this, &NavButtons::menuChanged, [=](QMenu *menu){
		m_menuButton->setMenu(m_menu);
		m_menuButton->setVisible(m_menu != nullptr);
		setFixedSize(minimumSizeHint());   // Refresh size. NavButtons doesn't request area for the hidden menu button

		if (m_menu) {
			onEvent(m_menu, QEvent::EnabledChange, [&](QEvent *) {
				m_menuButton->setEnabled(m_menu->isEnabled());
			});		// ObjectEventListener deregisters this listener automatically when the object gets deleted
		}
	});
}

QPushButton *NavButtons::makeForward(QWidget *parent)
{
	return new ImageButton(
		":/AeroQt/navButtons/forward/normal.png",
		":/AeroQt/navButtons/forward/hover.png",
		":/AeroQt/navButtons/forward/pressed.png",
		":/AeroQt/navButtons/forward/disabled.png",
		AeroThemeSettings::self()->noFadeNavButtons() ? 0 : 200,
		parent
	);
}

QPushButton *NavButtons::makeBack(QWidget *parent)
{
	return new ImageButton(
		":/AeroQt/navButtons/back/normal.png",
		":/AeroQt/navButtons/back/hover.png",
		":/AeroQt/navButtons/back/pressed.png",
		":/AeroQt/navButtons/back/disabled.png",
		AeroThemeSettings::self()->noFadeNavButtons() ? 0 : 200,
		parent
	);
}

QSize NavButtons::minimumSizeHint() const
{
	return m_background.size() + (m_menu ? QSize() : QSize(-qRound(13 * scale), 0));
}

void NavButtons::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	p.drawPixmap(0, 0, m_background);
}

// This is to fix sth on atmk's system, it will disappear eventually, please ignore
extern "C" {
	void _ZN4Aero10NavButtons11menuChangedERP5QMenu(NavButtons *that, QMenu **men) {
		that->menuChanged(*men);
	}
}
