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
    void setDeviceImage(const QString&strImage);

protected:
    bool eventFilter(QObject *watch, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override ;
    void changeEvent(QEvent *pEvt) final;


signals:
    void onReturn();

private:
    Ui::FrameDeviceHolder *ui;

    QList<SuperLabel *>m_pLBtns ;
    QList<QFrame *>m_pFrames ;
    void clickLabel(QLabel *label,int index=0);
};

#endif // FRAMEDEVICEHOLDER_H
