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


#include "ModuleLangMenu.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*
class USBNotifier : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit USBNotifier(QObject *parent = nullptr) : QObject(parent) {
        //QCoreApplication::instance()->installNativeEventFilter(this) ;
    }

signals:
    void devicePluggined(bool in=true);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override
    {
        Q_UNUSED(eventType)
        Q_UNUSED(result)
        MSG* msg = reinterpret_cast<MSG*>(message);
        if (msg->message == WM_DEVICECHANGE)
        {
            qDebug() << "USBNotifier::nativeEventFilter: " << msg->wParam << msg->lParam;
            //if(msg->wParam == DBT_DEVICEARRIVAL       )  emit devicePluggined(true);
            //if(msg->wParam == DBT_DEVICEREMOVECOMPLETE)  emit devicePluggined(false);
            emit devicePluggined(true);
            return true;
        }
        return false;
    }
};*/


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

    bool event(QEvent *event) final;

    void enumDevice();
    void addDevice(quint32 id, const QString&path, int connectType, int creator);

private slots:
    void on_pushButtonExit_clicked();
    void on_pushButtonMin_clicked();

private:
    Ui::MainWindow *ui;

    ModuleLangMenu *m_pLangMenu=nullptr;

    QLayout *m_layout = nullptr;

    QPointF m_dragPosition;
    bool m_dragging = false;

    bool m_bWaiting = true;

    QTimer *m_pTmHide = nullptr;
    QTimer *m_pShowRy = nullptr;
    QDialog *m_pFloatLeft = nullptr;
    QDialog *m_pFloatRight = nullptr;
    QDialog *m_pFloatReturn = nullptr;

    int m_creator = 0;
    QString m_langName;
    void updateDeviceInfo();
    QAction *m_act0 = nullptr;
    QAction *m_act1 = nullptr;
    QAction *m_act2 = nullptr;
};
#endif // MAINWINDOW_H
