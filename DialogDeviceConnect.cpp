
//#include <winsock2.h>
//#pragma comment(lib, "ws2_32.lib")

#include "DialogDeviceConnect.h"
#include "qforeach.h"
#include "ui_DialogDeviceConnect.h"

#include "ModuleGenKeymapping.h"

#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QWindow>

#define  MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

typedef struct
{
    QString name;
    quint8 cmd ;
    quint8 nLen;
    quint8 byte1;

    quint8 byte2;
    quint8 byte3;

    quint8 byte4;
    quint8 byte5;

    quint8 byte6;
    quint8 byte7;

}HidCmd;


static QList<HidCmd> CmdTable =
{
    {"CMD_SET_INFOR",CMD_SET_INFOR,8,0,0,0,0,0,0,0},
    {"CMD_GET_INFOR",CMD_GET_INFOR,8,0,0,0,0,0,0,0},
    {"CMD_SET_RESET",CMD_SET_RESET,8,0,0,0,0,0,0,0},
    {"CMD_GET_RESET",CMD_GET_RESET,8,0,0,0,0,0,0,0},
    {"CMD_SET_BATTERY",CMD_SET_BATTERY,8,0,0,0,0,0,0,0},
    {"CMD_GET_BATTERY",CMD_GET_BATTERY,8,0,0,0,0,0,0,0},
    {"CMD_SET_REPORT",CMD_SET_REPORT,8,0,0,0,0,0,0,0},
    {"CMD_GET_REPORT",CMD_GET_REPORT,8,0,0,0,0,0,0,0},
    {"CMD_SET_PROFILE",CMD_SET_PROFILE,8,0,0,0,0,0,0,0},
    {"CMD_GET_PROFILE",CMD_GET_PROFILE,8,0,0,0,0,0,0,0},
    {"CMD_SET_LEDONOFF",CMD_SET_LEDONOFF,8,1,1,0,0,0,0,0},
    {"CMD_GET_LEDONOFF",CMD_GET_LEDONOFF,8,0,0,0,0,0,0,0},
    {"CMD_SET_DEBOUNCE",CMD_SET_DEBOUNCE,8,0,0,0,0,0,0,0},
    {"CMD_GET_DEBOUNCE",CMD_GET_DEBOUNCE,8,0,0,0,0,0,0,0},
    {"CMD_SET_LEDPARAM",CMD_SET_LEDPARAM,9,1,3,3,7,255,255,255},
    {"CMD_GET_LEDPARAM",CMD_GET_LEDPARAM,8,0,0,0,0,0,0,0},
    {"CMD_SET_SLEDPARAM",CMD_SET_SLEDPARAM,9,1,3,3,7,255,255,255},
    {"CMD_GET_SLEDPARAM",CMD_GET_SLEDPARAM,8,0,0,0,0,0,0,0},
    {"CMD_SET_KBOPTION",CMD_SET_KBOPTION,8,0,0,0,0,0,0,0},
    {"CMD_GET_KBOPTION",CMD_GET_KBOPTION,8,0,0,0,0,0,0,0},
    {"CMD_SET_KEYMATRIX",CMD_SET_KEYMATRIX,8,0,0,0,0,0,0,0},
    {"CMD_GET_KEYMATRIX",CMD_GET_KEYMATRIX,8,0,0,0,0,0,0,0},
    {"CMD_SET_MACRO",CMD_SET_MACRO,8,0,0,0,0,0,0,0},
    {"CMD_GET_MACRO",CMD_GET_MACRO,8,0,0,0,0,0,0,0},
    {"CMD_SET_USERPIC",CMD_SET_USERPIC,8,0,0,0,0,0,0,0},
    {"CMD_GET_USERPIC",CMD_GET_USERPIC,8,0,0,0,0,0,0,0},
    {"CMD_SET_MUSIC",CMD_SET_MUSIC,8,0,0,0,0,0,0,0},
    {"CMD_SET_WINDOWS",CMD_SET_WINDOWS,8,0,0,0,0,0,0,0},
    {"CMD_SET_FN",CMD_SET_FN,8,0,0,0,0,0,0,0},
    {"CMD_GET_FN",CMD_GET_FN,8,0,0,0,0,0,0,0},
    {"CMD_SET_SLEEPTIME",CMD_SET_SLEEPTIME,8,0,0,0,0,0,0,0},
    {"CMD_GET_SLEEPTIME",CMD_GET_SLEEPTIME,8,0,0,0,0,0,0,0},
    {"CMD_SET_USERGIF",CMD_SET_USERGIF,8,0,0,0,0,0,0,0},
    {"CMD_SET_AUTOOSEN",CMD_SET_AUTOOSEN,8,0,0,0,0,0,0,0},
    {"CMD_GET_AUTOOSEN",CMD_GET_AUTOOSEN,8,0,0,0,0,0,0,0},
    {"CMD_SET_SKU",CMD_SET_SKU,8,0,0,0,0,0,0,0},
    {"CMD_GET_SKU",CMD_GET_SKU,8,0,0,0,0,0,0,0},
    {"",0,8,0,0,0,0,0,0,0}
};

static DialogDeviceConnect *s_connect = nullptr;
DialogDeviceConnect *DialogDeviceConnect::instance()
{
    return s_connect;
}


