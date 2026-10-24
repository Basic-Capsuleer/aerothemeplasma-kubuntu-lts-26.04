#include "imagebutton.h"

#include <QtWidgets>

namespace Aero {
namespace Private {

	ImageButton::ImageButton(const QString &normal, const QString &hover, const QString &pressed, const QString &disabled, int fadeDuration = 200, QWidget *parent)
		: QPushButton(parent),
		pixmapNormal(normal),
		pixmapHover(hover),
		pixmapPressed(pressed),
		pixmapDisabled(disabled)
	{
		setFlat(true);
		setMouseTracking(true);
		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		setFixedSize(pixmapNormal.size());

		current = statePixmap();

        anim = new QPropertyAnimation(this, "fade", this);
		anim->setDuration(fadeDuration);
		anim->setStartValue(0.0);
		anim->setEndValue(1.0);
	}

	void ImageButton::paintEvent(QPaintEvent *)
	{
		QPainter p(this);

		if (previous && fadeValue < 1.0) {
			p.setOpacity(1.0 - fadeValue);
			p.drawPixmap(0, 0, *previous);
		}

		p.setOpacity(fadeValue);
		p.drawPixmap(0, 0, *current);
	}

	void ImageButton::enterEvent(QEnterEvent *)
	{
		updateState();
	}

	void ImageButton::leaveEvent(QEvent *)
	{
		updateState();
	}

	void ImageButton::mousePressEvent(QMouseEvent *e)
	{
		QPushButton::mousePressEvent(e);
		updateState();
	}

	void ImageButton::mouseReleaseEvent(QMouseEvent *e)
	{
		QPushButton::mouseReleaseEvent(e);
		updateState();
	}

	void ImageButton::changeEvent(QEvent *e)
	{
		if (e->type() == QEvent::EnabledChange)
			updateState();

		QPushButton::changeEvent(e);
	}

	void ImageButton::setScale(qreal scale)
	{
		for (QPixmap *pixmap : {&pixmapNormal, &pixmapHover, &pixmapPressed, &pixmapDisabled})
			*pixmap = pixmap->scaled(pixmap->size() * scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		setFixedSize(pixmapNormal.size());
		current = statePixmap();
		previous = nullptr;
		update();
	}

	const QPixmap *ImageButton::statePixmap() const
	{
		if (!isEnabled())
			return &pixmapDisabled;
		if (isDown())
			return &pixmapPressed;
		if (underMouse())
			return &pixmapHover;
		return &pixmapNormal;
	}

	void ImageButton::updateState()
	{
		const QPixmap* target = statePixmap();
		if (target == current)
			return;

		previous = current;
		current = target;

		anim->stop();
		anim->setStartValue(0.0);
		anim->setEndValue(1.0);
		anim->start();
	}

} // namespace Private
} // namespace Aero
