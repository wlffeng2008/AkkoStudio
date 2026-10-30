#ifndef MODULEEFSPEED_H
#define MODULEEFSPEED_H

#include <QFrame>

namespace Ui {
class ModuleEfSpeed;
}

class ModuleEfSpeed : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleEfSpeed(QWidget *parent = nullptr);
    ~ModuleEfSpeed();
    void setSpeed(quint8 speed);

protected:
    void changeEvent(QEvent *pEvt);

signals:
    void onSpeedChanged(quint8 speed);

private:
    Ui::ModuleEfSpeed *ui;
    bool m_bOutSet=false;
};

#endif // MODULEEFSPEED_H
