#include "qvariantstuff.h"

#include <AeroQt/util/scopefn.h>

#define PREVIEW_SIZE QSize(64, 48)

EditorQImage::EditorQImage(QWidget *parent) :
	QWidget(parent)
{
	setAutoFillBackground(true);
    // setPalette(
    // 	palette() + also {
    // 		it.setColor(QPalette::Window, qApp->palette().color(QPalette::Base));
    // 	}
    // );

    auto p = palette();
    APPLY(p, {
        setColor(QPalette::Window, qApp->palette().color(QPalette::Base));
    });
    setPalette(p);

	setLayout(m_layout = new QGridLayout(this) + also {
		it->setSpacing(4);
		it->setContentsMargins(QMargins());
	});

	auto *lab = new QLabel;
	lab->setAutoFillBackground(true);
	m_layout->addWidget(lab, 1, 1);

	connect(this, &EditorQImage::imgChanged, [=]() {
		lab->setPixmap(QPixmap::fromImage(img().scaled(PREVIEW_SIZE, Qt::KeepAspectRatio)));
		lab->update();
	});

	m_btn = new QPushButton("Browse...");
	m_layout->addWidget(m_btn, 2, 1);

	connect(m_btn, &QAbstractButton::clicked, [=]() {
		this->setImg(QImage(QFileDialog::getOpenFileName(
			this,
			"Open Image",
			QString(),
			"Image files (*.png)"
		)));
	});
}


EditorImage9::EditorImage9(QWidget *parent) :
	EditorQImage(parent)
{
	/* Extra widgets */

#define BOX(Y, X, side, Side)				\
	{										\
		m_layout->addWidget(				\
			new QLabel(#Side ":"),			\
			Y, X							\
		);									\
		auto *sp = new QSpinBox;			\
		m_layout->addWidget(sp, Y, X + 1);	\
		bind_prop_bidi(										\
			this, "img9", &EditorImage9::img9Changed,		\
			sp, "value", &QSpinBox::valueChanged,			\
			true,											\
			[=](QVariant src) -> QVariant { return src.value<Image9>().margins.side(); },												\
			[=](QVariant dst) -> QVariant { auto m = m_img9; m.margins.set##Side(dst.value<int>()); return QVariant::fromValue(m); }   \
		);									\
	}

	BOX(1, 2, top, Top);
	BOX(2, 2, left, Left);
	BOX(1, 4, right, Right);
	BOX(2, 4, bottom, Bottom);

#undef BOX

#define BOX(Y, X, DIM, lab)					\
	{										\
		m_layout->addWidget(				\
			new QLabel(lab),				\
			Y, X							\
		);									\
		auto *sp = new QComboBox;			\
		sp->addItem("Stretch");				\
		sp->addItem("Tile");				\
		m_layout->addWidget(sp, Y, X + 1);	\
		bind_prop_bidi(						\
			this, "img9", &EditorImage9::img9Changed,				\
			sp, "currentIndex", &QComboBox::currentIndexChanged,	\
			true,							\
			[=](QVariant src) -> QVariant { return src.value<Image9>().tile##DIM; },											\
			[=](QVariant dst) -> QVariant { auto m = m_img9; m.tile##DIM = dst.value<int>(); return QVariant::fromValue(m); }	\
		);									\
	}

	BOX(1, 6, H, "Horiz.:")
	BOX(2, 6, V, "Vert.:")

#undef BOX

	/* Browse button */

	disconnect(m_btn, &QAbstractButton::clicked, nullptr, nullptr);
	connect(m_btn, &QAbstractButton::clicked, [=]() {
		QString path = QFileDialog::getOpenFileName(
			this,
			"Open Image",
			QString(),
			"Image files (*.png);;Android 9-patch PNGs (*.9.png)"
		);

		auto m = img9();

		if (path.endsWith(".9.png"))
			m.loadAndroidFormat(QImage(path));
		else
			m.image = QImage(path);

		setImg9(m);
	});

	/* Preview */

	bind_prop(
		this, "img9", this, "img", &EditorImage9::img9Changed, true,
		[=](QVariant val) -> QVariant {
			QPixmap pix(PREVIEW_SIZE);

			if (auto img9 = val.value<Image9>(); !img9.image.isNull()) {
				QPainter p(&pix);
				img9.paint(&p, pix.rect());
			}

			return pix;
		}
	);
}

QWidget *CustomTypeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QVariant data = index.data(Qt::EditRole);

	if (data.userType() == QMetaType::QImage) {
		auto *ed = new EditorQImage(parent);

		connect(ed, &EditorQImage::imgChanged, this, [=]() {
			if (ed->isVisible())
				emit const_cast<CustomTypeDelegate*>(this)->commitData(ed);
		});

		return ed;
	}
	if (data.userType() == qMetaTypeId<Image9>()) {
		auto *ed = new EditorImage9(parent);

		connect(ed, &EditorImage9::img9Changed, this, [=]() {
			if (ed->isVisible())
				emit const_cast<CustomTypeDelegate*>(this)->commitData(ed);
		});

		return ed;
	}

	return QStyledItemDelegate::createEditor(parent, option, index);
}

void CustomTypeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	QVariant data = index.data(Qt::EditRole);

	if (data.userType() == QMetaType::QImage) {
		auto *ed = qobject_cast<EditorQImage*>(editor);
		ed->setImg(data.value<QImage>());
	} else if (data.userType() == qMetaTypeId<Image9>()) {
		auto *ed = qobject_cast<EditorImage9*>(editor);
		ed->setImg9(data.value<Image9>());
	} else
		QStyledItemDelegate::setEditorData(editor, index);
}

void CustomTypeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	QVariant data = index.data(Qt::EditRole);

	if (data.userType() == QMetaType::QImage) {
		auto *ed = qobject_cast<EditorQImage*>(editor);
		model->setData(index, ed->img(), Qt::EditRole);
	} else if (data.userType() == qMetaTypeId<Image9>()) {
		auto *ed = qobject_cast<EditorImage9*>(editor);
		model->setData(index, QVariant::fromValue(ed->img9()), Qt::EditRole);
	} else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

QSize CustomTypeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
	if (index.data(Qt::EditRole).userType() == QMetaType::QImage) {
		return QSize(64, 64);
	}
	// if (index.data(Qt::EditRole).userType() == qMetaTypeId<Image9>()) {
	//	   return QSize(64, 64);
	// }

	return QStyledItemDelegate::sizeHint(option, index);
}

void CustomTypeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	editor->setGeometry(option.rect);
}
