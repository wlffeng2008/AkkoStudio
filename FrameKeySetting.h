#ifndef FRAMEKEYSETTING_H
#define FRAMEKEYSETTING_H

#include <QFrame>
#include <QLabel>
#include "ModuleDKSAdjust.h"
#include "ModuleGenKeymapping.h"

namespace Ui {
class FrameKeySetting;
}

class FrameKeySetting : public QFrame
{
    Q_OBJECT

public:
    explicit FrameKeySetting(QWidget *parent = nullptr);
    ~FrameKeySetting();

    void refresh();

protected:
    bool eventFilter(QObject*watched,QEvent*event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::FrameKeySetting *ui;
    ModuleDKSAdjust *m_adjust = nullptr;
    QLabel  *m_toAdjust = nullptr;
    QWidget *m_pMask = nullptr;

    quint8 m_DKSHid=0;
    qreal m_DKSLen=0.7;
    keyData m_DKS1;
    keyData m_DKS2;
    keyData m_DKS3;
    keyData m_DKS4;

    quint8 m_MTHid=0;
    keyData m_MT1;
    keyData m_MT2;

    quint8 m_TGLHid=0;
    keyData m_TGL1;

    quint8 m_SnapHid1=0;
    quint8 m_SnapHid2=0;

    int m_setType=0;
    bool m_bUpdating=false;
};

#endif // FRAMEKEYSETTING_H
