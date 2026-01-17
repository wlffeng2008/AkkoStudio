#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <QLabel>
#include <QTimer>
#include <QPaintEvent>
#include "SuperLabel.h"
#include "ModuleLangMenu.h"

class FrameMacro;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

#include "DialogDeviceConnect.h"

class FrameKeySetting;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override ;
    bool eventFilter(QObject *watched, QEvent *event) override ;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override ;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    Ui::MainWindow *ui;
    QPoint m_dragPosition;
    bool m_dragging = false;


    DialogDeviceConnect *m_pDevice = nullptr;
    ModuleLangMenu *m_pLangMenu = nullptr;
    FrameKeySetting *m_pKSetting = nullptr;

    int m_nStatus = 0;
    void setConnect(int nFlag);

    QList<SuperLabel *>m_pLBtns;
    QList<QFrame *>m_pFrames;
    void clickLabel(QLabel *label,int index=0);


};
#endif // MAINWINDOW_H
