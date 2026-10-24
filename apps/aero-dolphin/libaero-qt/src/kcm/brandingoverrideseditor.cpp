#include "brandingoverrideseditor.h"
#include "ui_brandingoverrideseditor.h"

#include "qvariantstuff.h"

#include <QtWidgets>
#include <AeroQt/util/objecteventlistener.h>
#include <AeroQt/util/scopefn.h>

QString overridesFile =
	QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
	"/aerothemeoverrides.dat";

BrandingOverridesEditor::BrandingOverridesEditor(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::BrandingOverridesEditor)
{
	qRegisterMetaType<Image9>();

	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	ui->tb->setItemDelegate(new CustomTypeDelegate(this));
	ui->tb->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tb->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	/* Adding */
	auto addRow = [=](int typeId) {
		int row = ui->tb->rowCount();
		ui->tb->insertRow(row);

		auto *item = new QTableWidgetItem();
		item->setData(Qt::EditRole, QString());
		ui->tb->setItem(row, 0, item);

		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, QVariant(QMetaType(typeId)));
		ui->tb->setItem(row, 1, item);

		ui->tb->openPersistentEditor(item);
	};

	ui->btAdd->setMenu(new QMenu(this) + also {
		connect(it->addAction("String"),  &QAction::triggered, [=]{ addRow(QMetaType::QString); });
		connect(it->addAction("Bool"),	  &QAction::triggered, [=]{ addRow(QMetaType::Bool); });
		connect(it->addAction("Integer"), &QAction::triggered, [=]{ addRow(QMetaType::Int); });
		connect(it->addAction("Image"),	  &QAction::triggered, [=]{ addRow(QMetaType::QImage); });
		connect(it->addAction("Image (9-slice)"), &QAction::triggered, [=]{ addRow(qMetaTypeId<Image9>()); });
	});

	/* Removing */
	connect(ui->btRem, &QAbstractButton::clicked, [=]() {
		if (auto idx = ui->tb->currentIndex(); idx.isValid()) {
			ui->tb->removeRow(idx.row());
		}
	});

	connect(ui->btImport, &QAbstractButton::clicked, [=]() {
		load(QFileDialog::getOpenFileName(
			this,
			"Export overrides",
			QString(),
			"Overrides lists (*.dat)"
		));
	});

	connect(ui->btExport, &QAbstractButton::clicked, [=]() {
		save(QFileDialog::getSaveFileName(
			this,
			"Export overrides",
			QString(),
			"Overrides lists (*.dat)"
		));
	});

	/* Load/Save */
	if (QFile::exists(overridesFile))
		load(overridesFile);

	connect(this, &QObject::destroyed, [=]() {
		save(overridesFile);
	});
}

BrandingOverridesEditor::~BrandingOverridesEditor()
{
	delete ui;
}

void BrandingOverridesEditor::load(QString path)
{
	// for (int y = 0; y < ui->tb->rowCount(); y++)
	//	   ui->tb->removeRow(0);

	QFile file(path);
	file.open(QIODevice::ReadOnly);

	QDataStream in(&file);

	while (!in.atEnd()) {
		QString name;
		QVariant val;

		in >> name >> val;

		/* Add row */
		int row = ui->tb->rowCount();
		ui->tb->insertRow(row);

		auto *item = new QTableWidgetItem();
		item->setData(Qt::EditRole, name);
		ui->tb->setItem(row, 0, item);

		item = new QTableWidgetItem();
		item->setData(Qt::EditRole, val);
		ui->tb->setItem(row, 1, item);

		ui->tb->openPersistentEditor(item);
	}
}

void BrandingOverridesEditor::save(QString path)
{
	QFile file(path);
	file.open(QIODevice::WriteOnly);

	QDataStream out(&file);

	for (int y = 0; y < ui->tb->rowCount(); y++) {
		QString name = ui->tb->item(y, 0)->data(Qt::EditRole).toString();
		QVariant val = ui->tb->item(y, 1)->data(Qt::EditRole);

		if (!name.isEmpty())
			out << name << val;
	}
}
