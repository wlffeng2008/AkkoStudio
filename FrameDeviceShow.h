#ifndef FRAMEDEVICESHOW_H
#define FRAMEDEVICESHOW_H

#include <QFrame>
#include <QScrollArea>

#include <AkkoDeviceBase.h>

namespace Ui {
class FrameDeviceShow;
}

class DeviceEnumInfo;

class FrameDeviceShow : public QFrame
{
    Q_OBJECT

public:
    explicit FrameDeviceShow(QWidget *parent = nullptr);
    ~FrameDeviceShow();

    static FrameDeviceShow *getFrameShow(int index,QWidget *parent);
    static bool HideDevieByIndex(int index);

    DeviceEnumInfo *m_pDevEI=nullptr;
    void setDevieInfo(DeviceEnumInfo *pDI);
    void updateBattery();

    void setImage(const QString & image, int type=0);
    void setSelect(bool select=true);


    QScrollArea *m_sa = nullptr;

    bool m_bConacted=false;
    QString m_image;
    QString m_typeImage;

signals:
    void onClicked(DeviceEnumInfo *device,const QString&deviceImg);
    void onReport(DeviceEnumInfo *device,const QString&battImg,const QString&typeImg,const QString&tip,const QString&qss);

protected:
    bool event(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::FrameDeviceShow *ui;
};

#endif // FRAMEDEVICESHOW_H
