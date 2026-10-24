#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QString>

#include <KAuth/ActionReply>
#include <KAuth/HelperSupport>

using namespace KAuth;

class KCMHelper : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    ActionReply write(const QVariantMap &args);

private:
    QString m_path;
    QString m_defaultConfig;
    QString m_userConfig;
    QString m_background;
    QString m_defaultBackground;

    void updateVars();
};

#endif
