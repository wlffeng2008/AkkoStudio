#ifndef MODULEKEYBOARD_H
#define MODULEKEYBOARD_H

#include <QAbstractButton>
#include <QFrame>

#include "SuperLabel.h"

namespace Ui {
class ModuleKeyboard;
}

class ModuleKeyboard : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleKeyboard(QWidget *parent = nullptr);
    ~ModuleKeyboard();

    void setKeyFixMode() ;
    void setkeyHited(int id) ;
    void showFlag(bool show=true);
    void showMtFlag(bool show=true);
    void setKeyEnable(const QString&objname, bool bEnable, bool bToDevice, bool bSetToAll=false) ;
    void setLightMode();

    void setKeyTip(const QString&objname,const QString&strTip1,const QString&strTip2,bool bSetToAll=true);
    void setKeyTip(quint8 hid,const QString&strTip1,const QString&strTip2,bool bSetToAll=true);
    void setSingleMode(bool set=true);
    void setSelectCount(int count=3);

    static void Update();

    void keepSpeacial();

signals:
    void onKeyClicked(const QString&text,quint8 hid);
    void onKeyChanged(quint8 hid,quint8 type,bool enable);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched,QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::ModuleKeyboard *ui;

    void setButtonEnable(QAbstractButton*btn,bool bEnable=true, bool bToDevice=true) ;
    QList<QAbstractButton*>m_disables ;
    QAbstractButton*m_curBtn = nullptr;

    CustomTooltip *m_Menu = nullptr;

    bool m_bFixMode = false ;
    int m_nSelectCount = 300 ;

    bool m_bSetLightMode=false;
    bool m_bSetMtMode=false;

    bool m_draging =false ;
    QPoint m_clkPt={0,0} ;
    QPoint m_nowPt={0,0} ;

    QByteArray m_data0;
};

#endif // MODULEKEYBOARD_H
