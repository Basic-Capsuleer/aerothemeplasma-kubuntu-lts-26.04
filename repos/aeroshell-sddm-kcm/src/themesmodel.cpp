#include "themesmodel.h"

#include <QDir>
#include <QFileInfo>

#include <KConfig>
#include <KConfigGroup>

ThemesModel::ThemesModel(QObject *parent)
    : QAbstractListModel{parent}
{ populate(); }

ThemesModel::~ThemesModel()
{}


int ThemesModel::rowCount(const QModelIndex &parent) const
{ return m_themes.count(); }

QVariant ThemesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return {};

    const Delegate delegate = m_themes.at(index.row());
    switch((Roles)role) {
        case Path:
            return delegate.path;
        case Display:
            return delegate.display;
    }

    return {};
}

QHash<int, QByteArray> ThemesModel::roleNames() const
{
    return QHash<int, QByteArray>{
        { Path, "path" },
        { Display, "display" }
    };
}


void ThemesModel::populate()
{
    beginResetModel();
    m_themes.clear();
    endResetModel();

    QFileInfoList entries = QDir("/usr/share/sddm/themes/")
                                .entryInfoList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot);
    beginInsertRows({}, 0, 0);
    for(const QFileInfo &entry : std::as_const(entries)) {
        KConfig cfg(entry.absoluteFilePath() + "/theme.conf");
        if(auto display = cfg.group("General").readEntry("aeroShellDisplay", ""); display != "") {
            m_themes.append({entry.absoluteFilePath(), display});
        }
    }
    endInsertRows();
}
