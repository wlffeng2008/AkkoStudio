#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ModuleLangMenu.h"
#include "FrameDeviceShow.h"
#include "AkkoDeviceBase.h"
#include "DialogDeviceConnect.h"
#include "framedeviceholder.h"
#include "ModuleGeneralMasker.h"
#include "FrameSystemInfo.h"

#include "hidapi.h"
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QStyleOption>
#include <QThread>
#include <QTimer>
#include <QWheelEvent>
#include <QProcess>
#include <QSettings>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QRgb>
#include <QAction>
#include <Qdir>
#include <QFile>
#include <QLibraryInfo>

#include <QSharedMemory>
#define UNIQUE_KEY "AkkoAudioApp_71A7F2D4-5566-4F99"

#ifdef Q_OS_WIN
#include <windows.h>
#endif


static HWND s_hWndEmb0 = NULL;
static HWND s_hWndEmb1 = NULL;
static QSettings settings("HKEY_CURRENT_USER\\Software\\Akko",QSettings::NativeFormat);

#include<tlhelp32.h>
static void killProcess(QString processName)
{
    if (processName.isEmpty()) return;

    HANDLE hRootHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hRootHandle) return;

    PROCESSENTRY32 pEntry = {0};
    pEntry.dwSize = sizeof(PROCESSENTRY32);

    BOOL bProcess = ::Process32First(hRootHandle, &pEntry);

    while (bProcess)
    {
        QString strProcName = QString::fromWCharArray(pEntry.szExeFile);
        if (strProcName.compare(processName,Qt::CaseInsensitive) == 0)
        {
            HANDLE handLe = ::OpenProcess(PROCESS_TERMINATE, FALSE, pEntry.th32ProcessID);
            if (handLe) ::TerminateProcess(handLe, 0);
        }
        bProcess = ::Process32Next(hRootHandle, &pEntry);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    {
        static QSharedMemory sharedMemory(UNIQUE_KEY);

        if (sharedMemory.attach()) {
            QMessageBox::warning(nullptr, tr("提示"), QString("AKKOStudio") + tr("程序已经在运行中！"));
            hide();
            exit(0);
            qApp->quit(); // 直接退出
            return;
        }
        sharedMemory.create(1);
    }

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("Akko Cloud Dirver Hub");

    setStyleSheet("QMainWindow{ background-color: rgba(255, 255, 255, 1); border: 1px solid skyblue; border-radius: 20px; }");

    {
        QString strPath = QApplication::applicationDirPath() + "/images";
        QDir d(strPath);

        QDir::Filters filters = QDir::Files | QDir::NoDotAndDotDot | QDir::Readable;
        QStringList nameFilters;
        // nameFilters << "*.txt" << "*.cpp";
        QStringList fileList = d.entryList(nameFilters, filters);
        for(QString strFile:fileList)
        {
            QString strNew = strFile;
            strNew.replace(" ","-");
            QFile::rename(strPath + "/" + strFile, strPath+ "/" + strNew);
        }
    }

    m_pLangMenu = new ModuleLangMenu(this);
    connect(m_pLangMenu, &ModuleLangMenu::onLangChanged, this, [=](int langId, const QString &lang) {
        m_langName = QString(" ") + lang ;
        settings.setValue("lastlang",langId);

        {
            QStringList langs = {"zh_CN","en_US","zh_TW","ja_JP","ko_KR","ru_RU","vi_VN","pt_PT","th_TH","de_DE","fr_FR","sv_SE","it_IT","tr_TR"};
            QString baseName = langs[langId] + ".qm";
            m_pMainTrM->load(QString(":/i18n/AKKOStudio_")+baseName);
            m_pMainTrA->load("qtbase_" + baseName, QLibraryInfo::path(QLibraryInfo::TranslationsPath));
            m_pMainTrB->load("qt_" + baseName, QLibraryInfo::path(QLibraryInfo::TranslationsPath));
        }

        {
            QStringList langs = {"cn","en","tw","jp","kr","ru","vi","pt","th","de","fr","sv","it","tr"};
            settings.setValue("LanguageCode",langs[langId]);
        }

        //     语言/区域	语言/区域标记	语言/区域标识符	语言/区域十进制标识符
        //     阿拉伯语(沙特阿拉伯)	ar-SA	0x0401	1025
        //     巴斯克语(巴斯克语)	eu-ES	0x042d	1069
        //     保加利亚语(保加利亚)	bg-BG	0x0402	1026
        //     加泰罗尼亚语	ca-ES	0x0403	1027
        //     中文（繁体，香港特别行政区）	zh-HK
        //         注意： 不再使用。 请参阅 zh-TW。

        //     0x0c04	3076
        //     中文(简体，中国)	zh-CN	0x0804	2052
        //     中文(繁体，台湾)	zh-TW	0x0404	1028
        //     克罗地亚语(克罗地亚)	hr-HR	0x041a	1050
        //     捷克语(捷克共和国)	cs-CZ	0x0405	1029
        //     丹麦语(丹麦)	da-DK	0x0406	1030
        //     荷兰语(荷兰)	nl-NL	0x0413	1043
        //     英语(美国)	en-US	0x0409	2052
        //     英语(英国)	en-GB	0x0809	2057
        //     爱沙尼亚语(爱沙尼亚)	et-EE	0x0425	1061
        //     芬兰语(芬兰)	fi-FI	0x040b	1035
        //     法语(加拿大)	fr-CA	0x0c0c	3084
        //     法语(法国)	fr-FR	0x040c	1036
        //     加利西亚语	gl-ES	0x0456	1110
        //     德语(德国)	de-DE	0x0407	1031
        //     希腊语(希腊)	el-GR	0x0408	1032
        //     希伯来语(以色列)	he-IL	0x040d	1037
        //     匈牙利语(匈牙利)	hu-HU	0x040e	1038
        //     印度尼西亚语(印度尼西亚)	id-ID	0x0421	1057
        //     意大利语(意大利)	it-IT	0x0410	1040
        //     日语(日本)	ja-JP	0x0411	1041
        //     韩语(韩国)	ko-KR	0x0412	1042
        //     拉脱维亚语(拉脱维亚)	lv-LV	0x0426	1062
        //     立陶宛语(立陶宛)	lt-LT	0x0427	1063
        //     挪威语(博克马尔语，挪威)	nb-NO	0x0414	1044
        //     波兰语(波兰)	pl-PL	0x0415	1045
        //     葡萄牙语（巴西）	pt-BR	0x0416	1046
        //     葡萄牙语(葡萄牙)	pt-PT	0x0816	2070
        //     罗马尼亚语(罗马尼亚)	ro-RO	0x0418	1048
        //     俄语(俄罗斯)	ru-RU	0x0419	1049
        //     塞尔维亚语(拉丁语，塞尔维亚共和国)	sr-Latn-CS
        //         注意： 不再使用。 请参阅 sr-Latn-RS。

        //     0x081a	2074
        //     塞尔维亚语(拉丁语，塞尔维亚共和国)	sr-Latn-RS	0x241A	9242
        //     斯洛伐克语(斯洛伐克)	sk-SK	0x041b	1051
        //     斯洛文尼亚语(斯洛文尼亚)	sl-SI	0x0424	1060
        //     西班牙语(墨西哥)	es-MX	0x080a	2058
        //     西班牙语(西班牙)	es-ES	0x0c0a	3082
        //     瑞典语(瑞典)	sv-SE	0x041d	1053
        //     泰语(泰国)	th-TH	0x041e	1054
        //     土耳其语（土耳其）	tr-TR	0x041f	1055
        //     乌克兰语(乌克兰)	uk-UA	0x0422	1058
        //     越南语	vi-VN	0x042a	1066
        {
            int id = langId;
            QList<int> langs= {2052,1033,1028,1041,1042,1049,1066,1046,1054,1031,1036,1053,1040,1055};
            settings.setValue("LANGUAGE",langs[id]);
        }
    });

    connect(ui->pushButtonLang, &QPushButton::clicked, this, [=] {
        ModuleLangMenu *pLangMenu = m_pLangMenu;
        if (!pLangMenu->isHidden())
            return;

        QRect btnRect = ui->pushButtonLang->geometry();
        QPoint PT = mapToGlobal(btnRect.bottomLeft());
        pLangMenu->setGeometry(PT.x(), PT.y(), 110, 280);
        pLangMenu->show();
    });

    connect(ui->pushButtonSet, &QPushButton::clicked, this, [=] {

        FrameSystemInfo *pSetInfo= new FrameSystemInfo(this);
        ModuleGeneralMasker gMask(pSetInfo,ui->stackedWidget);
        pSetInfo->show();
        pSetInfo->update();
        gMask.setStyleSheet("QDialog { background-color: rgba(200, 200, 200, 0.6); border: none; border-radius: 20px; }");
        gMask.exec();
        pSetInfo->deleteLater();
    });

    m_layout = ui->scrollAreaWidgetContents->layout();

    ui->labelPrev->setHidden(true);
    ui->labelNext->setHidden(true);
    ui->pushButtonSet->setHidden(true);

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QTimer *pCheck = new QTimer(this);
    connect(pCheck, &QTimer::timeout, this, [=] {
        bool toHide = m_layout->count() == 0;

        static bool last = false;
        if (last != toHide)
        {
            ui->stackedWidget->setHidden(toHide);
            ui->labelLogo->setHidden(toHide);
            update();
        }

        last = toHide;
    });
    pCheck->start(100);

    m_pFloatLeft = new QDialog(this);
    m_pFloatRight = new QDialog(this);
    m_pFloatReturn = new QDialog(this);

    m_pFloatLeft->setWindowFlags(  m_pFloatLeft->windowFlags() | Qt::FramelessWindowHint | Qt::Popup | Qt::WindowStaysOnTopHint|Qt::Tool | Qt::Dialog);
    m_pFloatRight->setWindowFlags(m_pFloatRight->windowFlags() | Qt::FramelessWindowHint | Qt::Popup | Qt::WindowStaysOnTopHint|Qt::Tool | Qt::Dialog);
    m_pFloatReturn->setWindowFlags(m_pFloatReturn->windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
    m_pFloatLeft->setAttribute(Qt::WA_TranslucentBackground);
    m_pFloatRight->setAttribute(Qt::WA_TranslucentBackground);
    m_pFloatReturn->setAttribute(Qt::WA_TranslucentBackground);
    m_pFloatLeft->setStyleSheet( "background-color: rgba(0, 255, 0, 0.8); border: 1px solid skyblue; border-radius: 30px; ");
    m_pFloatRight->setStyleSheet("background-color: rgba(0, 0, 255, 0.8); border: 1px solid skyblue; border-radius: 30px; ");
    m_pFloatLeft->setCursor(Qt::PointingHandCursor);
    m_pFloatRight->setCursor(Qt::PointingHandCursor);
    m_pFloatReturn->setCursor(Qt::PointingHandCursor);
    //m_pFloatLeft->raise();
    //m_pFloatRight->raise();

    m_pFloatLeft->setWindowOpacity(0.6);
    m_pFloatRight->setWindowOpacity(0.6);

    ui->labelPrev->installEventFilter(this);
    ui->labelNext->installEventFilter(this);
    m_pFloatLeft->installEventFilter(this);
    m_pFloatRight->installEventFilter(this);
    m_pFloatReturn->installEventFilter(this);
    ui->labelLogo->installEventFilter(this);
    ui->scrollArea->viewport()->installEventFilter(this);

    m_pTmHide = new QTimer(this);
    connect(m_pTmHide,&QTimer::timeout,this,[=]{
        m_pTmHide->stop();
        m_pFloatLeft->hide();
        m_pFloatRight->hide();
    });

    QTimer *pTMUsb = new QTimer(this);
    static USBNotifier *pUsb = new USBNotifier(this);
    QCoreApplication::instance()->installNativeEventFilter(pUsb);
    connect(pUsb, &USBNotifier::devicePluggined,this,[=](bool in) {
        pTMUsb->stop();
        pTMUsb->start(800);
    });

    connect(pTMUsb,&QTimer::timeout,this,[=]{
        pTMUsb->stop();
        ui->pushButtonScan->click();
    });

    settings.setValue("AkkoReturn", 0);
    settings.setValue("AkkoWnd", 0);
    static  QString strLastPath = settings.value("DevicePath").toString();
    static  QString strVdPath = settings.value("VendorDevicePath").toString();
    //settings.setValue("DevicePath","");
    settings.setValue("iotManagerInitialized",false);

    QTimer *pTMRet = new QTimer(this);
    pTMRet->start(50);
    connect(pTMRet,&QTimer::timeout,this,[=]{
        if(settings.value("AkkoReturn", 0).toInt() == 1)
        {
            settings.setValue("AkkoReturn", 0);
            ui->stackedWidget->setCurrentIndex(0);
            m_pFloatReturn->hide();
        };
    });

    //qDebug()<< QProcess::systemEnvironment();
    killProcess("Akko.exe");
    killProcess("Akko Cloud Driver v4.exe");

    QProcess::startDetached("Akko.exe", QStringList{"/super"});
    QProcess::startDetached("RyExe/Akko Cloud Driver v4.exe", QStringList{});
    QTimer::singleShot(2000,this,[=]{
        //qDebug() << strLastPath;
        settings.setValue("DevicePath","");
        settings.setValue("VendorDevicePath","");
    });

    QTimer::singleShot(4000,this,[=]{
        //qDebug() << strLastPath;
        settings.setValue("DevicePath",strLastPath);
        settings.setValue("VendorDevicePath",strVdPath);
    });

    QTimer *pTMFindWnd = new QTimer(this);
    pTMFindWnd->start(300);

    connect(pTMFindWnd,&QTimer::timeout,this,[=]{
        if(!s_hWndEmb0)
        {
            s_hWndEmb0 = ::FindWindow(nullptr, (LPCWSTR)QString("Akko Cloud Driver").utf16());

            HWND hWnd = s_hWndEmb0;
            if(hWnd)
            {
                settings.setValue("AkkoWnd", 0);
                LONG style = ::GetWindowLongPtr(hWnd, GWL_STYLE);
                style &= ~WS_CAPTION;
                style &= ~WS_POPUP;
                style &= ~WS_BORDER;
                style &= ~WS_THICKFRAME;
                style &= ~WS_MAXIMIZEBOX;
                style &= ~WS_MINIMIZEBOX;
                ::SetWindowLongPtr(hWnd, GWL_STYLE, style|WS_CHILD);

                LONG exStyle = ::GetWindowLongPtr(hWnd, GWL_EXSTYLE);
                exStyle &= ~WS_EX_DLGMODALFRAME;
                ::SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);

                ::SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE);
            }
        }

        if(!s_hWndEmb1)
        {
            s_hWndEmb1 = (HWND)(settings.value("AkkoWnd", 0).toUInt());
            if(!s_hWndEmb1) s_hWndEmb1 = ::FindWindow(nullptr, (LPCWSTR)QString("Akko").utf16());

            HWND hWnd = s_hWndEmb1;
            if(hWnd)
            {
                LONG style = ::GetWindowLongPtr(hWnd, GWL_STYLE);
                style &= ~WS_CAPTION;
                style &= ~WS_POPUP;
                style &= ~WS_BORDER;
                style &= ~WS_THICKFRAME;
                style &= ~WS_MAXIMIZEBOX;
                style &= ~WS_MINIMIZEBOX;
                ::SetWindowLongPtr(hWnd, GWL_STYLE, style|WS_CHILD);

                LONG exStyle = ::GetWindowLongPtr(hWnd, GWL_EXSTYLE);
                exStyle &= ~WS_EX_DLGMODALFRAME;
                ::SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);

                ::SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE);
            }
        }

        HWND hParentWnd = (HWND)ui->frameEmb->winId();
        if(s_hWndEmb0)
        {
            ::SetParent(s_hWndEmb0,hParentWnd);
            ::SetWindowPos(s_hWndEmb0, HWND_TOP, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE);
        }

        if(s_hWndEmb1)
        {
            ::SetParent(s_hWndEmb1,hParentWnd);
            ::SetWindowPos(s_hWndEmb1, HWND_TOP, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE);
        }

        static int nCount = 0;
        if(nCount ++ > 15 || (s_hWndEmb0 && s_hWndEmb1))
        {
            pTMFindWnd->stop();
        }
    });

    {
        QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
        trayIcon->setIcon(QIcon(":/images/logo.png"));
        trayIcon->setToolTip("AKKO Cloud Driver");
        trayIcon->show();

        connect(trayIcon,&QSystemTrayIcon::activated,this,[=](QSystemTrayIcon::ActivationReason reason){
            if(reason != QSystemTrayIcon::Context)
            {
                if(this->isHidden() || this->isMinimized())
                    this->showNormal();
                else
                    this->hide();
            }
        }) ;

        QMenu *trayMenu = new QMenu(this);

        QAction *showAction = new QAction("", this);
        QAction *hideAction = new QAction("", this);
        QAction *exitAction = new QAction("", this);

        m_act0 = showAction;
        m_act1 = hideAction;
        m_act2 = exitAction;

        trayMenu->addAction(showAction);
        trayMenu->addAction(hideAction);
        trayMenu->addSeparator();
        trayMenu->addAction(exitAction);

        connect(exitAction, &QAction::triggered, this, &QApplication::quit);
        connect(showAction, &QAction::triggered, this, &QMainWindow::showNormal);
        connect(hideAction, &QAction::triggered, this, &QMainWindow::hide);
        trayIcon->setContextMenu(trayMenu);

        updateDeviceInfo();
    }

    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->frameHold,&FrameDeviceHolder::onReturn,this,[=]{
        ui->stackedWidget->setCurrentIndex(0);
    });

    QTimer::singleShot(50,this,[=]{ enumDevice(); });
    connect(ui->pushButtonScan, &QPushButton::clicked, this, [=] { enumDevice(); });

    QTimer::singleShot(10,this,[=]{ m_pLangMenu->setLanguage(settings.value("lastlang").toInt());});
    QTimer::singleShot(8000,this,[=]{ m_bWaiting = false; });
    DialogDeviceConnect *pCnn =DialogDeviceConnect::instance();
    connect(pCnn,&DialogDeviceConnect::onUpdataLayer,this,[=](int layer){
        ui->frameHold->updateLayer(layer);
    });

    m_pShowRy = new QTimer(this);
    connect(m_pShowRy,&QTimer::timeout,this,[=]{
        if(m_pFloatReturn->isVisible())
        {
            HWND hWnd = m_creator == 0 ? s_hWndEmb0 : s_hWndEmb1;
            ::SetWindowPos(hWnd, HWND_TOP, 0, 0, ui->frameEmb->width(), ui->frameEmb->height()-20,  SWP_SHOWWINDOW | SWP_FRAMECHANGED);

            ::SetFocus(hWnd);
            ::SetActiveWindow(hWnd);
            ::BringWindowToTop(hWnd);
        }
    });
    m_pShowRy->start(100);
    //resize(2560,1800);
}

