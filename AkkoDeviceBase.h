#ifndef AKKODEVICEBASE_H
#define AKKODEVICEBASE_H

#include <QObject>
#include <QWidget>


typedef struct {
    quint32 driverId;
    quint16 VID;
    quint16 PID;
    quint8  device;
    quint8  type; // 0 keyboard 1 mouse 2 headphone
    quint8  connect;
    QString name;
} AkkoDeviceInfo;

QString getDisplayName(quint32 driverId, quint16 & deviceType, int brand=0);

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
