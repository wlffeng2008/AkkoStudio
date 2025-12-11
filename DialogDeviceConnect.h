#ifndef DIALOGDEVICECONNECT_H
#define DIALOGDEVICECONNECT_H

#include <QTimer>
#include <QDialog>
#include <QDebug>

#include <windows.h>
#include <dbt.h>

#include <QCoreApplication>
#include <QAbstractNativeEventFilter>

#include <QTableView>
#include <QStandardItemModel>

#include "hidapi.h"


namespace Ui {
class DialogDeviceConnect;
}

typedef enum
{
    CMD_SET_INFOR       = 0x00,  //(disable)
    CMD_GET_INFOR       = 0x8F,
    CMD_SET_RESET       = 0x01,
    CMD_GET_RESET       = 0x81,  //(disable)
    CMD_SET_BATTERY     =	0x02,
    CMD_GET_BATTERY     =	0x82,
    CMD_SET_REPORT      =	0x03,
    CMD_GET_REPORT      =0x83,
    CMD_SET_PROFILE     =0x04,
    CMD_GET_PROFILE     =0x84,
    CMD_SET_LEDONOFF    =0x05,
    CMD_GET_LEDONOFF    =0x85,
    CMD_SET_DEBOUNCE    =0x06,
    CMD_GET_DEBOUNCE    =0x86,
    CMD_SET_LEDPARAM    =0x07,
    CMD_GET_LEDPARAM    =0x87,
    CMD_SET_SLEDPARAM   =0x08,
    CMD_GET_SLEDPARAM   =0x88,
    CMD_SET_KBOPTION    =0x09,
    CMD_GET_KBOPTION    =0x89,
    CMD_SET_KEYMATRIX   =0x0A,
    CMD_GET_KEYMATRIX   =0x8A,
    CMD_SET_MACRO       =0x0B,
    CMD_GET_MACRO       =0x8B,
    CMD_SET_USERPIC     =0x0C,
    CMD_GET_USERPIC     =0x8C,
    CMD_SET_MUSIC       =0x0D,
    CMD_SET_WINDOWS     =0x0E,
    CMD_SET_FN          =0x10,
    CMD_GET_FN          =0x90,
    CMD_SET_SLEEPTIME  =0x11,
    CMD_GET_SLEEPTIME	 =0x91,
    CMD_SET_USERGIF	 =0x12
}KB_CMD;

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
            qDebug() << "nativeEventFilter: " << msg->wParam << msg->wParam;
            //if(msg->wParam == DBT_DEVICEARRIVAL       )  emit devicePluggined(true);
            //if(msg->wParam == DBT_DEVICEREMOVECOMPLETE)  emit devicePluggined(false);
            emit devicePluggined(true);
            return true ;
        }
        return false ;
    }
};

class DialogDeviceConnect : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDeviceConnect(QWidget *parent = nullptr);
    ~DialogDeviceConnect();
    void readSetting();

    static DialogDeviceConnect *instance() ;

    void disconnect() ;
    void startConnect();

    void setLEDOn(bool on);
    void setLEDPicture(int index);
    void setLEDOption(int option);
    void setLEDMode(int mode);
    void setLEDSpeed(int speed);
    void setLEDBright(int bright);
    void setLEDColor(const QColor&color, int option);
    void reset() ;

    void changeKey(quint8 hid, quint8 data0, quint8 data1, quint8 data2, quint8 data3, quint8 subLayer=0);

signals:
    void onConnect();
    void onDisconnect();
    void onReadBack(const QByteArray&data);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    Ui::DialogDeviceConnect *ui;
    QList<QByteArray>m_readList ;
    void addReadCmd(quint8 cmd,int len=8);
    void addReadCmd(QByteArray &cmd);

    hid_device *m_pDev0 = nullptr;
    hid_device *m_pDev1 = nullptr;
    hid_device *m_pDev2 = nullptr;

    QTimer *pTMClear  = nullptr;
    QTimer *m_pRdInput = nullptr;

    bool m_bClear = true ;

    QStandardItemModel *m_pModel = nullptr;
    QTableView *m_pTable = nullptr;

    bool m_bLedOn=true;
    void makeCmd(int row,bool autoSend=false);
    int  getRow(int cmd);
    void setRowValue(int row, int col,int value);

    void addLog(const QByteArray&log);
    QByteArray m_lastCmd ;
};

#endif // DIALOGDEVICECONNECT_H