static float m_MusicBuf[1920];

// 音频数据回调函数：每次获取到音频数据时触发
static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    const float* pcmBuf = (const float*)pInput;
    //ma_uint32 dataSize = frameCount * pDevice->playback.channels * sizeof(float);

    for (int i = 0; i < 960; i++)
    {
        m_MusicBuf[i] = pcmBuf[i];
    }

    //TRACE(_T("--------%s %d,%d,%d,%d,%d,%d,%d,%d,%d\n"), CA2W((pDevice->playback.name)), dataSize, (int)(pcmDataO[2] * 6), (int)(pcmDataO[25] * 6), (int)(pcmDataO[49] * 6), (int)(pcmDataO[83] * 6),
    //	(int)(pcmDataO[360] * 6), (int)(pcmDataO[400] * 6), (int)(pcmDataO[430] * 6), (int)(pcmDataO[470] * 6));
    //if (pWDlg) pWDlg->DrawCurv(pcmDataO);
}

DialogDeviceConnect::DialogDeviceConnect(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogDeviceConnect)
{
    ui->setupUi(this);
    s_connect = this ;
    setWindowFlags(windowFlags() |  Qt::MSWindowsFixedSizeDialogHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint );

    {
        static ma_device_config config = ma_device_config_init(ma_device_type_loopback);
        config.playback.format = ma_format_f32;
        config.playback.channels = 2;           // 声道数：立体声
        config.sampleRate = 0;                  // Set to 0 to use the device's native sample rate.
        config.dataCallback = data_callback;    // This function will be called when miniaudio needs more data.
        config.pUserData = this;

        static ma_device device;
        if (ma_device_init(NULL, &config, &device) != MA_SUCCESS)
        {
            qCritical() << "ma_device_init ERROR --------";
        }

        device.pUserData = this;
        ma_device_start(&device);     // The device is sleeping by default so you'll need to start it manually.

        QTimer *pTMMusic = new QTimer(this);
        connect(pTMMusic,&QTimer::timeout,this,[=]{

            if(!m_bSendMusic) return;

            QByteArray cmd(128,0);
            cmd[1] = 0x0d;
            cmd[8] = 0xf2;
            for(int i=0; i<22; i++)
            {
                quint8 value = ((quint8)(fabs(m_MusicBuf[i]) * 6.0)) % 0xFF;
                if(value>6) value = 6;
                cmd[i+9] = value;
            }
            hid_device *pDev = m_pDev2;

            char buf[1024] = {0};
            pTMMusic->stop();
            for(int i=0; i<10; i++)
            {
                hid_send_feature_report(pDev,(quint8 *)cmd.data(),65);
                QThread::msleep(20);
                hid_get_feature_report(pDev,(quint8 *)buf,65);
            }
            pTMMusic->start(20);
        });
        pTMMusic->start(20);
    }

    connect(ui->checkBoxOntop,&QCheckBox::clicked,this,[=](bool checked){
        QWindow *pWin = windowHandle();
        if(checked)
            pWin->setFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        else
            pWin->setFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    });

    m_pCntSet = new QSettings(QApplication::applicationDirPath() + "/device.ini",QSettings::IniFormat);
    ui->lineEditVID->setText(m_pCntSet->value("lastVID","3151").toString());
    ui->lineEditPID->setText(m_pCntSet->value("lastPID","502F").toString());

    connect(ui->pushButtonSaveMatrix,&QPushButton::clicked,this,[=]{
        setDefaultMatrix(m_KeyMatrix[0]);
        quint8 *data = (quint8 *)m_KeyMatrix[0].data();
        for(int i=0; i<512; i+=4)
        {
            qDebug().noquote() << QString::asprintf("%3d,%3d,%3d,%3d,  // %3d  0x%08X", data[i+0], data[i+1], data[i+2], data[i+3], i/4, qToBigEndian(*(quint32*)(data + i)) );
        }
    });

    {
        m_pTable = ui->tableView;
        m_pModel = new QStandardItemModel();
        m_pModel->setHorizontalHeaderLabels(QString("命令,数据长度,Byte0\n命令号(Hex),Byte1\nEffect,Byte2\nSpeed,Byte3\nBrightness,Byte4\nOption,Byte5\nR,Byte6\nG,Byte7\nB").split(',')) ;
        m_pTable->setModel(m_pModel);

        QHeaderView *pHDV = m_pTable->horizontalHeader();
        pHDV->setSectionResizeMode(QHeaderView::Stretch);
        pHDV->setSectionResizeMode(0,QHeaderView::Fixed);
        pHDV->resizeSection(0,165);
        pHDV->setSectionResizeMode(2,QHeaderView::Fixed);
        pHDV->resizeSection(2,90);
        pHDV->setSectionResizeMode(5,QHeaderView::Fixed);
        pHDV->resizeSection(5,90);

        foreach(const HidCmd & cmd,CmdTable)
        {
            if(cmd.name.isEmpty())
                continue;
            QStandardItem *item0 = new QStandardItem(cmd.name);
            QStandardItem *item1 = new QStandardItem(QString("%1").arg(cmd.nLen));
            QStandardItem *item2 = new QStandardItem(QString("%1").arg(cmd.cmd,2,16,QLatin1Char('0')).toUpper());
            QStandardItem *item3 = new QStandardItem(QString("%1").arg(cmd.byte1));
            QStandardItem *item4 = new QStandardItem(QString("%1").arg(cmd.byte2));
            QStandardItem *item5 = new QStandardItem(QString("%1").arg(cmd.byte3));
            QStandardItem *item6 = new QStandardItem(QString("%1").arg(cmd.byte4));
            QStandardItem *item7 = new QStandardItem(QString("%1").arg(cmd.byte5));
            QStandardItem *item8 = new QStandardItem(QString("%1").arg(cmd.byte6));
            QStandardItem *item9 = new QStandardItem(QString("%1").arg(cmd.byte7));

            QFont font = this->font();
            font.setBold(true);

            item0->setEditable(false);
            item1->setEditable(false);
            item2->setEditable(false);

            item0->setForeground(QBrush(Qt::blue));
            if(cmd.nLen == 9)
            {
                item1->setFont(font);
                item1->setForeground(QBrush(Qt::red));
                item2->setForeground(QBrush(Qt::red));
                font.setItalic(true);
                item0->setFont(font);
                item0->setBackground(QBrush(Qt::white));
            }

            item2->setFont(font);
            font.setBold(false);
            font.setItalic(false);

            m_pModel->appendRow({item0,item1,item2,item3,item4,item5,item6,item7,item8,item9});
            m_pTable->setRowHeight(m_pModel->rowCount()-1,20);
        }

        connect(m_pModel,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item){
            makeCmd(item->row());
        });
        connect(m_pTable,&QTableView::clicked,this,[=](const QModelIndex &index){
            int row = index.row();
            if(m_pModel->item(row,2)->text().toInt(nullptr,16) == CMD_SET_LEDONOFF)
            {
                int col = index.column();
                if(col == 3 || col == 4)
                {
                    int val = !m_pModel->item(row,col)->text().toInt();
                    m_pModel->item(row,col)->setText(QString::number(val));
                }
            }
            else
            {
                makeCmd(row);
            }
        });

        connect(m_pTable,&QTableView::doubleClicked,this,[=](const QModelIndex &index){
            int row = index.row();
            int col = index.column();
            if(col <= 2)
            {
                makeCmd(row,true);
            }
        });

        m_pTable->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_pTable,&QTableView::customContextMenuRequested,this,[=](const QPoint &pos){
            const QModelIndex index = m_pTable->indexAt(pos);
            int row = index.row();
            int col = index.column();
            if(col <= 2)
            {
                makeCmd(row,true);
            }
        });
    }

    QTimer *pTMUsb = new QTimer(this);
    static USBNotifier *pUsb = new USBNotifier(this);
    QCoreApplication::instance()->installNativeEventFilter(pUsb);
    connect(pUsb, &USBNotifier::devicePluggined,this,[=](bool in) {
        pTMUsb->stop();
        pTMUsb->start(800);
    });

    connect(pTMUsb,&QTimer::timeout,this,[=]{
        pTMUsb->stop();
        ui->pushButtonRefresh->click();
    });

    connect(ui->comboBoxPID,&QComboBox::currentIndexChanged,this,[=](int index){
        ui->lineEditPID->setText(ui->comboBoxPID->currentText());
        ui->pushButtonConnect->click();
    });
    connect(ui->pushButtonRefresh,&QPushButton::clicked,this,[=]{
        quint16 VID = ui->lineEditVID->text().trimmed().toUInt(nullptr,16);

        QStringList VIDList;
        hid_device_info *pRoot = hid_enumerate(VID,0);
        hid_device_info *pTemp = pRoot;
        while(pTemp)
        {
            if(pTemp->usage_page == 0xFFFF && pTemp->usage == 1)
            {
                VIDList.push_back(QString::asprintf("%04X", pTemp->product_id));
            }
            pTemp = pTemp->next;
        }
        hid_free_enumeration(pRoot);

        ui->comboBoxPID->blockSignals(true);

        ui->comboBoxPID->clear();
        ui->comboBoxPID->addItems(VIDList);

        QString strPID = ui->lineEditPID->text().trimmed().toUpper();
        int index = ui->comboBoxPID->findText(strPID);
        if(index == -1) index = 0;
        ui->comboBoxPID->setCurrentIndex(index);

        ui->comboBoxPID->blockSignals(false);

        ui->pushButtonConnect->click();
    });

    connect(ui->pushButtonConnect,&QPushButton::clicked,this,[=]{
        disconnect();

        quint16 VID = ui->lineEditVID->text().trimmed().toUInt(nullptr,16);
        quint16 PID = ui->lineEditPID->text().trimmed().toUInt(nullptr,16);

        hid_device_info *pRoot = hid_enumerate(VID,PID);
        hid_device_info *pTemp = pRoot;
        while(pTemp)
        {
            if(pTemp->usage_page == 0xFFFF)
            {
                if(pTemp->usage == 1)
                {
                    m_pDev0 = hid_open_path(pTemp->path);
                    hid_set_nonblocking(m_pDev0,1);
                    ui->labelFlag0->setPixmap(QString(":/images/General_OK4.png"));
                }

                if(pTemp->usage == 2)
                {
                    m_pDev1 = hid_open_path(pTemp->path);
                    m_pDev2 = hid_open_path(pTemp->path);
                    ui->labelFlag1->setPixmap(QString(":/images/General_OK4.png"));
                }
            }
            pTemp = pTemp->next;

            if(m_pDev0 && m_pDev1 && m_pDev2)
            {
                emit onConnect();

                m_isSupportAxis = false;
                m_isSupportTopDeadZone=false;
                m_multiple = 10;
                m_deviceId = 0;
                m_version  = 0;
                addReadCmd(CMD_GET_INFOR,true);
                m_pCntSet->setValue("lastVID",ui->lineEditVID->text().trimmed());
                m_pCntSet->setValue("lastPID",ui->lineEditPID->text().trimmed());
                break;
            }
        }
        hid_free_enumeration(pRoot);

        if(!m_pDev0 || !m_pDev1)
        {
            emit onDisconnect();
        }
    });

    connect(ui->pushButtonWrite,&QPushButton::clicked,this,[=]{
        if(!m_pDev1) return;
        QString strCmd = ui->lineEditCmd->text().trimmed();
        QByteArray data(QByteArray::fromHex(strCmd.toLatin1()));
        addReadCmd(data,true);
    });

    connect(ui->pushButtonRead,&QPushButton::clicked,this,[=]{

        char buf[1024] = {0};
        int nlen = hid_get_feature_report(m_pDev1,(quint8 *)buf,65);
        if(nlen > 0)
        {
            QByteArray data(buf+1,nlen-1);
            quint8 cmd = (quint8)data[0];
            int row = getRow(cmd);

            if(cmd & 0x80)
            {
                for(int i=1; i<8; i++)
                {
                    setRowValue(row,2+i,(quint8)data[i]);
                }
            }
            //qDebug() << "get_:" << data.toHex(' ').toUpper();
            quint8 *pCmd=(quint8 *)m_lastCmd.data();
            QString strInfo;
            switch(pCmd[0])
            {
            case CMD_GET_INFOR:
                m_Info = data;
                m_version  = *(quint16*)(data.data()+7);
                m_deviceId = *(quint32*)(data.data()+1);
                strInfo = QString::asprintf("Ver:0x%03X Id:%d[%03X]",m_version,m_deviceId,m_deviceId);
                qDebug() << strInfo;
                ui->labelVersion->setText(strInfo);

                if(m_version >= 0x300)
                    m_isSupportAxis = true;
                if(m_version >= 0x400)
                    m_isSupportTopDeadZone = true;

                if(m_version<0x300)
                    m_multiple = 10;
                else if(m_version < 0x500)
                    m_multiple = 100;
                else if(m_version >= 0x500)
                    m_multiple = 200;

                addReadCmd(CMD_GET_KBOPTION,true);
                break;

            case CMD_GET_KBOPTION:
                m_Optn = data;
                addReadCmd(CMD_GET_PROFILE,true);
                break;

            case CMD_GET_PROFILE:
                //qDebug() << "CMD_GET_PROFILE:" << data.left(16).toHex(' ').toUpper();
                //setProfile(data[1]);
                emit onUpdataLayer(data[1]);
                break;

            case CMD_GET_KEYMATRIX:
                m_KeyMatrix[pCmd[4]].append(data);
                break;

            case CMD_GET_SLEEPTIME:
                m_sleepTime = *(sleepTime *)(data.data() + 8);
                break;

            case CMD_GET_DEBOUNCE:
                m_debounce = data[1];
                break;

            case CMD_GET_REPORT:
                m_report = data[2];
                break;

            case CMD_GET_LEDPARAM:
                break;
            {
                int row = getRow(CMD_SET_LEDPARAM);
                setRowValue(row,3,data[1]);
                setRowValue(row,4,4 - data[2]);
                setRowValue(row,5,data[3]);

                if(data[1] == 0x16) m_bSendMusic=true;
            }
            break;

            case CMD_GET_SLEDPARAM:
            {
                int row = getRow(CMD_SET_SLEDPARAM);
                setRowValue(row,3,data[1]);
                setRowValue(row,4,4 - data[2]);
                setRowValue(row,5,data[3]);
            }
            break;

            case 0xE5:
                if(m_bCalibration && pCmd[1] == 0xFE)
                {
                    m_Cali.append(data);
                    if(pCmd[3] == 3)
                    {
                        static QByteArray tmp;
                        tmp = m_Cali;
                        m_Cali.clear();
                        emit onCalibration(tmp);
                    }
                    break;
                }

                if(pCmd[1] == 0x00) m_E500.append(data);
                if(pCmd[1] == 0x01) m_E501.append(data);
                if(pCmd[1] == 0x02) m_E502.append(data);
                if(pCmd[1] == 0x03) m_E503.append(data);
                if(pCmd[1] == 0x04) m_E504.append(data);
                if(pCmd[1] == 0x05) m_E505.append(data);
                if(pCmd[1] == 0x06) m_E506.append(data);
                if(pCmd[1] == 0x07) m_E507.append(data);
                if(pCmd[1] == 0x08) m_E508.append(data);
                if(pCmd[1] == 0x09) m_E509.append(data);
                if(pCmd[1] == 0x0A) m_E50A.append(data);
                if(pCmd[1] == 0xFF) m_E5FF.append(data);
                if(pCmd[1] == 0xFE) m_E5FE.append(data);
                if(pCmd[1] == 0xFC) m_E5FC.append(data);
                if(pCmd[1] == 0xFB) m_E5FB.append(data);
                break;
            }

            emit onReadBack(data);

            addLog(m_lastCmd.left(8),false);
            addLog(data);

            executeCmd(); // next cmd
        }
    });

    pTMClear = new QTimer(this);
    connect(pTMClear,&QTimer::timeout,this,[=]{
        pTMClear->stop();
        m_bClear=true;
    });

    m_pRdInput = new QTimer(this);
    m_pRdInput->start(20);
    connect(m_pRdInput,&QTimer::timeout,this,[=]{
        m_pRdInput->stop();
        char buf[1024] = {0};
        if(m_pDev0)
        {
            int nlen = hid_read(m_pDev0,(quint8 *)buf,64);

            if(nlen > 0)
            {
                QByteArray data(buf,nlen);
                qDebug() << "read:" << data.toHex(' ').toUpper();
                addLog(data);
                if(data[0] == 0x05 || data[1] == 0x04)
                {
                    if(data[2] == 0x16) m_bSendMusic=true;
                    addReadCmd(CMD_GET_LEDPARAM,true);
                }
            }
        }
        m_pRdInput->start(20);
    });

    m_pExecute = new QTimer(this);
    connect(m_pExecute,&QTimer::timeout,this,[=]{
        m_pExecute->stop();
        executeCmd();
    });

    ui->lineEditCmd->setStyleSheet("font-family: Fixedsys;");
    ui->lineEditPID->setStyleSheet("font-family: Fixedsys;");
    ui->lineEditVID->setStyleSheet("font-family: Fixedsys;");
    ui->plainTextEdit->setStyleSheet("font-family: Fixedsys;");
}

