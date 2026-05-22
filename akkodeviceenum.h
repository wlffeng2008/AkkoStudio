#ifndef AKKODEVICEENUM_H
#define AKKODEVICEENUM_H

#include <QObject>
#include <QThread>
class MainWindow;
class AkkoDeviceEnum : public QThread
{
    Q_OBJECT
public:
    explicit AkkoDeviceEnum(QObject *parent = nullptr);
    MainWindow *m_Root=nullptr;
    void Exit();
    void DoEnum();

protected:
    void run() override;
    bool m_bEnding=false;
    bool m_enum=false;
};

#endif // AKKODEVICEENUM_H
