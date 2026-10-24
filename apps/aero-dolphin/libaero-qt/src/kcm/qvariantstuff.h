#pragma once

#include <QStyledItemDelegate>
#include <QtWidgets>

#include <AeroQt/util/props.h>
#include <AeroQt/util/image9.h>

class EditorQImage : public QWidget {
	Q_OBJECT

protected:

	QGridLayout *m_layout;
	QPushButton *m_btn;

	MAKE_PROPERTY(QImage, img, Img);
public:
	explicit EditorQImage(QWidget *parent = nullptr);
};

class EditorImage9 : public EditorQImage {
	Q_OBJECT

	MAKE_PROPERTY(Image9, img9, Img9);

public:
	explicit EditorImage9(QWidget *parent = nullptr);
};

class CustomTypeDelegate : public QStyledItemDelegate {
	Q_OBJECT
public:
	using QStyledItemDelegate::QStyledItemDelegate;

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