void DialogDeviceConnect::DoConnectDevice(quint16 PID)
{
    QString strPID = QString::asprintf("%04X",PID);
    ui->lineEditPID->setText(strPID);
    ui->pushButtonConnect->click();
}

void DialogDeviceConnect::readAllData()
{
    m_bReadAll=true;
    m_bSendMusic=false;
    m_cmdList.clear();
    m_multiple = 10;
    m_version = 0;

    m_KeyMatrix[0].clear();
    m_KeyMatrix[1].clear();
    m_KeyMatrix[2].clear();
    m_KeyMatrix[3].clear();
    m_KeyMatrix[4].clear();
    m_KeyMatrix[5].clear();
    m_KeyMatrix[6].clear();
    m_KeyMatrix[7].clear();
    m_E500.clear();
    m_E501.clear();
    m_E502.clear();
    m_E503.clear();
    m_E504.clear();
    m_E505.clear();
    m_E506.clear();
    m_E507.clear();
    m_E508.clear();
    m_E509.clear();
    m_E50A.clear();
    m_E5FF.clear();
    m_E5FE.clear();
    m_E5FC.clear();
    m_E5FB.clear();

    addReadCmd(CMD_GET_REPORT);
    addReadCmd(CMD_GET_LEDPARAM);
    addReadCmd(CMD_GET_SLEDPARAM);
    addReadCmd(CMD_GET_DEBOUNCE);
    addReadCmd(CMD_GET_SLEEPTIME);

    quint8 layer = m_layer;
    for(quint8 subLayer=0; subLayer<4; subLayer++) // subLayer;
    {
        for(quint8 page=0; page<8; page++) // page
        {
            quint8 tmp[8] = {CMD_GET_KEYMATRIX,layer,0xFF,page, subLayer,0,0,0};
            QByteArray cmd((char *)tmp,8);
            addReadCmd(cmd);
        }
    }

    if(m_isSupportAxis)
    {
        quint8 readE5s[]=
        {
            0x00, 4,
            0x01, 4,
            0x02, 4,
            0x03, 4,
            0x04, 4,
            0x05, 2,
            0x06, 4,
            0x07, 2,
            0x08, 2,
            0x09, 2,
            0x0A, 8,
            0xFF, 4,
            0xFE, 4,
            0xFC, 2,
            0xFB, 2
        };

        QString strCmd;
        int nCount = sizeof(readE5s)/sizeof(quint8);
        for(int i=0; i<nCount; i+=2)
        {
            quint8 option = readE5s[i+0];
            quint8 countR = readE5s[i+1];

            for(int j=0; j<countR; j++)
            {
                addReadCmd(strCmd.asprintf("E5 %02X 01 %02X",option,j));
            }
        }
    }
}

