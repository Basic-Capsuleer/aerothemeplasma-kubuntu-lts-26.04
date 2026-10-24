#pragma once

#include <QImage>
#include <QDataStream>
#include <QMargins>
#include <QPainter>
#include <QRect>

#include <AeroQt/util/props.h>

/// Android-style 9-slice image

struct Image9
{
	QImage image;
	QMargins margins;
	bool tileH;		/// Else stretch
	bool tileV;

	Image9()	= default;
	~Image9()	= default;
	Image9(const Image9 &) = default;

	void paint(QPainter *p, QRect rect) const;
	void loadAndroidFormat(QImage src);
};

QDataStream &operator<<(QDataStream &stream, const Image9 &img9);
QDataStream &operator>>(QDataStream &stream, Image9 &img9);

QDebug operator<<(QDebug dbg, const Image9 &img9);

inline bool operator==(const Image9 &, const Image9 &) {
	return false;
}
inline bool operator!=(const Image9 &, const Image9 &) {
	return true;
}

Q_DECLARE_METATYPE(Image9);
