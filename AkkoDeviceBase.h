#ifndef AKKODEVICEBASE_H
#define AKKODEVICEBASE_H

#include <QObject>
#include <QWidget>

QString getDisplayName(quint32 driverId, quint16 & deviceType, int brand=0);
quint32 getExtrasDevice(quint32 VID, quint32 PID, quint64 MOD,QString & deviceName, quint16 & deviceType, quint16&deviceUSB);

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
