#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <windows.h>
#include <dbt.h>

#include <QCoreApplication>
#include <QAbstractNativeEventFilter>

#include <QMainWindow>
#include <QLayout>

#include <QDialog>
#include <QTimer>
#include <QTranslator>
#include <QSystemTrayIcon>


#include "ModuleLangMenu.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ModuleGeneralMasker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTranslator *m_pMainTrM = nullptr;
    QTranslator *m_pMainTrA = nullptr;
    QTranslator *m_pMainTrB = nullptr;

protected:
    void changeEvent(QEvent *pEvt) final;
    void paintEvent(QPaintEvent *event) final;
    void closeEvent(QCloseEvent *event) final;
    void mousePressEvent(QMouseEvent *event) final;
    void keyReleaseEvent(QKeyEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    bool eventFilter(QObject *obj, QEvent *e) final;
     void showEvent(QShowEvent *event) final;

    bool event(QEvent *event) final;

    void enumDevice();
    void addDevice(quint32 id, const QString&path1, const QString&path2, int connectType, int creator);

private slots:
    void on_pushButtonExit_clicked();
    void on_pushButtonMin_clicked();

private:
    Ui::MainWindow *ui;

    ModuleLangMenu *m_pLangMenu = nullptr;
    ModuleGeneralMasker *m_cover = nullptr;

    bool m_bForMGK=false;

    QLayout *m_layout = nullptr;

    QPointF m_dragPosition;
    bool m_dragging = false;
    bool m_closeShow = false;
    bool m_bActive=true;

    QSystemTrayIcon *trayIcon = nullptr;

    QTimer *m_pTmHide = nullptr;
    QDialog *m_pFloatLeft = nullptr;
    QDialog *m_pFloatRight = nullptr;
    QDialog *m_pFloatReturn = nullptr;

    int m_creator = 0;
    QString m_langSet;
    QString m_langName;
    void updateDeviceInfo();
    QAction *m_act0 = nullptr;
    QAction *m_act1 = nullptr;
    QAction *m_act2 = nullptr;

    void setHubSize(bool origin=true);
};
#endif // MAINWINDOW_H
