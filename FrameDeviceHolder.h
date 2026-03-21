#ifndef FRAMEDEVICEHOLDER_H
#define FRAMEDEVICEHOLDER_H

#include <QFrame>
#include <QLabel>
#include <QTimer>
#include <QPaintEvent>
#include "SuperLabel.h"


namespace Ui {
class FrameDeviceHolder;
}

class FrameDeviceHolder : public QFrame
{
    Q_OBJECT

public:
    explicit FrameDeviceHolder(QWidget *parent = nullptr);
    ~FrameDeviceHolder();
    void setDevice(void *device,const QString&strImage,const QString&strName);

    void updateBattery(void *device,const QString&battImg,const QString&typeImg,const QString&tip,const QString&qss);
    void updateLayer(int layer);

protected:
    bool eventFilter(QObject *watch, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override ;
    void changeEvent(QEvent *pEvt) final;

signals:
    void onReturn();

private:
    Ui::FrameDeviceHolder *ui;
    void *m_device = nullptr;
    QList<SuperLabel *>m_pLBtns ;
    QList<QFrame *>m_pFrames ;
    void clickLabel(QLabel *label,int index=0);
};

#endif // FRAMEDEVICEHOLDER_H