void MainWindow::addDevice(quint32 id, const QString &path1, const QString &path2, int connectType, int creator)
{
    AkkoDeviceInfo *dev = getDevice(id);
    if (dev)
    {
        FrameDeviceShow *device = FrameDeviceShow::getFrameShow(m_layout->count(),this);
        if (dev->type == 0)device->setFixedWidth(970);
        if (dev->type == 1)device->setFixedWidth(280);
        if (dev->type == 2)device->setFixedWidth(280);
        device->m_sa = ui->scrollArea;
        device->m_device = dev;
        device->m_connect = connectType;
        device->setName(dev->name,dev->type);
        device->setPath(path1,path2);
        device->setCreator(creator);
        device->show();
        m_layout->addWidget(device);
        m_layout->setSpacing(20);

        if(!device->m_bConacted)
        {
            device->m_bConacted = true;
            connect(device,&FrameDeviceShow::onReport,this,[=](void *dev,const QString&battImg,const QString&typeImg,const QString&tip,const QString&qss){
                ui->frameHold->updateBattery(dev,battImg,typeImg,tip,qss);
            });

            connect(device,&FrameDeviceShow::onClicked,this,[=](void *dev,const QString&path1,const QString&path2,const QString&image,int creator){

                ::SetWindowPos(s_hWndEmb1, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);
                ::SetWindowPos(s_hWndEmb0, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);

                m_creator = creator;
                m_pLangMenu->hide();
                AkkoDeviceInfo *pInfo = static_cast<AkkoDeviceInfo *>(dev);
                QList<quint16>IdList={2807,3131};//,2743
                if(IdList.contains(pInfo->id))
                {
                    DialogDeviceConnect::instance()->DoConnectDevice(pInfo->PID);
                    ui->stackedWidget->setCurrentIndex(2);
                    ui->frameHold->setDevice(pInfo,image,pInfo->name);
                    return;
                }

                if(creator == 0)
                {
                    if(m_bWaiting)
                    {
                        qDebug() << "Waiting for ......" ;
                        return;
                    }
                    // settings.setValue("iotManagerInitialized",true);
                    QString strlastPath = settings.value("DevicePath").toString();
                    if(strlastPath != path2)
                    {
                        settings.setValue("VendorDevicePath","");
                        settings.setValue("DevicePath","");
                        QTimer::singleShot(500,this,[=]{
                            settings.setValue("VendorDevicePath",path1);
                            settings.setValue("DevicePath",path2);
                        });
                    }
                    ui->frameEmb->setStyleSheet("#frameEmb{background-color: rgb(237,237,237); border-bottom-left-radius: 20px; border-bottom-right-radius:20px;}");
                }
                else
                {
                    settings.setValue("AkkoDeviceIndex",pInfo->id);
                    ui->frameEmb->setStyleSheet("#frameEmb{background-color: rgb(30, 30, 30); border-bottom-left-radius: 20px; border-bottom-right-radius:20px;}");
                }

                QTimer::singleShot(200,this,[=]{
                    m_pFloatReturn->setHidden(m_creator == 1);
                    ui->stackedWidget->setCurrentIndex(1);
                    HWND hWnd = m_creator == 0 ? s_hWndEmb0 : s_hWndEmb1;
                    HWND hParentWnd = (HWND)ui->frameEmb->winId();
                    ::SetParent(hWnd,hParentWnd);
                    ::SetWindowPos(hWnd, HWND_TOP, 0, 0, ui->frameEmb->width(), ui->frameEmb->height()-20, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
                    ui->stackedWidget->update();
                    ui->frameEmb->update();
                    ::RedrawWindow(hWnd, NULL, NULL, 0x07|RDW_UPDATENOW);

                    ::SetFocus(hWnd);
                    ::SetActiveWindow(hWnd);
                    update();
                });
            });
        }
    }
}

void MainWindow::changeEvent(QEvent *pEvt)
{
    if(pEvt->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        updateDeviceInfo();
    }
    QMainWindow::changeEvent(pEvt);
}

static void QLog(const char *buf,int nlen=16)
{
    QByteArray data(buf + 1, nlen);
    qDebug() << "tmp :" << data.left(nlen).toHex(' ').toUpper();
}

void MainWindow::enumDevice()
{
    while (m_layout->count())
    {
        FrameDeviceShow *item = (FrameDeviceShow *)m_layout->takeAt(0)->widget();
        if (!item) break;
        m_layout->removeWidget(item);
        item->hide();
    }

    QList<quint16> VidList = {0x3151, 0x38EE, 0x25A7, 0x05AC, 0x0461};
    foreach (quint16 VID, VidList)
    {
        hid_device_info *pRoot = hid_enumerate(VID, 0);
        hid_device_info *pTemp = pRoot;

        QString path1,path2;
        while (pTemp)
        {
            if(pTemp->usage_page == 0xFFFF)
            {
                if(pTemp->usage == 1) path1 = pTemp->path;
                if(pTemp->usage == 2) path2 = pTemp->path;
            }

            if(!path1.isEmpty() && !path2.isEmpty())
            {
                hid_device *pDev = hid_open_path(path2.toStdString().c_str());
                if (!pDev) continue;

                quint16 PID = pTemp->product_id;

                QByteArray cmd(120, 0);
                cmd[1] = 0x8F;
                cmd[8] = 0xFF - cmd[1];

                hid_send_feature_report(pDev, (quint8 *)cmd.data(), 65);
                QThread::msleep(20);

                char buf[128] = {0};
                int nlen = hid_get_feature_report(pDev, (quint8 *)buf, 65);
                if (nlen > 0)
                {
                    QByteArray data(buf + 1, nlen - 1);
                    quint32 id = *(quint32 *)(data.data() + 1);

                    int connectType=0;
                    if(id>0x1000)
                    {
                        {
                            QByteArray tmp(120, 0);
                            tmp[1] = 0xf6;
                            tmp[2] = 0x0A;
                            tmp[8] = 0xFF - tmp[1] - tmp[2];
                            hid_send_feature_report(pDev, (quint8 *)tmp.data(), 65);
                            QThread::msleep(10);
                            nlen = hid_get_feature_report(pDev, (quint8 *)buf, 65);
                            QLog(buf);

                            while(1)
                            {
                                tmp[1] = 0xf7;
                                tmp[2] = 0x00;
                                tmp[8] = 0xFF - tmp[1] - tmp[2];
                                hid_send_feature_report(pDev, (quint8 *)tmp.data(), 65);
                                QThread::msleep(15);
                                nlen = hid_get_feature_report(pDev, (quint8 *)buf, 65);
                                if(buf[6] == 1)
                                    break;
                            }
                            QLog(buf);

                            tmp[1] = 0x8F;
                            tmp[2] = 0x00;
                            tmp[8] = 0xFF - tmp[1] - tmp[2];
                            hid_send_feature_report(pDev, (quint8 *)tmp.data(), 65);
                            QThread::msleep(20);
                            nlen = hid_get_feature_report(pDev, (quint8 *)buf, 65);

                            while(1)
                            {
                                tmp[1] = 0xf7;
                                tmp[2] = 0x00;
                                tmp[8] = 0xFF - tmp[1] - tmp[2];
                                hid_send_feature_report(pDev, (quint8 *)tmp.data(), 65);
                                QThread::msleep(15);
                                nlen = hid_get_feature_report(pDev, (quint8 *)buf, 65);
                                if(buf[0] == 0)
                                    break;
                            }

                            tmp[1] = 0xfc;
                            tmp[2] = 0x00;
                            tmp[8] = 0xFF - tmp[1] - tmp[2];
                            hid_send_feature_report(pDev, (quint8 *)tmp.data(), 65);
                            QThread::msleep(15);
                            nlen = hid_get_feature_report(pDev, (quint8 *)buf, 65);

                            connectType=1;
                        }

                        if (nlen > 0)
                        {
                            QByteArray data(buf + 1, nlen - 1);
                            id = *(quint32 *)(data.data() + 1);
                            qDebug() << "get_:" << data.left(16).toHex(' ').toUpper() << id << Qt::hex << PID;
                        }
                    }
                    else
                    {
                        qDebug() << "get_:" << data.left(16).toHex(' ').toUpper() << id << Qt::hex << PID;
                    }

                    addDevice(id,path1,path2,connectType,0);
                    path1.clear();
                    path2.clear();
                }

                hid_close(pDev);
            }
            pTemp = pTemp->next;
        }
        hid_free_enumeration(pRoot);
    }

    QString strCmd0("04 00 00 1a 06 00 00 00");
    QString strCmd1("04 00 00 30 06 00 00 00");
    QList<quint16>VShengs = {0x38EE,0x320F};
    foreach (quint16 VID, VShengs)
    {
        hid_device_info *pRoot = hid_enumerate(VID, 0);
        hid_device_info *pTemp = pRoot;
        while (pTemp)
        {
            //qDebug()<< pTemp->path << pTemp->usage << pTemp->usage_page;
            if(pTemp->usage == 146 && pTemp->usage_page == 65308)
            {
                hid_device *pDev = hid_open_path(pTemp->path);
                if(!pDev) continue;

                quint16 PID = pTemp->product_id;

                quint8 szBuf[128] = {0};
                QByteArray cmd = QByteArray::fromHex(strCmd0.toLatin1());
                hid_write(pDev,(quint8*)cmd.data(),cmd.size());
                QThread::msleep(5);
                int len = hid_read_timeout(pDev,szBuf,16,500);

                QByteArray Log((char *)szBuf,len);
                qDebug() << "read:" << Log.left(16).toHex(' ').toUpper() << Qt::hex << PID;
                quint8 device = szBuf[11];

                cmd = QByteArray::fromHex(strCmd1.toLatin1());
                hid_write(pDev,(quint8*)cmd.data(),cmd.size());
                QThread::msleep(5);
                hid_read_timeout(pDev,szBuf,16,500);
                hid_close(pDev);

                Log.clear();
                Log.append((char *)szBuf,len);
                qDebug() << "read:" << Log.left(16).toHex(' ').toUpper() << Qt::hex << PID;
                //发送：04 00 00 30 06 00 00 00指令，鼠标回复第12个字节区分同pid的设备，值：
                //灵动V9 ultra 为0
                //泰坦N9 ultra 为4
                //灵动V9 max   为1
                //泰坦N9 max   为3
                // 4：Pulse 01
                // 5：3108 RF
                // 6：AG ONE
                // 7：灵动V9 Max
                // 8：泰坦N9 Max
                // 9：灵动V9 Ultra
                //10：泰坦N9 Ultra
                //11：3087
                quint32 devId = 0;
                quint8 connectType=0;
                switch(PID)
                {
                case 0x000B:
                case 0x000C:
                    devId = 4;
                    if(device != 0) devId = 6;
                    break;

                case 0x0010:
                case 0x0011:
                case 0x000D:
                case 0x000F:
                    if(device == 0) devId = 9;
                    if(device == 4) devId = 10;
                    if(device == 1) devId = 7;
                    if(device == 3) devId = 8;
                    break;

                case 0x5151:
                case 0x5152:
                    devId = 5;
                    break;

                case 0x0007:
                case 0x0008:
                    devId = 11;
                    break;
                case 0x22b4:
                    devId = 12;
                    break;
                }
                if(PID == 0xf) connectType=1;

                addDevice(devId,"null",pTemp->path,connectType,1);
            }
            pTemp = pTemp->next;
        }
        hid_free_enumeration(pRoot);
    }
    updateDeviceInfo();
}

void MainWindow::updateDeviceInfo()
{
    QString strInfo = QString(tr("我的设备")) + QString("(%1)").arg(m_layout->count());
    ui->labelInfo0->setText(strInfo);

    m_act0->setText(tr("显示窗口"));
    m_act1->setText(tr("隐藏窗口"));
    m_act2->setText(tr("退出程序"));

    ui->pushButtonLang->setText(m_langName);
}

MainWindow::~MainWindow() { delete ui; }

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::MouseButtonRelease)
    {
        QScrollBar *sb = ui->scrollArea->horizontalScrollBar();

        if (ui->labelPrev == obj || m_pFloatLeft == obj)
        {
            m_pLangMenu->hide();
            sb->setValue(sb->value() - ui->scrollArea->width());
        }

        if (ui->labelNext == obj || m_pFloatRight == obj)
        {
            m_pLangMenu->hide();
            sb->setValue(sb->value() + ui->scrollArea->width());
        }

        if (m_pFloatReturn == obj)
        {
            static int nFlag = 0;
            if(++nFlag <= 2)
            {
                settings.setValue("DevicePath","");
                settings.setValue("VendorDevicePath","");
            }
            ui->stackedWidget->setCurrentIndex(0);
            m_pFloatReturn->hide();
        }

        ui->scrollArea->update();

        if(ui->labelLogo == obj)
        {
            DialogDeviceConnect::instance()->show();
        }
    }

    if(e->type() != QEvent::Paint)
    {
        qreal scale = 1.0;
        int nYPos = geometry().height()/2 + 50;
        QPoint P1 = mapToGlobal(QPoint(20,nYPos));
        QPoint P2 = mapToGlobal(QPoint(geometry().width() - 60*scale,nYPos));
        m_pFloatLeft->setGeometry(P1.x(),P1.y(),48*scale,48*scale);
        m_pFloatRight->setGeometry(P2.x(),P2.y(),48*scale,48*scale);

        QPoint P3 = mapToGlobal(QPoint(5,ui->stackedWidget->geometry().top()+5));
        m_pFloatReturn->setGeometry(P3.x(),P3.y(),80*scale,32*scale);

        if(obj == ui->scrollArea->viewport() || m_pFloatLeft == obj || m_pFloatRight == obj)
        {
            if(e->type() == QEvent::Enter)
            {
                m_pTmHide->stop();
                m_pLangMenu->hide();
                if(m_layout->count()>=3)
                {
                    m_pFloatLeft->show();
                    m_pFloatRight->show();
                }
            }
        }

        if(obj == ui->scrollArea->viewport())
        {
            if(e->type() == QEvent::Leave)
            {
                m_pTmHide->start(50);
            }
        }
    }

    if(e->type() == QEvent::Paint)
    {
        if(m_pFloatLeft == obj)
        {
            QPainter p(m_pFloatLeft);
            p.setRenderHint(QPainter::Antialiasing, true);
            p.drawImage(m_pFloatLeft->rect(),QImage(":/images/a-left.png"));
        }

        if(m_pFloatRight == obj)
        {
            QPainter p(m_pFloatRight);
            p.setRenderHint(QPainter::Antialiasing, true);
            p.drawImage(m_pFloatRight->rect(),QImage(":/images/a-right.png"));
        }

        if(m_pFloatReturn == obj)
        {
            QPainter p(m_pFloatReturn);
            p.setRenderHint(QPainter::Antialiasing, true);
            QRect rect = m_pFloatReturn->rect();
            QRgb A = qRgba(255,255,255,100);
            p.setBrush(QColor::fromRgba(A));
            p.setPen(QColor::fromRgba(qRgba(100,100,100,100)));
            p.drawRoundedRect(rect,16,16);
            p.setRenderHint(QPainter::SmoothPixmapTransform, true);
            p.drawImage(rect.adjusted(24,5,-24,1),QImage(":/images/btn-return.png"));
        }
    }

    return QMainWindow::eventFilter(obj, e);
}

