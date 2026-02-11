#ifndef AKKODEVICEBASE_H
#define AKKODEVICEBASE_H

#include <QObject>
#include <QWidget>


typedef struct {
    quint32 id;
    quint16 VID;
    quint16 PID;
    quint8 type; // 0 keyboard 1 mouse 2 headphone
    QString name;

} AkkoDeviceInfo;

AkkoDeviceInfo *getDevice(quint32 id);

class AkkoDeviceBase : public QWidget
{
    Q_OBJECT
public:
    explicit AkkoDeviceBase(QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *event) override;

signals:
};

#endif // AKKODEVICEBASE_H
