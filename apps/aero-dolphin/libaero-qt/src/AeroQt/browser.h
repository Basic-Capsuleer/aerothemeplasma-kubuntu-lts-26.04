#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QFrame>
#include <QMainWindow>

#include <AeroQt/navbuttons.h>
#include <AeroQt/util/props.h>

#include <functional>

class CrumbBar;
class BrowserHistory;

namespace Aero {

/** An object providing an address bar with history and nav buttons for a list of pages.
 *  To use, add the widgets `addressBar()`, `page()` and `navButtons()` somehwere into your UI.
 */

class Browser : public QObject
{
    Q_OBJECT

    friend class ::CrumbBar;

public:
    using MakeFn = std::function<QWidget *(QMap<QString, QString>)>;

private:
    BrowserHistory *m_hist;

    CrumbBar *m_crumbs;
    NavButtons *m_navBut;
    QFrame *m_frame;

    QWidget *m_page;    // Just for convenience

    struct PgInfo {
        QAction *act;
        MakeFn make;
    };

    QMap<QString, PgInfo> m_pages;

public:
    Q_PROPERTY(QString path READ path WRITE navigateTo NOTIFY pageChanged)
    Q_PROPERTY(QWidget *page READ page NOTIFY pageChanged)

    explicit Browser(QString initialPath, QWidget *parent = nullptr);

    // You get to place these when initializing your subclass. They are created without a parent.
    QWidget *addressBar();
    inline NavButtons *navButtons() { return m_navBut; }
    inline QFrame *pageFrame() { return m_frame; }

    QString path();
    QWidget *page();

    bool navigateTo(QString path);
    void reload();
    QAction *addPage(QString path, MakeFn makePage /* can be nullptr */);
    QAction *actionForPath(QString path);

    inline QStringList allPaths() { return m_pages.keys(); }    /// Note that paths are stored without trailing separator, so "/" is stored under ""
    QList<QAction *> children(QString path);
    QList<QAction *> children(QAction *act);

Q_SIGNALS:
    void pageChanged();
    void pageNotFound(QString, QMap<QString, QString>);

private:
    bool displayPage(QString _path);
};

} // namespace Aero

#endif // BROWSERWINDOW_H
