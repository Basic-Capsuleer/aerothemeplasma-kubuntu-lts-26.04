#include <QtWidgets>

#include "wizard.h"

#include "insetwindow.h"
#include "navbuttons.h"
#include "util/props.h"
#include "util/scopefn.h"
#include "util/objecteventlistener.h"

using namespace Aero;

Wizard::Wizard(QWidget *parent)
	: QWizard(parent)
{
	Wizard::convert(this);
}

void Wizard::convert(QWizard *that)
{
	if (!that->property("_Aero_originalRoot").isValid()) {	// Don't convert an already converted wizard
		that->setWizardStyle(QWizard::NStyles);

		QWidget *originalRoot = that->findChild<QWidget *>(Qt::FindDirectChildrenOnly);
		that->setProperty("_Aero_originalRoot", QVariant::fromValue(originalRoot));

		auto newRoot = new QWidget;
		originalRoot->setParent(newRoot);

		Aero::makeInsetWindow(
			that,
			newRoot,
			createHeader(that),
			nullptr
		);

		connect(that, &QWizard::currentIdChanged, [=](int id) {
			if (id != -1)
				imposeWidgetStyle(that, newRoot);
		});
	}
}

QWidget *Wizard::createHeader(QWizard *that)
{
	auto *root = new QWidget;
	auto *l = new QHBoxLayout(root);
	l->setSpacing(6);

	{
		auto backButton = Aero::NavButtons::makeBack(root);
		l->addWidget(backButton);

		auto actualBackButton = that->button(QWizard::BackButton);
		connect(backButton, &QAbstractButton::clicked, actualBackButton, &QAbstractButton::click);
		connect(that, &QWizard::currentIdChanged, [=](int id) {
			backButton->setEnabled(actualBackButton->isEnabled());
		});
		backButton->setEnabled(actualBackButton->isEnabled());
	}

	{
		auto icon = new QLabel(root);
		l->addWidget(icon);
		connect(that, &QWidget::windowIconChanged, [icon](const QIcon &i) {
			auto p = i.pixmap(QSize(24, 24));
			icon->setPixmap(p);
			icon->setVisible(!p.isNull());
		});
		that->windowIconChanged(that->windowIcon());
	}

	{
		auto title = new QLabel(root);
		l->addWidget(title);
		bind_prop(
			that, "windowTitle",
			title, "text",
			&QWidget::windowTitleChanged,
			true
		);

		title->setGraphicsEffect(new QGraphicsDropShadowEffect + also{
			it->setOffset(0, 0);
			it->setBlurRadius(8);
			it->setColor(Qt::white);
		});
	}

	l->addStretch();

	return root;
}

void Wizard::imposeWidgetStyle(QWizard *that, QWidget *newRoot)
{
	// Unfortuately, this needs to be called upon every page change, bc QWizard creates all widgets -- even the persistent ones -- from scratch upon every page change for some reason.

	/* In its untouched state, directly after a page change, this is the structure of the wizard:
	 *
	 * QWidget qwizardRoot
	 *	|- QLabel titleLab
	 *	|- QFrame frame
	 *	|	|- QVBoxLayout frameLayout
	 *	|	|- QWizardPage
	 *	|	|- QWizardPage
	 *	|	\- QLabel subtitleLab
	 *	\- QGridLayout
	 *		|- [frame]
	 *		\- QHBoxLayout buttonLayout
	 */

	QWidget *qwizardRoot = that->property("_Aero_originalRoot").value<QWidget *>();
	if (!qwizardRoot) return;

	QFrame	*frame = nullptr;
	for (auto c : qwizardRoot->children()) {
		if (c->metaObject() == &QFrame::staticMetaObject)	// The problem is that a QLabel inherits QFrame and would this also match a findChild call
			frame = qobject_cast<QFrame *>(c);
	}

	if (!frame)	   // Don't ask. It's magic!
		frame = newRoot->findChild<QFrame *>(Qt::FindDirectChildrenOnly);
	else
		frame->setParent(newRoot);

	QVBoxLayout	*frameLayout	= frame->findChild<QVBoxLayout *>(Qt::FindDirectChildrenOnly);
	QGridLayout	*gridLayout	= qwizardRoot->findChild<QGridLayout *>(Qt::FindDirectChildrenOnly);
	QHBoxLayout	*buttonLayout	= gridLayout->findChild<QHBoxLayout	*>(Qt::FindDirectChildrenOnly);
	QLabel	*titleLab	= qwizardRoot->findChild<QLabel *>(Qt::FindDirectChildrenOnly);
	QLabel	*subtitleLab	= frame->findChild<QLabel *>(Qt::FindDirectChildrenOnly);

	// Frame
	{
		frame->show();

		// Add title as child
		auto titleLayout = new QVBoxLayout;
		if (titleLab)
			titleLayout->addWidget(titleLab);
		if (subtitleLab)
			titleLayout->addWidget(subtitleLab);
		titleLayout->setSpacing(4);
		frameLayout->insertLayout(0, titleLayout);

		// Set padding
		frame->layout()->setContentsMargins(QMargins(33, 19, 33, 19));
		frameLayout->setSpacing(19);

		// Set fill
		frame->setAutoFillBackground(true);
		auto pal = frame->palette();
		pal.setColor(QPalette::Window, QColor(255, 255, 255));
		frame->setPalette(pal);
	}

	// Title widget style
	if (titleLab)
	{
		titleLab->setFont(titleLab->font() + also {
			it.setBold(false);
			it.setPixelSize(16);
		});

		titleLab->setPalette(titleLab->palette() + also {
			it.setColor(QPalette::Text, QColor(0, 51, 153));
		});


		titleLab->setIndent(0);
	}

	// Layout margins outside of frame
	{
		gridLayout->setSpacing(0);
		gridLayout->setContentsMargins(QMargins(0, 0, 0, 0));

		buttonLayout->setSpacing(10);
		buttonLayout->setContentsMargins(QMargins(10, 10, 10, 10));

		if (that->currentPage()->children().count() == 0)
			frameLayout->addStretch();
	}

	auto buttonsHeight = buttonLayout->sizeHint().height();

	auto updateGeom = [=](QResizeEvent *ev) {
		frame->setGeometry(newRoot->rect().adjusted(0, 0, -2, -buttonsHeight));
		qwizardRoot->setGeometry(QRect(newRoot->rect().bottomLeft() + QPoint(0, -buttonsHeight), QSize(newRoot->rect().width(), buttonsHeight)));
	};
	onEvent<QResizeEvent>(qwizardRoot, QEvent::Resize, updateGeom);
	updateGeom(nullptr);

	that->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);	  // No idea why it does this but we need to counteract it: https://github.com/qt/qtbase/blob/01caad87ef1f974e395d3691f6736bb81de31835/src/widgets/dialogs/qwizard.cpp#L1308
}
