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
    void setKeyEnable(const QString&objname,bool bEnable,bool bSetToAll=false) ;
    void setSingleMode(bool set=true) ;
    void setSelectCount(int count=3) ;

    static void Update();

signals:
    void onKeyClicked(const QString&text,quint8 hid);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched,QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::ModuleKeyboard *ui;

    QList<QAbstractButton*>m_disables ;
    QAbstractButton*m_curBtn = nullptr;
    QAbstractButton*m_spcBtn = nullptr;
    CustomTooltip *m_Menu = nullptr;

    bool m_bFixMode = false ;
    int m_nSelectCount = 300 ;

    bool m_draging =false ;
    QPoint m_clkPt={0,0} ;
    QPoint m_nowPt={0,0} ;

    QByteArray m_data0;
};

#endif // MODULEKEYBOARD_H
