#include "image9.h"

#include <QImage>
#include <QPixmap>
#include <QMargins>
#include <QVarLengthArray>

QDataStream& operator<<(QDataStream &stream, const Image9 &img9)
{
	return stream << img9.image << img9.margins << img9.tileH << img9.tileV;
}

QDataStream& operator>>(QDataStream &stream, Image9 &img9)
{
	return stream >> img9.image >> img9.margins >> img9.tileH >> img9.tileV;
}

void Image9::paint(QPainter *p, QRect rect) const
{
	if (image.isNull() || rect.isEmpty())
		return;

	const int iw = image.width();
	const int ih = image.height();

	// Source margins, clamped so opposing margins never exceed the image
	// size. When left+right >= iw (or top+bottom >= ih) the center
	// collapses to zero width/height and the affected corner absorbs the
	// remaining image content, which reproduces the "whole image in one
	// corner" edge case without special-casing it.
	const int srcL = qBound(0, margins.left(), iw);
	const int srcR = qBound(0, margins.right(), iw - srcL);
	const int srcT = qBound(0, margins.top(), ih);
	const int srcB = qBound(0, margins.bottom(), ih - srcT);
	const int srcCW = qMax(0, iw - srcL - srcR);
	const int srcCH = qMax(0, ih - srcT - srcB);

	const int sx1 = srcL; // center column start (native, unaffected by squeeze)
	const int sy1 = srcT; // center row start (native, unaffected by squeeze)

	// Destination corner sizes. Corners keep their native size unless the
	// target rect is smaller than the sum of opposing corners, in which
	// case they are CLIPPED (never scaled) to fit: left/top are given
	// priority, right/bottom take whatever remains.
	const int rw = rect.width();
	const int rh = rect.height();

	const int dstL = qMin(srcL, rw);
	const int dstR = qMin(srcR, rw - dstL);
	const int dstT = qMin(srcT, rh);
	const int dstB = qMin(srcB, rh - dstT);

	const int dstCW = qMax(0, rw - dstL - dstR);
	const int dstCH = qMax(0, rh - dstT - dstB);

	const int dx0 = rect.left();
	const int dx1 = dx0 + dstL;
	const int dx2 = dx1 + dstCW;
	const int dy0 = rect.top();
	const int dy1 = dy0 + dstT;
	const int dy2 = dy1 + dstCH;

	// Source coordinates for the margin columns/rows. Each is sized to
	// match its (possibly clipped) destination exactly and anchored to
	// the image's outer edge, so a squeeze crops the inner side of the
	// patch instead of scaling the whole thing down.
	const int scL = 0;			  // left column always anchored at x=0
	const int scR = iw - dstR;	  // right column anchored at the image's right edge
	const int scT = 0;			  // top row always anchored at y=0
	const int scB = ih - dstB;	  // bottom row anchored at the image's bottom edge

	struct Cell {
		QRect src;
		QRect dst;
		bool  tileX;
		bool  tileY;
	};

	// Local copies avoid shadowing the class-level tileH/tileV fields
	// while keeping the cell table below readable.
	const bool wantTileH = tileH;
	const bool wantTileV = tileV;

	const Cell cells[9] = {
						   { QRect(scL, scT, dstL,	dstT),	QRect(dx0, dy0, dstL,  dstT),  false,	  false		},
						   { QRect(sx1, scT, srcCW, dstT),	QRect(dx1, dy0, dstCW, dstT),  wantTileH, false		},
						   { QRect(scR, scT, dstR,	dstT),	QRect(dx2, dy0, dstR,  dstT),  false,	  false		},
						   { QRect(scL, sy1, dstL,	srcCH), QRect(dx0, dy1, dstL,  dstCH), false,	  wantTileV },
						   { QRect(sx1, sy1, srcCW, srcCH), QRect(dx1, dy1, dstCW, dstCH), wantTileH, wantTileV },
						   { QRect(scR, sy1, dstR,	srcCH), QRect(dx2, dy1, dstR,  dstCH), false,	  wantTileV },
						   { QRect(scL, scB, dstL,	dstB),	QRect(dx0, dy2, dstL,  dstB),  false,	  false		},
						   { QRect(sx1, scB, srcCW, dstB),	QRect(dx1, dy2, dstCW, dstB),  wantTileH, false		},
						   { QRect(scR, scB, dstR,	dstB),	QRect(dx2, dy2, dstR,  dstB),  false,	  false		},
						   };

	auto validCell = [](const Cell &c) {
		return c.src.width() > 0 && c.src.height() > 0
			   && c.dst.width() > 0 && c.dst.height() > 0;
	};

	if (!wantTileH && !wantTileV) {
		// Fast path: no tiling anywhere, so all patches can be batched
		// into a single hardware-accelerated call.
		const QPixmap pm = QPixmap::fromImage(image);
		QVarLengthArray<QPainter::PixmapFragment, 9> fragments;

		for (const Cell &c : cells) {
			if (!validCell(c))
				continue;

			QPainter::PixmapFragment f;
			f.x = c.dst.left() + c.dst.width() / 2.0;
			f.y = c.dst.top() + c.dst.height() / 2.0;
			f.sourceLeft = c.src.left();
			f.sourceTop = c.src.top();
			f.width = c.src.width();
			f.height = c.src.height();
			f.scaleX = qreal(c.dst.width()) / qreal(c.src.width());
			f.scaleY = qreal(c.dst.height()) / qreal(c.src.height());
			f.rotation = 0;
			f.opacity = 1;
			fragments.append(f);
		}

		if (!fragments.isEmpty())
			p->drawPixmapFragments(fragments.constData(), fragments.size(), pm);

		return;
	}

	// Fallback: at least one axis requires tiling, so patches are drawn
	// with explicit looping instead of the batched fragment call.
	for (const Cell &c : cells) {
		if (!validCell(c))
			continue;

		if (!c.tileX && !c.tileY) {
			p->drawImage(c.dst, image, c.src);
			continue;
		}

		// The tiling axis repeats at native source size; the other axis
		// (if not tiling) is scaled to the destination size first.
		const int stepW = c.tileX ? c.src.width()  : c.dst.width();
		const int stepH = c.tileY ? c.src.height() : c.dst.height();

		if (stepW <= 0 || stepH <= 0)
			continue;

		for (int y = c.dst.top(); y <= c.dst.bottom(); y += stepH) {
			const int drawH = qMin(stepH, c.dst.bottom() - y + 1);
			const int srcH	= c.tileY ? drawH : c.src.height();

			for (int x = c.dst.left(); x <= c.dst.right(); x += stepW) {
				const int drawW = qMin(stepW, c.dst.right() - x + 1);
				const int srcW	= c.tileX ? drawW : c.src.width();

				const QRect srcPiece(c.src.left(), c.src.top(), srcW, srcH);

				if (c.tileX && c.tileY) {
					// Both axes tile: draw a plain, untransformed blit so
					// edge tiles are always clipped, never resampled.
					p->drawImage(QPoint(x, y), image, srcPiece);
				} else if (c.tileX) {
					// Horizontal tiling only: the tile width itself is
					// never scaled (drawW == srcW), but the vertical
					// (stretch) axis still needs an explicit target
					// height, so use the rect form for that axis alone.
					const QRect dstPiece(x, y, drawW, drawH);
					p->drawImage(dstPiece, image, srcPiece);
				} else if (c.tileY) {
					// Vertical tiling only: mirror image of the case
					// above, height stays 1:1 while width stretches.
					const QRect dstPiece(x, y, drawW, drawH);
					p->drawImage(dstPiece, image, srcPiece);
				}
			}
		}
	}
}

void Image9::loadAndroidFormat(QImage src)
{
	this->image = src.copy(src.rect().marginsRemoved(QMargins(1, 1, 1, 1)));

	int x = 0;

	while(x < src.width() && src.pixelColor(x, 0).alpha() == 0)
		x++;

    this->margins.setLeft(x - 1);

	while(x < src.width() && src.pixelColor(x, 0).alpha() != 0)
		x++;

    this->margins.setRight(src.width() - x - 1);

	// Y
	int y = 0;

	while(y < src.height() && src.pixelColor(0, y).alpha() == 0)
		y++;

    this->margins.setTop(y - 1);

	while(y < src.height() && src.pixelColor(0, y).alpha() != 0)
		y++;

    this->margins.setBottom(src.height() - y - 1);
}

QDebug operator<<(QDebug dbg, const Image9 &img9)
{
    return dbg << "Image9(" << (img9.image.isNull() ? "null" : "QImage(...)") << img9.margins << img9.tileH << img9.tileV << ")\n";
}
