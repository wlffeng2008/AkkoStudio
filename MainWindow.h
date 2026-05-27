#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLayout>

#include <QTimer>
#include <QDialog>
#include <QSettings>
#include <QTranslator>
#include <QSystemTrayIcon>

#include "ModuleLangMenu.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ModuleGeneralMasker;
class AkkoDeviceEnum;

class DeviceEnumInfo
{
public:
    DeviceEnumInfo(){};
    ~DeviceEnumInfo(){};

    quint16 VID;
    quint16 PID;
    int driverId;
    int deeviceType; //0 KB, 1 Mouse, 2 HP
    int creator;
    int connectType;
    int battery;
    bool toShow;
    int deviceClor;
    quint32 lastTime;
    QString strName;
    QString strImage;
    QString strPath1;
    QString strPath2;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTranslator *m_pMainTrM = nullptr;
    QTranslator *m_pMainTrA = nullptr;
    QTranslator *m_pMainTrB = nullptr;

    void enumDevice();
    void addDevice(quint16 VID, quint16 PID, quint32 driverId, const QString&path1, const QString&path2, int connectType, int creator);

protected:
    void changeEvent(QEvent *pEvt) final;
    void paintEvent(QPaintEvent *event) final;
    void keyReleaseEvent(QKeyEvent *event) final;
    void mousePressEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    bool eventFilter(QObject *obj, QEvent *e) final;
    void closeEvent(QCloseEvent *event) final;
    void showEvent(QShowEvent *event) final;

    bool event(QEvent *event) final;

signals:
    void oEnumDeiceDone();

private slots:
    void on_pushButtonExit_clicked();
    void on_pushButtonMin_clicked();

private:
    Ui::MainWindow *ui;

    ModuleLangMenu *m_pLangMenu = nullptr;
    ModuleGeneralMasker *m_cover = nullptr;
    AkkoDeviceEnum *m_Enum = nullptr;

    bool m_bForMGK = false;

    QLayout *m_layout = nullptr;

    QPointF m_dragPosition;
    bool m_dragging = false;
    bool m_closeShow = false;
    bool m_bActive = true;

    QSystemTrayIcon *trayIcon = nullptr;
    QSettings *m_pSet = nullptr;

    QTimer *m_pTmHide = nullptr;
    QDialog *m_pFloatLeft = nullptr;
    QDialog *m_pFloatRight = nullptr;
    QDialog *m_pFloatReturn = nullptr;

    int m_creator = 0;
    int m_showId = 0;
    QString m_showPath;
    QList<DeviceEnumInfo *> m_tmp;

    QString m_langSet;
    QString m_langName;
    void updateDeviceInfo();
    QAction *m_act0 = nullptr;
    QAction *m_act1 = nullptr;
    QAction *m_act2 = nullptr;

    HWND m_hCurHwnd = nullptr;

    bool m_bCanReturn = true;

    void setHubSize(bool origin=true);
    void addToHub(DeviceEnumInfo *pDevInfo,int index=0);
};
#endif // MAINWINDOW_H
