#ifndef ACTIONPGPH_H
#define ACTIONPGPH_H

#include <QLabel>
#include <KTitleWidget>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>

#include "util/props.h"
#include "util/scopefn.h"
#include "util/flowlayout.h"

namespace Aero {

class ActionPgph : public KTitleWidget
{
    Q_OBJECT

    QWidget *m_widg;

    FlowLayout *m_flow;
    QFormLayout *m_form;    /// If this is set, assume long text mode

    QGridLayout *m_internalGrid;
    QWidget *m_lastSep;

    QMap<QAction *, QHBoxLayout *> m_items;

    MAKE_PROPERTY(QAction *, titleAction, TitleAction);

public:
    explicit ActionPgph(QWidget *parent = nullptr);
    explicit ActionPgph(QString text, QIcon icon = QIcon(), bool longText = false, QWidget *parent = nullptr);

    void setIcon(const QIcon &icon);

    inline bool longText() { return m_form != nullptr; }

protected:
    virtual void actionEvent(QActionEvent *event) override;
};

} // namespace Aero

#endif // ACTIONPGPH_H
