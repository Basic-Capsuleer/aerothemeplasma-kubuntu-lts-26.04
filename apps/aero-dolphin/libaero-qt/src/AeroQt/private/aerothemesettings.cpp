#include "aerothemesettings.h"

#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QJsonObject>
#include <QVariant>
#include <QString>

QJsonObject dictFromObj(const QObject *obj) {
	QJsonObject jsonObj;
	if (!obj) {
		return jsonObj;
	}

	// 1. Get the meta-object metadata block
	const QMetaObject *metaObj = obj->metaObject();
	int count = metaObj->propertyCount();

	// 2. Loop through every available Q_PROPERTY
	for (int i = 0; i < count; ++i) {
		QMetaProperty prop = metaObj->property(i);
		QString propName = QString::fromUtf8(prop.name());

		// 3. Exclude objectName
		if (propName == QLatin1String("objectName")) {
			continue;
		}

		// 4. Read the runtime variant value
		QVariant value = prop.read(obj);
		auto typeId = prop.metaType().id();

		// 5. Explicitly filter and insert allowed primitives
		if (typeId == QMetaType::Int) {
			jsonObj.insert(propName, value.toInt());
		} else if (typeId == QMetaType::Bool) {
			jsonObj.insert(propName, value.toBool());
		} else if (typeId == QMetaType::QString) {
			jsonObj.insert(propName, value.toString());
		}
	}

	return jsonObj;
}

void objReadDict(QObject *obj, const QJsonObject &jsonObj) {
	if (!obj || jsonObj.isEmpty()) {
		return;
	}

	// 1. Get the meta-object schema
	const QMetaObject *metaObj = obj->metaObject();
	int count = metaObj->propertyCount();

	// 2. Loop through the QObject's defined properties
	for (int i = 0; i < count; ++i) {
		QMetaProperty prop = metaObj->property(i);
		QString propName = QString::fromUtf8(prop.name());

		// Skip non-writable properties or objectName
		if (!prop.isWritable() || propName == QLatin1String("objectName")) {
			continue;
		}

		// 3. Check if the incoming JSON contains this specific property key
		if (!jsonObj.contains(propName)) {
			continue;
		}

		QJsonValue jsonVal = jsonObj.value(propName);
		auto typeId = prop.metaType().id();

		// 4. Validate types and safely update the QObject property
		if (typeId == QMetaType::Int && jsonVal.isDouble()) {
			// JSON numbers are parsed as doubles natively, cast to int
			prop.write(obj, jsonVal.toInt());
		} else if (typeId == QMetaType::Bool && jsonVal.isBool()) {
			prop.write(obj, jsonVal.toBool());
		} else if (typeId == QMetaType::QString && jsonVal.isString()) {
			prop.write(obj, jsonVal.toString());
		}
	}
}


ToolbarRule::ToolbarRule(QObject *parent)
	: QObject(parent)
{ }

void AeroThemeSettings::setToolbarOverrides(QList<ToolbarRule *> rules)
{
	QJsonArray arr;
	for (auto *rule: rules)
		arr.append(dictFromObj(rule));

	QJsonDocument doc(arr);
	auto json = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

	AeroThemeSettingsGen::setToolbarOverrides(json);
}

QList<ToolbarRule *> AeroThemeSettings::toolbarOverrides()
{
	QString json = AeroThemeSettingsGen::toolbarOverrides();

	QJsonParseError parseError;
	QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &parseError);

	if (parseError.error != QJsonParseError::NoError) {
		qWarning() << "JSON Parsing Error:" << parseError.errorString() << "at offset" << parseError.offset;
		return QList<ToolbarRule *>(); // Return an empty fallback array
	}

	QList<ToolbarRule *> list;
	for (const QJsonValue &value : doc.array()) {
		auto *rule = new ToolbarRule(nullptr);
		objReadDict(rule, value.toObject());
		list.append(rule);
	}

	return list;
}