DialogDeviceConnect::~DialogDeviceConnect()
{
    delete ui;
}

void DialogDeviceConnect::executeCmd()
{
    if(m_cmdList.count() <= 0)
    {
        if(m_bReadAll)
        {
            m_bReadAll=false;
            emit onReadDone();
        }
        return;
    }

    QByteArray cmd = m_cmdList[0];
    m_cmdList.pop_front();
    cmd.append(128,0);

    int nLen = 8;
    if( (quint8)cmd[0] == CMD_SET_LEDPARAM ||
        (quint8)cmd[0] == CMD_SET_SLEDPARAM)
        nLen = 9;

    quint8 sum = 0;
    for(int i=0; i<nLen-1; i++)
        sum += cmd[i];
    sum = 0xFF - (sum & 0xFF);
    cmd[nLen-1] = sum;

    m_lastCmd = cmd;

    //qDebug() << "send:" << cmd.left(16).toHex(' ').toUpper();

    cmd.insert(0,(char)0) ; // report id
    hid_send_feature_report(m_pDev1,(quint8 *)cmd.data(),65);

    QTimer::singleShot(20,this,[=]{
        ui->pushButtonRead->click();
    });
}

void DialogDeviceConnect::setProfile(int layer)
{
    m_layer = layer;
    QByteArray cmd(12,0);
    cmd[0]=CMD_SET_PROFILE;
    cmd[1]=layer;
    addReadCmd(cmd,true);

    QTimer::singleShot(500,this,[=]{readAllData();});
}

