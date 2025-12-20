#ifndef FRAMEKEYSETTING_H
#define FRAMEKEYSETTING_H

#include <QFrame>
#include <QLabel>
#include "ModuleDKSAdjust.h"

namespace Ui {
class FrameKeySetting;
}

class FrameKeySetting : public QFrame
{
    Q_OBJECT

public:
    explicit FrameKeySetting(QWidget *parent = nullptr);
    ~FrameKeySetting();

protected:
    bool eventFilter(QObject*watched,QEvent*event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::FrameKeySetting *ui;
    ModuleDKSAdjust *m_adjust = nullptr;
    QLabel *m_toAdjust = nullptr;
    QWidget *m_pMask = nullptr;

};

#endif // FRAMEKEYSETTING_H