bool MainWindow::event(QEvent *event)
{
    if(event->type() == QEvent::Leave)
    {
        if(!geometry().contains(QCursor::pos()))
            m_pLangMenu->hide();
    }

    return QMainWindow::event(event);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setRenderHint(QPainter::Antialiasing, true);

    ui->labelLogo->hide();
    int borderRadius = 20;
    QPainterPath path;
    path.addRoundedRect(this->rect(), borderRadius, borderRadius);

    p.setClipPath(path);
    if (ui->stackedWidget->isHidden())
    {
        m_pFloatReturn->hide();
        p.drawImage(this->rect(), QImage("./images/MainPicture.png"));
        QFont font = this->font();
        //font.setBold(true);
        font.setPointSize(22);
        p.setFont(font);
        int adjust = this->height() - 90;
        p.drawText(this->rect().adjusted(0,adjust,0,0), Qt::AlignCenter, tr("正在搜索设备") + QString("..."));
    }
    else
    {
        p.fillRect(this->rect(), Qt::white);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        p.drawImage(QRect(30,25,136,40), QImage(":/images/AkkoFlag.png"));
    }

    p.setPen(Qt::blue);
    p.drawRoundedRect(this->rect(), borderRadius, borderRadius);

    QMainWindow::paintEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    auto res = QMessageBox::question(this,tr("提示"),tr("确定要退出 AKKO 驱动程序？"));
    if(res != QMessageBox::Yes)
    {
        return;
    }
    m_pFloatReturn->hide();

    ::PostMessage(s_hWndEmb0,WM_CLOSE,0,0);
    ::PostMessage(s_hWndEmb1,WM_CLOSE,0,0);

    qApp->exit();
    exit(0);
}