void DialogDeviceConnect::set65Value(quint8 option,quint8 index,quint32 value)
{
    if(index >= 128) return;
    int type = 1;
    char *buf = nullptr;
    switch(option)
    {
    case 0x00: buf=m_E500.data(); type=2; break;
    case 0x01: buf=m_E501.data(); type=2; break;
    case 0x02: buf=m_E502.data(); type=2; break;
    case 0x03: buf=m_E503.data(); type=2; break;
    case 0x04: buf=m_E504.data(); type=2; break;
    case 0x05: buf=m_E505.data(); type=1; break;
    case 0x06: buf=m_E506.data(); type=2; break;
    case 0x07: buf=m_E507.data(); type=1; break;
    case 0x08: buf=m_E508.data(); type=1; break;
    case 0x09: buf=m_E509.data(); type=1; break;
    case 0x0A: buf=m_E50A.data(); type=4; break;
    case 0xFF: buf=m_E5FF.data(); type=2; break;
    case 0xFE: buf=m_E5FE.data(); type=2; break;
    case 0xFC: buf=m_E5FC.data(); type=1; break;
    case 0xFB: buf=m_E5FB.data(); type=1; break;
    }

    if(buf)
    {
        if(type == 1) ((quint8  *)buf)[index] = value;
        if(type == 2) ((quint16 *)buf)[index] = value;
        if(type == 4) ((quint32 *)buf)[index] = value;
    }
}

