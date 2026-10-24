#ifndef AEROSHELLSDDMTHEMEMODELCOOLYEAH_H
#define AEROSHELLSDDMTHEMEMODELCOOLYEAH_H

#include <QObject>
#include <QQuickItem>
#include <QAbstractListModel>

struct Delegate {
    QString path;
    QString display;
};

class ThemesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum Roles { Path, Display };
    Q_ENUM(Roles)

    explicit ThemesModel(QObject *parent = nullptr);
    ~ThemesModel() override;

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void populate();

private:
    QList<Delegate> m_themes{};
};

#endif // AEROSHELLSDDMTHEMEMODELCOOLYEAH_H
