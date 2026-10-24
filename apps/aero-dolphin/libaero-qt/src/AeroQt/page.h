#ifndef PAGE_H
#define PAGE_H

#include <QFrame>
#include <QVBoxLayout>

namespace Aero {

class Page : public QFrame
{
    Q_OBJECT

    QVBoxLayout *m_layout;

public:
    explicit Page(QString title, QWidget *parent = nullptr);
    explicit Page(QWidget *parent = nullptr);

    QVBoxLayout *layout();      // Makes one if not set

Q_SIGNALS:
};

} // namespace Aero

#endif // PAGE_H