quint32 DialogDeviceConnect::get65Value(quint8 option,quint8 index)
{
    if(index >= 128) return 0;
    int type = 1;
    char *buf = nullptr;
    switch(option)
    {
    case 0x00: buf=m_E500.data(); type=2; break;
    case 0x01: buf=m_E501.data(); type=2; break;
    case 0x02: buf=m_E502.data(); type=2; break;
    case 0x03: buf=m_E503.data(); type=2; break;
    case 0x04: buf=m_E504.data(); type=2; break;
    case 0x05: buf=m_E505.data(); type=1; break;
    case 0x06: buf=m_E506.data(); type=2; break;
    case 0x07: buf=m_E507.data(); type=1; break;
    case 0x08: buf=m_E508.data(); type=1; break;
    case 0x09: buf=m_E509.data(); type=1; break;
    case 0x0A: buf=m_E50A.data(); type=4; break;
    case 0xFF: buf=m_E5FF.data(); type=2; break;
    case 0xFE: buf=m_E5FE.data(); type=2; break;
    case 0xFC: buf=m_E5FC.data(); type=1; break;
    case 0xFB: buf=m_E5FB.data(); type=1; break;
    }

    if(buf)
    {
        if(type == 1) return ((quint8  *)buf)[index];
        if(type == 2) return ((quint16 *)buf)[index];
        if(type == 4) return ((quint32 *)buf)[index];
    }

    return 0;
}

quint8 DialogDeviceConnect::getKeyType(quint8 hid)
{
    //if(m_version<0x300)
    //    return 0;
    int index = getIndex(hid);
    quint8 type = get65Value(0x07,index);
    return type;
}

quint8 DialogDeviceConnect::getSnapkey(quint8 index)
{
    quint8 snapHid = get65Value(0x09,index);
    return snapHid;
}

void DialogDeviceConnect::getKeydata(keyData *pDk,quint8 index,quint8 layer)
{
    quint8 *data = (quint8 *)m_KeyMatrix[layer].data();
    pDk->b0 = data[index*4 + 0];
    pDk->b1 = data[index*4 + 1];
    pDk->b2 = data[index*4 + 2];
    pDk->b3 = data[index*4 + 3];
}

QStringList DialogDeviceConnect::getKeyString(quint8 hid)
{
    QStringList res;

    int index = getIndex(hid);
    if(m_E507.size() <= index)
        return res;
    quint8 type = m_E507[index];

    if(type == 7)
    {
        quint8 snapHid = getSnapkey(index);
        res.push_back(::getKeyValue(snapHid));
    }
    else
    {
        for(int sonlayer=0; sonlayer<4; sonlayer++)
        {
            keyData kd;
            getKeydata(&kd,index,sonlayer);
            res.push_back(::getKeyString(&kd,false));
        }
    }

    return res;
}

