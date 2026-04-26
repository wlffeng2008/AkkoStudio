#ifndef DIALOGDEVICECONNECT_H
#define DIALOGDEVICECONNECT_H

#include <QTimer>
#include <QDialog>
#include <QDebug>
#include <QSettings>
#include <QThread>

#include <windows.h>
#include <dbt.h>

#include <QCoreApplication>
#include <QAbstractNativeEventFilter>

#include <QTableView>
#include <QStandardItemModel>

#include "hidapi.h"
#include "miniaudio.h"
#include "ModuleGenKeymapping.h"

namespace Ui {
class DialogDeviceConnect;
}

typedef struct
{
    quint16 timeBt ;
    quint16 time24 ;
    quint16 timeDBt ;
    quint16 timeD24 ;

}sleepTime;

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
    CMD_SET_USERGIF	 =0x12,
    CMD_SET_AUTOOSEN   =0x17,
    CMD_GET_AUTOOSEN   =0x97,
    CMD_SET_SKU         =0x50,
    CMD_GET_SKU         =0xD0,
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
            qDebug() << "USBNotifier::nativeEventFilter: " << msg->wParam << msg->lParam;
            //if(msg->wParam == DBT_DEVICEARRIVAL       )  emit devicePluggined(true);
            //if(msg->wParam == DBT_DEVICEREMOVECOMPLETE)  emit devicePluggined(false);
            emit devicePluggined(true);
            return true;
        }
        return false;
    }
};

class DialogDeviceConnect : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDeviceConnect(QWidget *parent = nullptr);
    ~DialogDeviceConnect();

    void readAllData();
    void setProfile(int layer=0);

    static DialogDeviceConnect *instance();

    void disconnect();
    void startConnect();

    void setLEDOn(bool on);
    void setLEDMode(int mode,quint8 opt);
    void setLEDSpeed(int speed);
    void setLEDBright(int bright);
    void setLEDColor(const QColor&color, int option);

    void setKBOption(quint8 option, quint8 value);
    quint8 getKBOption(quint8 option);

    void setMacro(quint8 hid,quint16 repeat,quint8 mode,quint8 macroId,const QByteArray&data);

    void setSleepTime(quint16 value=0,int type=0);
    sleepTime *getSleepTime();

    quint8 getDebounce();
    void setDebounce(quint8 level=0);
    quint8 getReport();
    void setReport(quint8 level=0);

    void reset();

    void restKey(quint8 hid);
    void changeKey(quint8 hid, keyData *pDk, quint8 subLayer=0,quint8 save=1);
    void changeKeyFn(quint8 hid, keyData *pDk, quint8 subLayer=0,quint8 save=1);
    void enableKey(quint8 hid, bool enable=true, quint8 subLayer=0);
    void enableKeyFn(quint8 hid, bool enable=true, quint8 subLayer=0);
    QByteArray getMatix(bool fnLayer=false);
    void getKeydata(keyData *pDk, quint8 index, quint8 layer=0);
    quint8 getSnapkey(quint8 index);
    void send65Cmd(quint8 option,quint8 hid,quint32 data,bool save);
    void send65Cmd(quint8 option,quint8 hid,char *data,quint8 len,bool save);
    quint32 get65Value(quint8 option,quint8 index);
    void set65Value(quint8 option,quint8 index,quint32 value);

    quint8 getKeyType(quint8 hid);
    QStringList getKeyString(quint8 hid);

    void StartCalibration();
    void StopCalibration();

    void DoConnectDevice(quint16 PID,bool bleMode,const QString&path1,const QString&path2);

    bool isLoading();

signals:
    void onConnect();
    void onDisconnect();
    void onReadBack(const QByteArray&data);
    void onUpdataLayer(int layer);
    void onReadDone();
    void onCalibration(const QByteArray&data);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    Ui::DialogDeviceConnect *ui;
    QList<QByteArray>m_cmdList;
    void addReadCmd(quint8 cmd,int len=8,bool execute=false);
    void addReadCmd(QByteArray &cmd,bool execute=false);
    void addReadCmd(const QString&strCmd,bool execute=false);

    void executeCmd();
    QSettings *m_pCntSet = nullptr;

    hid_device *m_pDev0 = nullptr;
    hid_device *m_pDev1 = nullptr;
    hid_device *m_pDev2 = nullptr;

    QTimer *pTMClear = nullptr;
    QTimer *m_pRdInput = nullptr;
    QTimer *m_pExecute = nullptr;

    bool m_bClear = true;
    bool m_bCalibration = false;
    QTimer *m_TMCali = nullptr;

    QStandardItemModel *m_pModel = nullptr;
    QTableView *m_pTable = nullptr;

    sleepTime m_sleepTime={0};
    quint8 m_report=0;
    quint8 m_debounce=0;
    quint8 m_layer=0;
    bool m_bleMode=false;
    QString m_path1;
    QString m_path2;

    bool m_bLedOn=true;
    void makeCmd(int row,bool autoSend=false);
    int  getRow(int cmd);
    void setRowValue(int row, int col,int value);

    void addLog(const QByteArray&log,bool addRetrun=true);
    QByteArray m_lastCmd;

    QByteArray m_E500; /** 读4次 触发行程 = 0,*/
    QByteArray m_E501; /** 读4次 抬起行程 = 1,*/
    QByteArray m_E502; /** 读4次 RT触发行程 = 2,*/
    QByteArray m_E503; /** 读4次 RT抬起行程 = 3,*/
    QByteArray m_E504; /** 读4次 动态键程的起始行程 = 4,*/
    QByteArray m_E505; /** 读2次 MT的长按时间 = 5,*/
    QByteArray m_E506; /** 读4次 死区 = 6,*/
    QByteArray m_E507; /** 读2次 按键模式 = 7*/
    QByteArray m_E508; /** 读2次 动态键程可选标志 = 8,*/
    QByteArray m_E509; /** 读2次 Snap对号 = 9,*/
    QByteArray m_E50A; /** 读8次 所有按键动态键程可选标志 = 10,*/
    QByteArray m_E5FF; /** 读4次 按键按压动态数值 = 0xFF,*/
    QByteArray m_E5FE; /** 读4次 按键按压行程数值 = 0xFE,*/
    QByteArray m_E5FC; /** 读2次 轴体类型 = 0xFC,*/
    QByteArray m_E5FB; /** 读2次 顶部死区 = 0xFB,*/
    QByteArray m_Cali;
    QByteArray m_Optn;
    QByteArray m_Info;
    QByteArray m_KeyMatrix[8];
    QByteArray m_KeyMatrixFn[8];

    bool m_isSupportAxis = false;
    bool m_isSupportTopDeadZone=false;
    quint16 m_multiple = 10;
    quint32 m_deviceId = 0;
    quint16 m_version  = 0;

    bool m_bReadAll = false;
    bool m_bSendMusic = false;
};

#endif // DIALOGDEVICECONNECT_H