void MainWindow::on_pushButtonExit_clicked()
{
    this->close();
}

void MainWindow::on_pushButtonMin_clicked()
{
    this->showMinimized();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    m_pLangMenu->hide();
    QScrollBar *sb = ui->scrollArea->horizontalScrollBar();

    if(event->key() == Qt::Key_Left || event->key() == Qt::Key_Up)
    {
        sb->setValue(sb->value() - sb->width());
    }
    if(event->key() == Qt::Key_Right || event->key() == Qt::Key_Down)
    {
        sb->setValue(sb->value() + sb->width());
    }
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_pLangMenu->hide();
    if (event->button() == Qt::LeftButton)
    {
        if(QRect(30,25,136,40).contains(event->pos()))
            DialogDeviceConnect::instance()->show();
        if (event->pos().y() < 80)
        {
            m_dragPosition = event->globalPosition() - frameGeometry().topLeft();
            event->accept();
            m_dragging = true;
            return;
        }
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_dragging)
    {
        QPointF MP = event->globalPosition() - m_dragPosition;
        move(MP.toPoint());
        event->accept();

        QPoint P3 = mapToGlobal(QPoint(5,ui->stackedWidget->geometry().top()+5));
        m_pFloatReturn->setGeometry(P3.x(),P3.y(),80,32);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = false;
        event->accept();
    }
}