void DialogDeviceConnect::send65Cmd(quint8 option, quint8 hid, quint32 data, bool save)
{
    send65Cmd(option,hid,(char *)&data,4,save);
}

void DialogDeviceConnect::send65Cmd(quint8 option, quint8 hid, char *data, quint8 len, bool save)
{
    quint8 index=getIndex(hid);
    quint8 pack[8] = {0x65, option, 0, index, save, 0, 0, 0};
    QByteArray snd((char*)pack,8);
    snd.append(data,len);
    addReadCmd(snd,true);

    set65Value(option,index,*(quint32 *)data);
}

void DialogDeviceConnect::changeKey(quint8 hid,  keyData*pDk, quint8 subLayer, quint8 save)
{
    quint8 index=getIndex(hid);
    quint8 pack[12] = {CMD_SET_KEYMATRIX, m_layer, index, 0,   0, save, subLayer, 0,   pDk->b0, pDk->b1, pDk->b2, pDk->b3};
    QByteArray snd((char*)pack,12);
    addReadCmd(snd,true);

    ((keyData*)m_KeyMatrix[subLayer].data())[index] = *(keyData*)pDk;
}

void DialogDeviceConnect::restKey(quint8 hid)
{
    keyData kd={0,0,hid,0};
    changeKey(hid,&kd,0,false);
    kd.b2=0;
    changeKey(hid,&kd,1,false);
    changeKey(hid,&kd,2,false);
    changeKey(hid,&kd,3,true);

    send65Cmd(0x07,hid,0x0000,false);
    send65Cmd(0x00,hid,0x0190,false);
    send65Cmd(0x01,hid,0x0230,false);
    send65Cmd(0x06,hid,0x003C,false);
    send65Cmd(0xFB,hid,0x003C,false);
    send65Cmd(0xFC,hid,0x0002,true);
}

void DialogDeviceConnect::setSleepTime(quint16 value, int type)
{
    if(type==0) m_sleepTime.timeBt =value*60;
    if(type==1) m_sleepTime.time24 =value*60;
    if(type==2) m_sleepTime.timeDBt=value*60;
    if(type==3) m_sleepTime.timeD24=value*60;

    quint8 tmp[12] = {CMD_SET_SLEEPTIME, 0, 0, 0, 0, 0, 0, 0};
    QByteArray snd((char*)tmp,8);
    snd.append((char *)&m_sleepTime,8);
    addReadCmd(snd,true);
}

sleepTime *DialogDeviceConnect::getSleepTime()
{
    return &m_sleepTime;
}

quint8 DialogDeviceConnect::getDebounce()
{
    return m_debounce;
}

void DialogDeviceConnect::setDebounce(quint8 level)
{
    int row = getRow(CMD_SET_DEBOUNCE);
    if(row == -1) return;
    setRowValue(row,3,level);
    m_debounce = level;
    makeCmd(row,true);
}

quint8 DialogDeviceConnect::getReport()
{
    return m_report;
}

void DialogDeviceConnect::setReport(quint8 level)
{
    int row = getRow(CMD_SET_REPORT);
    if(row == -1) return;
    setRowValue(row,4,level);
    m_report=level;
    makeCmd(row,true);
}

void DialogDeviceConnect::enableKey(quint8 hid, bool enable, quint8 subLayer)
{
    keyData set = {0};
    if(enable) set.b2 = hid;
    changeKey(hid, &set, subLayer);
}

QByteArray DialogDeviceConnect::getMatix(int sub)
{
    return m_KeyMatrix[0];
}

void DialogDeviceConnect::addLog(const QByteArray&log, bool addRetrun)
{
    if(m_bClear) ui->plainTextEdit->clear();
    QString strLog(log.toHex(' ').toUpper());
    int nLen = strLog.length() ;
    for(int at=nLen-12; at>0; at -= 12)
        strLog.insert(at,' ');
    ui->plainTextEdit->appendPlainText(strLog);
    if(addRetrun)
        ui->plainTextEdit->appendPlainText("\n");
    m_bClear = false;
    pTMClear->stop();
    pTMClear->start(500);
}

void DialogDeviceConnect::addReadCmd(quint8 cmd, int len, bool execute)
{
    QByteArray data(len,0);
    data[0] = cmd;
    addReadCmd(data, execute);
}

void DialogDeviceConnect::addReadCmd(const QString&strCmd, bool execute)
{
    QByteArray cmd = QByteArray::fromHex(strCmd.toLatin1());
    addReadCmd(cmd, execute);
}

void DialogDeviceConnect::addReadCmd(QByteArray&cmd, bool execute)
{
    if(!m_pDev1) return;

    m_cmdList.push_back(cmd);

    if(execute)
    {
        executeCmd();
    }
    else
    {
        m_pExecute->stop();
        m_pExecute->start(10);
    }
}

void DialogDeviceConnect::startConnect()
{
    ui->pushButtonRefresh->click();
}

void DialogDeviceConnect::makeCmd(int row, bool autoSend)
{
    QByteArray data;
    data.append((char)m_pModel->item(row,2)->text().toInt(nullptr,16));
    for(int i=3; i<10; i++)
    {
        data.append((char)m_pModel->item(row,i)->text().toInt());
    }

    ui->lineEditCmd->setText(data.toHex(' ').toUpper());

    if(autoSend) ui->pushButtonWrite->click();
}

