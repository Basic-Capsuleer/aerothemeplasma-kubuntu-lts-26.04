#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGraphicsOpacityEffect>

namespace Aero {
    class Sidebar: public QScrollArea {
        Q_OBJECT

        QWidget     *textWrap, *m_seeAlsoLabel;
        QVBoxLayout *m_navV;
        QFormLayout *m_itemsL, *m_seeAlsoL;
        int m_initialWidth;

        QGraphicsOpacityEffect *m_sidebarTextEffect;
        bool m_fadeInText, m_fadeOutText, m_showIcons;

        QAction *m_goHome;

    public:
        void setFadeInText(bool b);
        void setFadeOutText(bool b);

    public:


        Sidebar(QWidget *parent = nullptr);

        Sidebar(
            QAction *goHome = nullptr,
            int initialWidth = 0,       // FIXME: Get rid of this
            QWidget *parent = nullptr,
            bool showIcons = false
        );

        void addAction(QAction *act); // When an destination is clicked, it emits ::triggered()
        void addSeeAlso(QAction *act);
        void setHomeAction(QAction *act);

        void clear();

        virtual QSize sizeHint() const override;

    protected:
        virtual void paintEvent(QPaintEvent *) override;

    private:
        void widgetForAction(QAction *act, QWidget *&widget, QWidget *&indicator);
    };
}

#endif // SIDEBAR_H
