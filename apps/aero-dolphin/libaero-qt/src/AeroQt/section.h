#ifndef SECTION_H
#define SECTION_H

#include <QGroupBox>

#include "util/props.h"

namespace Aero {

class Section : public QGroupBox
{
    Q_OBJECT

    QWidget *m_header;

public:

    explicit Section(QWidget *parent = nullptr);

public Q_SLOTS:
    void toggleCollapsed();

protected:
    // virtual QSize sizeHint() const;
    void paintEvent(QPaintEvent *event) override;

    QWidget *makeHeader();

private:
    void onCollapse();
    void onUncollapse();
};

class SubSection : public Section
{
    Q_OBJECT
public:
    explicit SubSection(QWidget *parent = nullptr);
// protected:
//     virtual QWidget *makeHeader() override;
};

} // namespace Aero

#endif // SECTION_H