void DialogDeviceConnect::reset()
{
    makeCmd(getRow(CMD_SET_RESET),true);
    m_KeyMatrix[0] = ::getDefaultMatrix();
    readAllData();
}

void DialogDeviceConnect::disconnect()
{
    if(m_pDev0) hid_close(m_pDev0);
    if(m_pDev1) hid_close(m_pDev1);
    if(m_pDev2) hid_close(m_pDev2);

    m_pDev0 = nullptr;
    m_pDev1 = nullptr;
    m_pDev2 = nullptr;

    ui->labelFlag0->setPixmap(QString(":/images/General_NG4.png"));
    ui->labelFlag1->setPixmap(QString(":/images/General_NG4.png"));
}

bool DialogDeviceConnect::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message == WM_DEVICECHANGE)
    {
        switch (msg->wParam)
        {
        case DBT_DEVICEARRIVAL:
        case DBT_DEVICEREMOVECOMPLETE:
            ui->pushButtonRefresh->click();
            break;
        }
    }

    return QDialog::nativeEvent(eventType, message, result);
}

int DialogDeviceConnect::getRow(int cmd)
{
    int count = m_pModel->rowCount();
    for(int i=0; i<count; i++)
    {
        int value = m_pModel->item(i,2)->text().toInt(nullptr, 16);
        if(value == cmd)
            return i;
    }
    return -1;
}

void DialogDeviceConnect::setLEDOn(bool on)
{
    int row = getRow(CMD_SET_LEDONOFF);
    if(row == -1) return;
    setRowValue(row, 3, on ? 0 : 1);
    setRowValue(row, 4, on ? 0 : 1);
    makeCmd(row, true);
}

void DialogDeviceConnect::setRowValue(int row, int col, int value)
{
    QStandardItem *item = m_pModel->item(row,col);
    if(item) item->setText(QString::number(value));
}


void DialogDeviceConnect::setLEDMode(int mode, quint8 opt)
{
    int row = getRow(CMD_SET_LEDPARAM);
    if(row == -1) return;

    m_bSendMusic = (mode == 0x16);
    setRowValue(row, 3, mode);
    if(mode != 0x0D)
    {
        setRowValue(row, 6, (opt<<4) | 0x07);
    }
    else
    {
        setRowValue(row, 6, (opt<<4) );
    }

    makeCmd(row, true);
}

void DialogDeviceConnect::setLEDSpeed(int speed)
{
    int row = getRow(CMD_SET_LEDPARAM);
    if(row == -1) return;
    setRowValue(row, 4, 4-speed);
    makeCmd(row, true);
}

void DialogDeviceConnect::setLEDBright(int bright)
{
    int row = getRow(CMD_SET_LEDPARAM);
    if(row == -1) return;
    setRowValue(row, 5, bright);
    makeCmd(row, true);
}

void DialogDeviceConnect::setLEDColor(const QColor&color, int option)
{
    int row = getRow(CMD_SET_LEDPARAM);
    if(row == -1) return;
    setRowValue(row, 6, option);
    if(option>6)
    {
        setRowValue(row, 7, color.red()  );
        setRowValue(row, 8, color.green());
        setRowValue(row, 9, color.blue() );
    }

    makeCmd(row,true);
}

void DialogDeviceConnect::setMacro(quint8 hid, quint16 repeat, quint8 mode, quint8 macroId, const QByteArray&data)
{
    QByteArray buf((char*)&repeat, 2);
    buf.append(data);

    quint8 count = buf.size() / 56;
    quint8 tail  = buf.size() % 56;
    if(tail>0)
    {
        buf.append(56 - tail, 0);
        count++;
    }

    for(quint8 i=0; i<count; i++)
    {
        quint8 szTmp[] = {CMD_SET_MACRO, macroId, i, 56, (i == count-1), 0, 0, 0};
        QByteArray pack((char *)szTmp, 8);
        pack.append(buf.left(56));
        buf.remove(0, 56);
        addReadCmd(pack);
    }

    keyData set{0x09, mode, macroId, 0};
    changeKey(hid, &set);
}

void DialogDeviceConnect::StartCalibration()
{
    m_bCalibration = true;
    if(!m_TMCali)
        m_TMCali = new QTimer(this);
    m_Cali.clear();
    addReadCmd("1E 01", true);
    m_TMCali->start(120);
    connect(m_TMCali,&QTimer::timeout, this, [=]{
        static quint64 index = 0;
        addReadCmd(QString::asprintf("E5 FE 01 %02X", index++ % 4));
    });
}

void DialogDeviceConnect::StopCalibration()
{
    m_bCalibration = false;
    if(m_TMCali)
        m_TMCali->stop();
    m_cmdList.clear();
    addReadCmd("1E 00", true);
}

void DialogDeviceConnect::setKBOption(quint8 option, quint8 value)
{
    m_Optn[0     ] = CMD_SET_KBOPTION;
    m_Optn[option] = value;
    addReadCmd(m_Optn,true);
}

quint8 DialogDeviceConnect::getKBOption(quint8 option)
{
    return m_Optn[option];
}
