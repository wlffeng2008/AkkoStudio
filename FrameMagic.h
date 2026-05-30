#ifndef FRAMEMAGIC_H
#define FRAMEMAGIC_H

#include <QFrame>

namespace Ui {
class FrameMagic;
}

class FrameMagic : public QFrame
{
    Q_OBJECT

public:
    explicit FrameMagic(QWidget *parent = nullptr);
    ~FrameMagic();

protected:
    bool eventFilter(QObject*watched, QEvent*event) override;

private:
    Ui::FrameMagic *ui;

    QColor m_selColor;
};

#endif // FRAMEMAGIC_H
