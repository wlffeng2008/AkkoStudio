#ifndef FRAMEMOUSE_H
#define FRAMEMOUSE_H

#include <QFrame>

namespace Ui {
class FrameMouse;
}

class FrameMouse : public QFrame
{
    Q_OBJECT

public:
    explicit FrameMouse(QWidget *parent = nullptr);
    ~FrameMouse();

    void updateName(const QString&name);

protected:
    bool eventFilter(QObject *watch, QEvent *event) override;

signals:
    void onReturn();

private:
    Ui::FrameMouse *ui;
};

#endif // FRAMEMOUSE_H
