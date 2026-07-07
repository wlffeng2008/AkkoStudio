#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "hidapi.h"

#include "ModuleLangMenu.h"
#include "FrameDeviceShow.h"
#include "AkkoDeviceBase.h"
#include "DialogDeviceConnect.h"
#include "framedeviceholder.h"
#include "ModuleGeneralMasker.h"
#include "FrameSystemInfo.h"
#include "AkkoDeviceEnum.h"

#include "Downloader.h"

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

#include <QJsonDocument>
#include <QJsonObject>

#include <QScreen>
#include <QApplication>

#include <QSharedMemory>
#include <QStandardPaths>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

static HWND s_hWndEmb[10]={nullptr};

#include<tlhelp32.h>
static bool killProcess(const QString&processName)
{
    if (processName.isEmpty())
        return false;

    HANDLE hRootSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hRootSnap)
        return false;

    bool bKill = false;
    PROCESSENTRY32 pEntry = {0};
    pEntry.dwSize = sizeof(PROCESSENTRY32);
    BOOL bProcess = ::Process32First(hRootSnap, &pEntry);
    while (bProcess)
    {
        QString strProcName = QString::fromWCharArray(pEntry.szExeFile);
        if (strProcName.compare(processName,Qt::CaseInsensitive) == 0)
        {
            HANDLE hKill = ::OpenProcess(PROCESS_TERMINATE, FALSE, pEntry.th32ProcessID);
            if (hKill)
            {
                bKill =::TerminateProcess(hKill, 0);
                break;
            }
        }
        bProcess = ::Process32Next(hRootSnap, &pEntry);
    }
    ::CloseHandle(hRootSnap);

    return bKill;
}

static bool isRunning(const QString&processName)
{
    if (processName.isEmpty())
        return false;

    HANDLE hRootSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hRootSnap)
        return false;

    bool bRunning = false;

    PROCESSENTRY32 pEntry = {0};
    pEntry.dwSize = sizeof(PROCESSENTRY32);
    BOOL bProcess = ::Process32First(hRootSnap, &pEntry);
    while (bProcess)
    {
        QString strProcName = QString::fromWCharArray(pEntry.szExeFile);
        if (strProcName.compare(processName,Qt::CaseInsensitive) == 0)
        {
            bRunning = true;
            break;
        }
        bProcess = ::Process32Next(hRootSnap, &pEntry);
    }
    ::CloseHandle(hRootSnap);

    return bRunning;
}

static void HideStartProcess(const QString&strExePath)
{
    ::WinExec(strExePath.toStdString().c_str(),SW_HIDE);
    return;

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {0};

    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL bRet = ::CreateProcess(
        (LPCWSTR)QString(strExePath).utf16(),
        NULL,
        NULL,
        NULL,
        FALSE,
        NORMAL_PRIORITY_CLASS,
        NULL,
        NULL,
        &si,
        &pi
        );

    if (bRet)
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

#include <WtsApi32.h>
#pragma comment(lib,"Wtsapi32.lib")

BOOL IsScreenLocked()
{
    WTS_INFO_CLASS wtsic = WTSSessionInfoEx;
    LPTSTR ppBuffer = nullptr;
    DWORD dwBytesReturned = 0;
    LONG sessionFlags = WTS_SESSIONSTATE_UNKNOWN;
    if (::WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, wtsic, &ppBuffer, &dwBytesReturned))
    {
        if (dwBytesReturned > 0)
        {
            WTSINFOEXW const* pInfo = (WTSINFOEXW*)ppBuffer;
            if (pInfo->Level == 1)
                sessionFlags = pInfo->Data.WTSInfoExLevel1.SessionFlags;
        }
        ::WTSFreeMemory(ppBuffer);
        ppBuffer = nullptr;
    }

    return (sessionFlags == WTS_SESSIONSTATE_LOCK);
}

DWORD GetSystemIdle()
{
    tagLASTINPUTINFO LastInput = { 0 };
    LastInput.cbSize = sizeof(tagLASTINPUTINFO);
    LastInput.dwTime = 0;

    DWORD nIdle = 0;
    if (::GetLastInputInfo(&LastInput))
        nIdle = (::GetTickCount() - LastInput.dwTime) / 1000;
    return nIdle;
}


static bool m_bForMGK = false;

QString getUserDataPath()
{
    QString strPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/AppData/Local/") + (m_bForMGK ? "MgkStudio" : "AkkoStudio");
    QDir DData(strPath);
    if(!DData.exists())
        DData.mkdir(strPath);
    return strPath;
}

static QSettings settings0("HKEY_CURRENT_USER\\Software\\Akko",QSettings::NativeFormat);
static QSettings settings1("HKEY_CURRENT_USER\\Software\\MonsGeek",QSettings::NativeFormat);

QSettings *getUserSetting()
{
    return (m_bForMGK ? &settings1 : &settings0);
}

// https://www.akkogear.com/akkoupdate.txt
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pSet = getUserSetting();

    {
        static QSharedMemory sharedMemory(m_bForMGK ? "MGKAudioApp_71A7F2D4-5566-4F99" : "AkkoAudioApp_71A7F2D4-5566-4F99");

        if (sharedMemory.attach())
        {
            hide();
            sharedMemory.lock();
            memset(sharedMemory.data(), 0, 1024);
            memcpy(sharedMemory.data(), "12345678", 8);
            sharedMemory.unlock();
            exit(0);
            return;
        }
        sharedMemory.create(1024);

        static QTimer *pTMCheckSM = new QTimer(this);
        connect(pTMCheckSM,&QTimer::timeout,this,[=]{

            sharedMemory.lock();
            QString signalFlag = QString::fromLocal8Bit((char*)sharedMemory.data()).trimmed();
            sharedMemory.unlock();
            if(signalFlag == "12345678")
            {
                showNormal();
                raise();
                activateWindow();
                setFocus();

                sharedMemory.lock();
                memset(sharedMemory.data(), 0, 1024);
                sharedMemory.unlock();
            };
        });
        pTMCheckSM->start(200);
    }

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle(m_bForMGK ? "MonsGeek Dirver Hub" : "Akko Dirver Hub");

    setStyleSheet("QMainWindow{ background-color: rgba(255, 255, 255, 1); border: 1px solid skyblue; border-radius: 20px; }");

    {
        QString strPath = QApplication::applicationDirPath() + "/images/";
        QDir d(strPath);

        QDir::Filters filters = QDir::Files | QDir::NoDotAndDotDot | QDir::Readable;
        QStringList nameFilters;
        // nameFilters << "*.txt" << "*.cpp";
        QStringList fileList = d.entryList(nameFilters, filters);
        for(const QString &strOld:std::as_const(fileList))
        {
            QString strNew = strOld;
            if(strOld.contains(" "))
            {
                strNew.replace(" ","-");
                QFile::copy(strPath + strOld, strPath + strNew);
            }
        }
    }

    ui->stackedWidget->setHidden(true);
    ui->labelPrev->setHidden(true);
    ui->labelNext->setHidden(true);
    m_layout = ui->scrollAreaWidgetContents->layout();
    m_layout->setSpacing(20);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_pLangMenu = new ModuleLangMenu(this);
    connect(m_pLangMenu, &ModuleLangMenu::onLangChanged, this, [=](int langId, const QString &lang){
        m_langName = QString(" ") + lang;
        m_pSet->setValue("lastlang",langId);

        {
            QStringList langs = {"zh_CN","en_US","zh_TW","ja_JP","ko_KR","ru_RU","vi_VN","pt_PT","th_TH","de_DE","fr_FR","sv_SE","it_IT","tr_TR"};
            QString baseName = langs[langId] + ".qm";
            if(m_pMainTrM->load(QString(":/i18n/AKKOStudio_")+baseName)){}
            if(m_pMainTrA->load("qtbase_" + baseName, QLibraryInfo::path(QLibraryInfo::TranslationsPath))){}
            if(m_pMainTrB->load("qt_"     + baseName, QLibraryInfo::path(QLibraryInfo::TranslationsPath))){}

            QString strSet = langs[langId].replace('_','-');
            if(m_creator == 2)
                m_pSet->setValue("ByLocale",strSet);
            m_langSet = strSet;
        }

        {
            QStringList langs = {"cn","en","tw","jp","kr","ru","vi","pt","th","de","fr","sv","it","tr"};
            m_pSet->setValue("LanguageCode",langs[langId]);
        }

        //     语言/区域	语言/区域标记	语言/区域标识符	语言/区域十进制标识符
        //     阿拉伯语(沙特阿拉伯)	ar-SA	0x0401	1025
        //     巴斯克语(巴斯克语)	eu-ES	0x042d	1069
        //     保加利亚语(保加利亚)	bg-BG	0x0402	1026
        //     加泰罗尼亚语     	ca-ES	0x0403	1027
        //     中文(简体，中国)    	zh-CN	0x0804	2052
        //     中文(繁体，台湾 )	zh-TW	0x0404	1028
        //     克罗地亚语(克罗地亚)	hr-HR	0x041a	1050
        //     捷克语(捷克共和国)	cs-CZ	0x0405	1029
        //     丹麦语(丹麦)      	da-DK	0x0406	1030
        //     荷兰语(荷兰)      	nl-NL	0x0413	1043
        //     英语(美国)       	en-US	0x0409	2052
        //     英语(英国)       	en-GB	0x0809	2057
        //     爱沙尼亚语(爱沙尼亚)	et-EE	0x0425	1061
        //     芬兰语(芬兰)      	fi-FI	0x040b	1035
        //     法语(加拿大)      	fr-CA	0x0c0c	3084
        //     法语(法国)       	fr-FR	0x040c	1036
        //     加利西亚语        	gl-ES	0x0456	1110
        //     德语(德国)       	de-DE	0x0407	1031
        //     希腊语(希腊)      	el-GR	0x0408	1032
        //     希伯来语(以色列)     he-IL	0x040d	1037
        //     匈牙利语(匈牙利)    	hu-HU	0x040e	1038
        //     印度尼西亚语       	id-ID	0x0421	1057
        //     意大利语(意大利)    	it-IT	0x0410	1040
        //     日语(日本)       	ja-JP	0x0411	1041
        //     韩语(韩国)       	ko-KR	0x0412	1042
        //     拉脱维亚语(拉脱维亚)	lv-LV	0x0426	1062
        //     立陶宛语(立陶宛)    	lt-LT	0x0427	1063
        //     挪威语(博克马尔语)	nb-NO	0x0414	1044
        //     波兰语(波兰)      	pl-PL	0x0415	1045
        //     葡萄牙语（巴西  ）	pt-BR	0x0416	1046
        //     葡萄牙语(葡萄牙)    	pt-PT	0x0816	2070
        //     罗马尼亚语(罗马尼亚)	ro-RO	0x0418	1048
        //     俄语(俄罗斯)      	ru-RU	0x0419	1049
        //     塞尔维亚语(拉丁语)	sr-Latn-RS	0x241A	9242
        //     斯洛伐克语(斯洛伐克)	sk-SK	0x041b	1051
        //     斯洛文尼亚语         sl-SI	0x0424	1060
        //     西班牙语(墨西哥)    	es-MX	0x080a	2058
        //     西班牙语(西班牙)    	es-ES	0x0c0a	3082
        //     瑞典语(瑞典)      	sv-SE	0x041d	1053
        //     泰语(泰国)       	th-TH	0x041e	1054
        //     土耳其语（土耳其）	tr-TR	0x041f	1055
        //     乌克兰语(乌克兰)     uk-UA	0x0422	1058
        //     越南语               vi-VN	0x042a	1066
        {
            int id = langId;
            QList<int> langs = {2052,1033,1028,1041,1042,1049,1066,1046,1054,1031,1036,1053,1040,1055};
            m_pSet->setValue("LANGUAGE",langs[id]);
        }
    });

    connect(ui->pushButtonLang, &QPushButton::clicked, this, [=] {
        ModuleLangMenu *pLangMenu = m_pLangMenu;
        if (!pLangMenu->isHidden())
            return;

        QRect btnRect = ui->pushButtonLang->parentWidget()->geometry();
        QPoint PT = mapToGlobal(btnRect.bottomLeft());
        pLangMenu->setGeometry(PT.x() + 10, PT.y(), 110, 280);
        pLangMenu->show();
    });

    static ModuleGeneralMasker gMask(nullptr);
    gMask.setStyleSheet("QDialog { background-color: rgba(220, 220, 220, 0.96); border: none; border-bottom-left-radius: 20px; border-bottom-right-radius: 20px;}");
    gMask.setMinimumSize(QSize(1280,800));
    //gMask.setParent(this);
    static FrameSystemInfo *pSetInfo = new FrameSystemInfo();

    connect(ui->pushButtonSet, &QPushButton::clicked, this, [=] {
        QTimer::singleShot(100,this,[=]{
            gMask.setContent(pSetInfo,ui->stackedWidget);
            gMask.raise();
            gMask.exec();
        });
    });

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
    m_Enum = new AkkoDeviceEnum(this);
    m_Enum->m_Root = this;

    m_pFloatLeft->setWindowFlags(  m_pFloatLeft->windowFlags()   | Qt::FramelessWindowHint | Qt::Tool);
    m_pFloatRight->setWindowFlags(m_pFloatRight->windowFlags()   | Qt::FramelessWindowHint | Qt::Tool);
    m_pFloatReturn->setWindowFlags(m_pFloatReturn->windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
    m_pFloatLeft->setAttribute(Qt::WA_TranslucentBackground);
    m_pFloatRight->setAttribute(Qt::WA_TranslucentBackground);
    m_pFloatReturn->setAttribute(Qt::WA_TranslucentBackground);
    m_pFloatLeft->setStyleSheet( "background-color: rgba(0, 255, 0, 0.8); border: 1px solid skyblue; border-radius: 30px; ");
    m_pFloatRight->setStyleSheet("background-color: rgba(0, 0, 255, 0.8); border: 1px solid skyblue; border-radius: 30px; ");
    m_pFloatLeft->setCursor(Qt::PointingHandCursor);
    m_pFloatRight->setCursor(Qt::PointingHandCursor);
    m_pFloatReturn->setCursor(Qt::PointingHandCursor);

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

    static USBNotifier *pUsb = new USBNotifier(this);
    QCoreApplication::instance()->installNativeEventFilter(pUsb);
    connect(pUsb, &USBNotifier::devicePluggined,this,[=](bool in) {
        Q_UNUSED(in)
        m_Enum->DoEnum();
    });

    //qDebug()<< QProcess::systemEnvironment();

    QString strRoot = QApplication::applicationDirPath();
    QDir EDir(strRoot + "/RyExe");
    if(!EDir.exists()) strRoot += "/..";
    QFile::rename(strRoot + "/WsExe/Akko.exe",strRoot + "/WsExe/Akko-WS.exe");

    QString strExe0 = strRoot + "/RyExe/AkkoCloudDriver.exe";
    QString strExe1 = strRoot + "/WsExe/Akko-WS.exe";
    QString strExe2 = strRoot + "/ByExe/Akko-BY.exe";
    QString strExe3 = strRoot + "/JmExe/Akko-Gaming-Bub.exe";
    if(m_bForMGK) strExe0 = strRoot + "/MgExe/MonsGeekDriver.exe";

    m_strRyExe = strExe0;
    m_strRyName = (m_bForMGK ? "MonsGeekDriver.exe" : "AkkoCloudDriver.exe");

    m_strWsExe  = strExe1;
    m_strWsName = "Akko-WS.exe";
    m_strByExe  = strExe2;
    m_strByName = "Akko-BY.exe";
    m_strJmExe  = strExe3;
    m_strJmName = "Akko-Gaming-Bub.exe";

    HWND hParentWnd = (HWND)ui->frameEmb->winId();
    QTimer *pTMFindWnd = new QTimer(this);
    pTMFindWnd->start(200);

    connect(pTMFindWnd,&QTimer::timeout,this,[=]{

        if(this->isVisible())
        {
            if(s_hWndEmb[1]){ ::ShowWindow(s_hWndEmb[1],SW_HIDE); }
            if(s_hWndEmb[3]){ ::ShowWindow(s_hWndEmb[3],SW_HIDE); }
        }

        if(!isRunning(m_strWsName) && s_hWndEmb[1])
        {
            m_pSet->setValue("AkkoWnd", 0);
            s_hWndEmb[1] = nullptr;
            HideStartProcess(m_strWsExe);
        }

        if(!isRunning(m_strByName) && s_hWndEmb[2])
        {
            s_hWndEmb[2] = nullptr;
            HideStartProcess(m_strByExe);
        }

        if(!isRunning(m_strJmName) && s_hWndEmb[3])
        {
            s_hWndEmb[3] = nullptr;
            HideStartProcess(m_strJmExe);
        }

        if(!s_hWndEmb[0])
        {
            HWND hWnd = ::FindWindow(nullptr, (LPCWSTR)QString(m_bForMGK ? "MonsGeek Driver" : "Akko Cloud Driver").utf16());
            if(hWnd)
            {
                s_hWndEmb[0] = hWnd;
                qDebug() << "Found RY ---------------";
                ::SetWindowLongPtr(hWnd, GWL_STYLE, 0x960a0000|WS_CHILD);
                ::SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0x80000);
                ::SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE);
                ::SetParent(hWnd,hParentWnd);
                m_pSet->setValue("ParentHwnd", (quint32)hWnd);
            }
        }

        if(!s_hWndEmb[1])
        {
            HWND hWnd = (HWND)m_pSet->value("AkkoWnd").toUInt();
            if(!hWnd) hWnd = ::FindWindow(nullptr, (LPCWSTR)QString("Akko").utf16());
            if(hWnd)
            {
                s_hWndEmb[1] = hWnd;
                qDebug() << "Found WS ---------------";
                ::ShowWindow(hWnd,SW_HIDE);
            }
        }

        if(!s_hWndEmb[2])
        {
            HWND hWnd = ::FindWindow(nullptr, (LPCWSTR)QString("bytech").utf16());
            if(hWnd)
            {
                qDebug() << "Found BY ---------------";
                s_hWndEmb[2] = hWnd;
                ::SetWindowLongPtr(hWnd, GWL_STYLE, 0x960a0000|WS_CHILD);
                ::SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0x80000);
                ::SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE);
                ::ShowWindow(hWnd,SW_HIDE);
                ::SetParent(hWnd,hParentWnd);
            }
        }

        if(!s_hWndEmb[3])
        {
            HWND hWnd = ::FindWindow(nullptr, (LPCWSTR)QString("Akko-Gaming-Bub").utf16());
            if(hWnd)
            {
                qDebug() << "Found JM ---------------";
                s_hWndEmb[3] = hWnd;
                ::ShowWindow(hWnd,SW_HIDE);
            }
        }
    });

    {
        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setIcon(QIcon(":/images/logo.png"));
        trayIcon->setToolTip(m_bForMGK ? "MonsGeek Driver" : "AKKO Cloud Driver");
        trayIcon->show();

        connect(trayIcon,&QSystemTrayIcon::activated,this,[=](QSystemTrayIcon::ActivationReason reason){
            if(reason != QSystemTrayIcon::Context)
            {
                if(m_creator == 1 || m_creator == 3)
                  return;

                qDebug() <<"QSystemTrayIcon::activated";
                if(this->isHidden() || this->isMinimized())
                {
                    this->raise();
                    this->showNormal();
                    this->setFocus();
                    this->activateWindow();
                }
                else
                {
                    this->hide();
                }
            }
        });

        QAction *showAction = new QAction("", this);
        QAction *hideAction = new QAction("", this);
        QAction *exitAction = new QAction("", this);

        m_act0 = showAction;
        m_act1 = hideAction;
        m_act2 = exitAction;

        QMenu *trayMenu = new QMenu(this);
        trayMenu->addAction(showAction);
        trayMenu->addAction(hideAction);
        trayMenu->addSeparator();
        trayMenu->addAction(exitAction);

        connect(exitAction, &QAction::triggered, this, &QApplication::quit);
        connect(showAction, &QAction::triggered, this, [=]{
            if(m_creator == 1 || m_creator == 3)
                return;

            this->raise();
            this->showNormal();
            this->setFocus();
            this->activateWindow();
        });
        connect(hideAction, &QAction::triggered, this, [=]{
            this->hide();
            m_bManHide=true;
        });
        trayIcon->setContextMenu(trayMenu);

        updateDeviceInfo();
    }

    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->frameHold,&FrameDeviceHolder::onReturn,this,[=]{
        ui->stackedWidget->setCurrentIndex(0);
    });

    QTimer::singleShot(10,this,[=]{ m_pLangMenu->setLanguage(m_pSet->value("lastlang").toInt()); });

    DialogDeviceConnect *pCnn = DialogDeviceConnect::instance();
    connect(pCnn,&DialogDeviceConnect::onUpdataLayer,this,[=](int layer){
        ui->frameHold->updateLayer(layer);
    });

    connect(pCnn,&DialogDeviceConnect::onReadAll,this,[=]{
        m_bReadAll=true;
    });
    connect(pCnn,&DialogDeviceConnect::onReadDone,this,[=]{
        m_bReadAll=false;
    });
    connect(pCnn,&DialogDeviceConnect::onKeyTesting,this,[=](bool testing){
        m_KeyTesting = testing;
    });

    {
        m_pSet->setValue("AkkoReturn", 0);
        m_pSet->setValue("MonsGeekReturn", 0);
        m_pSet->setValue("AkkoWnd", 0);
        // m_pSet->setValue("AkkoDeviceIndex", 0xFF);
        m_pSet->setValue("iotManagerInitialized",false);
        m_pSet->setValue("ShowWindowControlButtons",false);
        m_pSet->setValue("DeviceId",0);
        m_pSet->setValue("DevicePath","");
        m_pSet->setValue("PageLoaded",false);
        m_pSet->setValue("VendorDevicePath","");
        m_pSet->setValue("ByDeviceUuid","");

        connect(ui->pushButtonScan, &QPushButton::clicked, this, [=] {
            setHubSize(true);
            m_pSet->setValue("ByDeviceUuid","");
            m_pFloatReturn->hide();
            ui->stackedWidget->setCurrentIndex(0);
            m_tmp.clear();
            while(m_layout->count())
            {
                QWidget *item = m_layout->takeAt(0)->widget();
                if(!item) break;
                m_layout->removeWidget(item);
                item->hide();
            }

            killProcess(m_strWsName);
            killProcess(m_strByName);

            m_Enum->DoEnum();
        });

        QTimer *pTMEnum = new QTimer(this);
        pTMEnum->start(500);
        connect(pTMEnum,&QTimer::timeout,this,[=]{
            pTMEnum->stop();

            int interval = 60000;
            if(ui->stackedWidget->currentIndex() == 0)
                interval = 30000;
            pTMEnum->start(interval);

            m_Enum->DoEnum();
        });

        connect(this,&MainWindow::oEnumDeiceDone,this,[=]{
            int index = 0;
            int count = m_tmp.count();
            for(int i=0; i<count; i++)
            {
                if(m_tmp[i]->toShow)
                    addToHub(m_tmp[i],index++);
            }
        },Qt::QueuedConnection);

        QTimer *pTMRet = new QTimer(this);
        pTMRet->start(100);
        connect(pTMRet,&QTimer::timeout,this,[=]{
            if((m_pSet->value("AkkoReturn").toInt() || m_pSet->value("MonsGeekReturn").toInt()) && m_bCanReturn)
            {
                qDebug() << "Action AkkoReturn";
                m_pSet->setValue("AkkoReturn", 0);
                m_pSet->setValue("MonsGeekReturn", 0);
                ui->stackedWidget->setCurrentIndex(0);
                m_pFloatReturn->hide();
                for(int i=0; i<10; i++)
                {
                    if(s_hWndEmb[i])
                    {
                        ::ShowWindow(s_hWndEmb[i], SW_HIDE);
                        if(i == 1 || i == 3)
                            continue;
                        ::SetWindowPos(s_hWndEmb[i], HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_HIDEWINDOW);
                    }
                }

                m_creator = -1;
                m_showId  = 0;
                m_showPath.clear();
                m_hCurHwnd = nullptr;
                m_pActDev = nullptr;

                QTimer::singleShot(200,this,[=]{
                    this->show();
                    this->raise();
                    setHubSize(true);
                });
            }
        });

        QTimer *pTMIdle = new QTimer(this);
        pTMIdle->start(5000);
        connect(pTMIdle,&QTimer::timeout,this,[=]{
            if(IsScreenLocked() && GetSystemIdle() > 300)
            {
                m_pSet->setValue("AkkoReturn", 1);
            }
        });

        QTimer *pTMEsc = new QTimer(this);
        pTMEsc->start(30);
        connect(pTMEsc,&QTimer::timeout,this,[=]{
            if((::GetKeyState(VK_ESCAPE)&0x8000) != 0 && !m_KeyTesting /*&& m_bActive && isVisible() && !isMinimized()*/)
            {
                m_pSet->setValue("AkkoReturn", 1);
            }
        });
    }

    QTimer::singleShot(5000,this,[=]{

        static Downloader *pChecker = new Downloader(this);
        static Downloader *pRWorker = new Downloader(this);
        static QTimer *pTMCheck = new QTimer(this);

        static QString strVer;
        static QString strUrl;
        static QString strFile;

        pTMCheck->start(200);
        connect(pTMCheck,&QTimer::timeout,this,[=]{
            pTMCheck->stop();
            pTMCheck->start(10000);
            QString strFile = QApplication::applicationDirPath() + "/config/updateset.ini";
            QSettings Set(strFile,QSettings::IniFormat);
            QString strCheck = Set.value("updateurl","https://www.akkogear.com/akkoupdate.txt").toString();
            pChecker->startDownload(strCheck,"");
        });

        connect(pChecker,&Downloader::dataIn,this,[=](const QByteArray&data){
            QJsonDocument jDoc = QJsonDocument::fromJson(data);
            if(jDoc.isObject())
            {
                QJsonObject jObj = jDoc.object();
                //qDebug() << jObj;

                strVer = jObj["vresion"].toString();
                strUrl = jObj["url"].toString();
                if(strVer >= ::currentVersion())
                {
                    pTMCheck->stop();

                    strFile = QApplication::applicationDirPath() + "/updatepack";
                    QDir UP(strFile);
                    if(!UP.exists()) UP.mkpath(strFile);
                    strFile += QString("/Updater-V%1.exe").arg(strVer);

                    pRWorker->startDownload(strUrl,strFile);
                }
            }
        });

        connect(pRWorker,&Downloader::finished,this,[=](bool success, const QString &msg){
            if(success)
            {
                if(QMessageBox::question(this,tr("提示"),tr("发现新版本，现在要立即更新吗？") + QString("  (V%1)").arg(strVer)) == QMessageBox::Yes)
                {
                    QProcess::startDetached(strFile,QStringList{"/VERYSILENT","/SP-", "/SUPPRESSMSGBOXES"});
                    trayIcon->hide();
                    exit(0);
                }
                else
                {
                    pTMCheck->start(3600000);
                }
            }
        });
    });

    setHubSize(true);
}

void MainWindow::addToHub(DeviceEnumInfo *pDevInfo, int index)
{
    FrameDeviceShow *pFrmDS = FrameDeviceShow::getFrameShow(index, this);
    pFrmDS->m_sa = ui->scrollArea;
    pFrmDS->setDevieInfo(pDevInfo);
    pFrmDS->show();

    if(m_layout->indexOf(pFrmDS) < 0)
        m_layout->addWidget(pFrmDS);

    updateDeviceInfo();

    if(pFrmDS->m_bConacted)
        return;

    pFrmDS->m_bConacted = true;
    connect(pFrmDS,&FrameDeviceShow::onReport,this,[=](DeviceEnumInfo *dev,const QString&battImg,const QString&typeImg,const QString&tip,const QString&qss){
        ui->frameHold->updateBattery(dev,battImg,typeImg,tip,qss);
    });

    connect(pFrmDS,&FrameDeviceShow::onClicked,this,[=](DeviceEnumInfo *dev, const QString&image){
        m_creator = dev->creator;
        m_showId  = dev->driverId;
        m_showPath = dev->strPath2;
        m_pLangMenu->hide();
        m_hCurHwnd = nullptr;
        m_bCanReturn = false;

        for(int i=0; i<10; i++)
        {
            if(s_hWndEmb[i])
            {
                ::ShowWindow(s_hWndEmb[i], SW_HIDE);
                if( i == 1 || i == 3)
                    continue;
                ::SetWindowPos(s_hWndEmb[i], HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_HIDEWINDOW);
            }
        }

        if(m_creator == 0)
        {
            QList<quint16>IdList={2807,3779}; //,2743,3131,3800
            if(IdList.contains(dev->driverId) || dev->strName.contains("5075"))
            {
                bool bleMode = (dev->connectType == 2);
                DialogDeviceConnect::instance()->DoConnectDevice(dev->VID,dev->PID,bleMode,dev->strPath1,dev->strPath2);
                ui->stackedWidget->setCurrentIndex(2);
                ui->frameHold->setDevice(dev,image,dev->strName);
                m_pActDev = dev;
                m_bCanReturn = true;
                return;
            }
            else
            {
                //if(!m_pSet->value("DeviceLoaded").toBool())
                //    return;

                QString strlastPath = m_pSet->value("DevicePath").toString();
                if(strlastPath != dev->strPath2)
                {
                    m_pSet->setValue("DeviceId",0);
                    m_pSet->setValue("DevicePath","");
                    m_pSet->setValue("PageLoaded","false");
                    m_pSet->setValue("VendorDevicePath","");

                    QTimer::singleShot(100,this,[=]{
                        m_pSet->setValue("DeviceId",dev->driverId);
                        m_pSet->setValue("VendorDevicePath",dev->strPath1);
                        m_pSet->setValue("DevicePath",dev->strPath2);
                    });
                }
                setHubSize(false);
            }
        }
        else
        {
            if(m_creator == 1)
            {
                m_pSet->setValue("AkkoDeviceIndex",(quint32)dev->driverId);
            }

            if(m_creator == 2)
            {
                QString strSet = m_langSet;
                if(!strSet.isEmpty())
                    m_pSet->setValue("ByLocale",strSet);
                m_pSet->setValue("ByDeviceUuid",QString("0x") + dev->strPath1);
            }

            if(m_creator == 3)
            {
                QString strSet = m_langSet;
                if(!strSet.isEmpty())
                    m_pSet->setValue("JmLocale",strSet);
                m_pSet->setValue("JmDevicePath",dev->strPath2);
            }
        }

        QTimer::singleShot(100,this,[=]{
            HWND hWnd = s_hWndEmb[m_creator];
            if(!hWnd) return;
            m_pActDev = dev;

            qreal scaleFactor = this->devicePixelRatio();
            if(m_creator == 1 || m_creator == 3)
            {
                RECT rc;
                ::GetWindowRect(hWnd,&rc);

                QSize cs = QApplication::screens().at(0)->size();

                int x = (cs.width()  * scaleFactor - (rc.right-rc.left))/2;
                int y = (cs.height() * scaleFactor - (rc.bottom-rc.top))/2;

                if(x < 0 || x > cs.width())   x = 0;
                if(y < 0 || y > cs.height())  y = 0;

                hide();

                ::BringWindowToTop(hWnd);
                ::ShowWindow(hWnd,SW_SHOW);
                ::SetForegroundWindow(hWnd);
                //::MoveWindow(hWnd,x,y,(rc.right-rc.left),(rc.bottom-rc.top),TRUE);

                QTimer::singleShot(1000,this,[=]{

                    ::BringWindowToTop(hWnd);
                    ::ShowWindow(hWnd,SW_SHOW);
                    ::SetForegroundWindow(hWnd);
                    ::RedrawWindow(hWnd,NULL,NULL,RDW_ERASENOW|RDW_UPDATENOW);

                    m_hCurHwnd = hWnd;
                });
            }
            else
            {
                ui->stackedWidget->setCurrentIndex(1);
                ui->frameEmb->setVisible(true);

                int nWidth  = ui->frameEmb->width() *scaleFactor+1;
                int nHeight = ui->frameEmb->height()*scaleFactor+1;
                ::SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, nWidth, nHeight, SWP_SHOWWINDOW);

                int radius = 32;

                HRGN hFullRound = ::CreateRoundRectRgn(0, 0, nWidth, nHeight, radius, radius);
                HRGN hTopRect0 = ::CreateRectRgn(0, 0, nWidth, nHeight);
                HRGN hTopRect1 = ::CreateRectRgn(0, 0, nWidth, radius);

                HRGN hFinalRgn = CreateRectRgn(0, 0, 0, 0);
                ::CombineRgn(hFinalRgn, hFullRound, hTopRect0, RGN_AND);
                ::CombineRgn(hFinalRgn, hFinalRgn, hTopRect1, RGN_OR);

                ::SetWindowRgn(hWnd, hFinalRgn, TRUE);

                ::DeleteObject(hFullRound);
                ::DeleteObject(hTopRect0);
                ::DeleteObject(hTopRect1);
            }

            m_bActive = true;

            QTimer::singleShot(2000,this,[=]{
                m_bCanReturn = true;
                m_pSet->setValue("AkkoReturn", 0);
                m_pSet->setValue("MonsGeekReturn", 0);
                m_pFloatReturn->setHidden(m_creator == 1 || m_creator == 3 || ui->stackedWidget->currentIndex() != 1);
            });
        });
    });
}

void MainWindow::addDevice(quint16 VID, quint16 PID, quint64 driverId, const QString &path1, const QString &path2, int connectType, int creator)
{
    quint16 connect = connectType;
    QString strName ;
    quint16 deviceType = -1;
    if(creator == 0)
    {
        if(driverId == 0 || driverId > 0x10000)
            return;
        strName = getDisplayName(driverId, deviceType, m_bForMGK ? 1 : 0);
    }
    else
    {
        driverId = getExtrasDevice(VID,PID,driverId,strName,deviceType,connect);
    }

    if(strName.isEmpty())
        return;

    int count = m_tmp.count();
    for(int i=0; i<count; i++)
    {
        if(m_tmp[i]->strPath2 == path2)
        {
            m_tmp[i]->driverId = driverId;
            m_tmp[i]->toShow = true;
            m_tmp[i]->lastTime = time(nullptr);
            return;
        }
    }

    DeviceEnumInfo *pDev = new DeviceEnumInfo();
    pDev->driverId = driverId;
    pDev->creator  = creator;
    pDev->connectType = connect;
    pDev->deeviceType = deviceType;
    pDev->strName = strName;
    pDev->strPath1 = path1;
    pDev->strPath2 = path2;
    pDev->toShow = true;
    pDev->hShowWnd = nullptr;
    pDev->lastTime = time(nullptr);
    pDev->VID = VID;
    pDev->PID = PID;

    m_tmp.append(pDev);

    switch (creator)
    {
    case 0:
        if(!isRunning(m_strRyName))
        {
            ::HideStartProcess(m_strRyExe);
            QThread::msleep(100);
        }
        break;
    case 1:
        if(!isRunning(m_strWsName))
        {
            ::HideStartProcess(m_strWsExe);
            QThread::msleep(100);
        }
        break;
    case 2:
        if(!isRunning(m_strByName))
        {
            ::HideStartProcess(m_strByExe);
            QThread::msleep(100);
        }
        break;
    case 3:
        if(!isRunning(m_strJmName))
        {
            ::HideStartProcess(m_strJmExe);
            QThread::msleep(100);
        }
        break;
    default:
        break;
    }
}

static void stringToInt(const QString&value,QList<quint64>&retList)
{
    retList.clear();
    QStringList vals = value.split(',');
    int count = vals.count();
    for(int i=0; i<count; i++)
    {
        retList.push_back(getIntVal(vals[i]));
    }
}

void MainWindow::enumDevice()
{
    if(m_bEnuming) return;
    if(m_bReadAll) return;

    if((isMinimized() || isHidden()) && !m_hCurHwnd) return;

    m_bEnuming = true;
    QStringList allPaths;

    quint16 VID = 0;
    quint16 PID = 0;
    quint16 UPG = 0;
    quint16 USA = 0;
    char *PATH = nullptr;
    QSettings enumSet(QApplication::applicationDirPath() + "/config/enumsetting.ini",QSettings::IniFormat);

    QString path1,path2;
    QList<quint64> VidList;// = {0x3151, 0x38EE, 0x25A7, 0x05AC, 0x0461};//

    stringToInt(enumSet.value("VIDset0","0x3151, 0x38EE, 0x25A7, 0x05AC, 0x0461").toString(),VidList);

    foreach (quint16 Vid, VidList)
    {
        hid_device_info *pRoot = hid_enumerate(Vid, 0);
        hid_device_info *pEDev = pRoot;
        while (pEDev)
        {
            VID = pEDev->vendor_id;
            PID = pEDev->product_id;
            UPG = pEDev->usage_page;
            USA = pEDev->usage;
            PATH= pEDev->path;

            //if((PID < 0x0B || PID > 0x5B || PID == 0x0000) && !(PID == 0x5151 || PID == 0x5152 || PID == 0x22B4 || PID == 0x22B5 ))
            {
                //qDebug().noquote() << QString::asprintf("VID=0x%04X PID=0x%04X usage_page=0x%04X usage=0x%04X",VID,PID,UPG,USA);

                if(UPG == 0xFF55 && USA == 0x0202) // BLE
                {
                    path2 = PATH;
                    allPaths.push_back(path2);
                    hid_device *pDev = hid_open_path(PATH);
                    if (pDev)
                    {
                        quint8 cmd[120]={0};
                        cmd[0] = 0x06;
                        cmd[1] = 0x55;
                        cmd[2] = 0x8F;
                        cmd[9] = 0xFF - cmd[2];

                        int nlen = 0;
                        int ntry = 0;
                        quint8 buf[128] = {0};

                        hid_set_nonblocking(pDev,0);
                        while(ntry++ < 5)
                        {
                            hid_write(pDev, cmd, 66);
                            nlen = hid_read_timeout(pDev, (quint8 *)buf, 66, 200);
                            if(buf[2] == 0x8F || buf[2] == 0x88) break;
                            QThread::msleep(100);
                        }

                        QByteArray data((const char *)buf+2, nlen);

                        quint32 id = *(quint32 *)(buf + 3);
                        if(id)
                        {
                            addDevice(VID,PID,id,"null",path2,2,0);
                            //qDebug().noquote() << "BLE_:" << data.left(16).toHex(' ').toUpper() << QString::asprintf("id:%04d VID:0x%04X,PID:0x%04X",id,VID,PID);
                        }
                    }
                }
                else
                {
                    if(UPG == 0xFFFF)
                    {
                        //if(USA == 1) path1 = pEDev->path;
                        if(USA == 2)
                        {
                            path1 = pEDev->path;
                            path2 = pEDev->path;
                        }
                    }

                    if(UPG == 0xFF01 && USA == 1 && VID == 0x3151 && PID == 0x504A)
                    {
                        path1 = "HitScreen";
                        path2 = pEDev->path;
                    }

                    if(!path1.isEmpty() && !path2.isEmpty())
                    {
                        allPaths.push_back(path2);
                        hid_device *pDev = hid_open_path(path2.toStdString().c_str());
                        if (pDev)
                        {
                            quint8 cmd[120]={0};
                            cmd[1] = 0x8F;
                            cmd[8] = 0xFF - cmd[1];

                            int nlen = 0;
                            quint8 buf[128] = {0};
                            int nTryCount = 0;
                            quint32 devId = 0;
                            int connectType = 0;
                            QByteArray data;

                            do
                            {
                                hid_send_feature_report(pDev, cmd, 65);
                                QThread::msleep(150);
                                nlen = hid_get_feature_report(pDev, buf, 65);
                                if(buf[1] == 0x8F)
                                {
                                    if(nlen>0)
                                    {
                                        data = QByteArray((char *)buf + 1, nlen - 1);
                                        devId = *(quint32 *)(data.data() + 1);
                                    }
                                    if(nTryCount++ >= 5) break;
                                    if(devId > 0) break;
                                }

                                QThread::msleep(50);
                            }while(true);

                            if(devId == 0 || devId > 0x1000)
                            {
                               // qDebug() << "Find 2.4G Device" ;
                                devId = 0;
                                quint8 tmp[120]={0};
                                tmp[1] = 0xf6;
                                tmp[2] = 0x0A;
                                tmp[8] = 0xFF - tmp[1] - tmp[2];
                                hid_send_feature_report(pDev, tmp, 65);
                                QThread::msleep(100);
                                nlen = hid_get_feature_report(pDev, buf, 65);
                                Q_UNUSED(nlen)

                                int nTry = 0;
                                while(nTry++ < 5)
                                {
                                    tmp[1] = 0xf7;
                                    tmp[2] = 0x00;
                                    tmp[8] = 0xFF - tmp[1] - tmp[2];
                                    hid_send_feature_report(pDev, tmp, 65);
                                    QThread::msleep(100);
                                    nlen = hid_get_feature_report(pDev, buf, 65);
                                    if(buf[6] == 1) break;
                                    QThread::msleep(50);
                                    Q_UNUSED(nlen)
                                }

                                tmp[1] = 0x8F;
                                tmp[2] = 0x00;
                                tmp[8] = 0xFF - tmp[1] - tmp[2];
                                hid_send_feature_report(pDev, tmp, 65);
                                QThread::msleep(100);
                                nlen = hid_get_feature_report(pDev, buf, 65);
                                Q_UNUSED(nlen)

                                nTry = 0;
                                while(nTry++ < 5)
                                {
                                    tmp[1] = 0xf7;
                                    tmp[2] = 0x00;
                                    tmp[8] = 0xFF - tmp[1] - tmp[2];
                                    hid_send_feature_report(pDev, tmp, 65);
                                    QThread::msleep(100);
                                    nlen = hid_get_feature_report(pDev, buf, 65);
                                    if(buf[1] == 0) break;
                                    QThread::msleep(50);
                                    Q_UNUSED(nlen)
                                }

                                tmp[1] = 0xfc;
                                tmp[2] = 0x00;
                                tmp[8] = 0xFF - tmp[1] - tmp[2];
                                hid_send_feature_report(pDev, tmp, 65);
                                QThread::msleep(100);
                                nlen = hid_get_feature_report(pDev, buf, 65);

                                if (nlen > 0)
                                {
                                    connectType = 1;
                                    data  = QByteArray((char *)buf + 1, nlen - 1);
                                    devId = *(quint32 *)(data.data() + 1);
                                }
                            }

                            //qDebug().noquote() << "get_:" << data.left(16).toHex(' ').toUpper() << QString::asprintf("devId: %04d VID:0x%04X, PID:0x%04X",devId,VID,PID);

                            addDevice(VID,PID,devId,path1,path2,connectType,0);
                            path1.clear();
                            path2.clear();

                            hid_close(pDev);
                        }
                    }
                }
            }
            pEDev = pEDev->next;
        }
        if(pRoot) hid_free_enumeration(pRoot);
    }

    QString strCmd0("04 00 00 1a 06 00 00 00"); // 电量
    QString strCmd1("04 00 00 30 06 00 00 00"); // 机型ID
    QString strCmd2("04 AA 00 AA 00 00 00 00"); // 连接状态
    QList<quint16>VShengs = {0x38EE,0x320F};
    stringToInt(enumSet.value("VIDset1","0x38EE, 0x320F").toString(),VidList);
    foreach (quint16 Vid, VShengs)
    {
        hid_device_info *pRoot = hid_enumerate(Vid, 0);
        hid_device_info *pEDev = pRoot;
        while (pEDev && !m_bForMGK)
        {
            VID = pEDev->vendor_id;
            PID = pEDev->product_id;
            UPG = pEDev->usage_page;
            USA = pEDev->usage;
            PATH= pEDev->path;

            if((PID >= 0x0B && PID <= 0x50) || (PID == 0x5151 || PID == 0x5152 || PID == 0x22B4 || PID == 0x22B5))
            {
                //qDebug().noquote() << QString::asprintf("VID=0x%04X PID=0x%04X usage_page=0x%04X usage=0x%04X %s",VID,PID,UPG,USA,PATH);
                if(USA == 0x0092 && UPG == 0xFF1C)
                {
                    hid_device *pDev = hid_open_path(PATH);
                    if(pDev)
                    {
                        quint8 szBuf[128] = {0};
                        QByteArray cmd = QByteArray::fromHex(strCmd2.toLatin1());

                        int len1 = 0;
                        for(int i=0; i<10; i++)
                        {
                            cmd = QByteArray::fromHex(strCmd1.toLatin1());
                            hid_write(pDev,(quint8*)cmd.data(),cmd.size());
                            QThread::msleep(20);
                            len1 = hid_read_timeout(pDev,szBuf,16,50);

                            if(szBuf[3] == 0x30 && szBuf[4] == 0x06) break;
                            QThread::msleep(10);
                        }

                        if(len1 >= 12)
                        {
                            quint8  device = szBuf[11];
                            quint32 driverId = 0;
                            quint8  connectType = 0;

                            /*
                            switch(PID)
                            {
                            case 0x0013: connectType = 1;
                            case 0x0012:
                                if(device == 0) driverId = 25;
                                break;

                            case 0x000C: connectType = 1;
                            case 0x000B:
                                driverId = 4;
                                if(device != 0) driverId =  6;
                                if(device == 2) driverId = 12;
                                if(device == 3) driverId = 18;
                                if(device == 4) driverId = 23;
                                if(device == 5) driverId = 24;
                                if(device == 6) driverId = 26;
                                break;

                            case 0x0011:
                            case 0x000F: connectType = 1;
                            case 0x0010:
                            case 0x000D:
                                if(device == 1) driverId =  7;
                                if(device == 3) driverId =  8;
                                if(device == 0) driverId =  9;
                                if(device == 4) driverId = 10;
                                if(device == 5) driverId = 15;
                                if(device == 6) driverId = 16;
                                if(device == 7) driverId = 19;
                                if(device == 8) driverId = 20;
                                if(device == 9) driverId = 21;
                                if(device ==10) driverId = 28;
                                if(PID == 0x000D || PID == 0x000F)
                                if(device == 1) driverId = 17;
                                break;

                            case 0x0024: connectType = 1;
                            case 0x0023:
                                driverId = 14;
                                if(device == 5) driverId = 13;
                                if(device == 1) driverId = 21;
                                if(device == 2) driverId = 27;
                                break;

                            case 0x0026: connectType = 1;
                            case 0x0025:
                                driverId = 9;
                                if(device == 1) driverId = 10;
                                break;

                            case 0x0028: connectType = 1;
                            case 0x0027:
                                driverId = 9;
                                if(device == 1) driverId = 10;
                                break;

                            case 0x0030: connectType = 1;
                            case 0x0029:
                                driverId = 15;
                                if(device == 1) driverId = 16;
                                if(device == 5) driverId = 15;
                                if(device == 6) driverId = 16;
                                break;

                            case 0x0032: connectType = 1;
                            case 0x0031:
                                driverId = 15;
                                if(device == 1) driverId = 16;
                                break;

                            case 0x5152: connectType = 1;
                            case 0x5151:
                                driverId = 5;
                                break;

                            case 0x0008: connectType = 1;
                            case 0x0007:
                                driverId = 11;
                                break;

                            case 0x22b5: connectType = 1;
                            case 0x22b4:
                                driverId = 12;
                                break;
                            }

                            if(PID == 0x22B4) driverId = 6;
                            if(PID == 0x000F) connectType = 1;

                            if(VID == 0x3311)
                            {
                                if(device == 0) driverId = 15;
                                if(device == 1) driverId = 16;
                            }*/

                            //QByteArray Log((char *)szBuf,len1);
                            //qDebug().noquote() << "read:" << Log.left(16).toHex(' ').toUpper() << QString::asprintf("PID: 0x%04X",PID) << device << "Device ID:" << device;

                            addDevice(VID,PID,device,"null",PATH,connectType,1);
                            allPaths.push_back(PATH);
                        }
                    }
                }
            }
            pEDev = pEDev->next;
        }
        if(pRoot) hid_free_enumeration(pRoot);
    }

    {
        quint8 kReadBufferHead = 0x82;
        quint8 kReadCodeSubCmd = 0x01;
        quint8 kReportId = 0x09;

        hid_device_info* pRoot = hid_enumerate(0x38ee, 0x0016);
        hid_device_info* pEDev = pRoot;
        while (pEDev)
        {
            VID = pEDev->vendor_id;
            PID = pEDev->product_id;
            UPG = pEDev->usage_page;
            USA = pEDev->usage;
            PATH= pEDev->path;
            //qDebug().noquote() << QString::asprintf("VID=0x%04X PID=0x%04X usage_page=0x%04X usage=0x%04X",VID,PID,UPG,USA);
            if(pEDev->usage_page == 65376) // 65280
            {
                hid_device* pDev = hid_open_path(PATH);
                if (pDev)
                {
                    quint8 report[128]={0};
                    report[0] = kReportId;
                    report[1] = kReadBufferHead;
                    report[2] = kReadCodeSubCmd;
                    report[4] = 1;
                    report[6] = 0x06;

                    unsigned char sum = 0;
                    for (int index = 0; index < 63; index++)
                        sum += report[index];
                    report[63] = static_cast<unsigned char>(0xFF - sum);

                    hid_write(pDev,report,64);
                    hid_read_timeout(pDev, report, 64, 50);

                    QByteArray Log((char *)report,64);
                    QString Uuid = Log.mid(7,6).toHex();
                    addDevice(VID,PID,Uuid.toULongLong(),Uuid,Uuid,0,2);

                    allPaths.push_back(Uuid);

                    //qDebug().noquote() << QString::asprintf("VID=0x%04X PID=0x%04X usage_page=0x%04X usage=0x%04X",VID,PID,UPG,USA);
                    hid_close(pDev);
                }
            }

            pEDev = pEDev->next;
        }
        if(pRoot) hid_free_enumeration(pRoot);
    }

    {
        hid_device_info* pRoot = hid_enumerate(0x3554, 0);
        hid_device_info* pEDev = pRoot;
        while (pEDev && !m_bForMGK)
        {
            VID = pEDev->vendor_id;
            PID = pEDev->product_id;
            UPG = pEDev->usage_page;
            USA = pEDev->usage;
            PATH= pEDev->path;
            //qDebug().noquote() << QString::asprintf("VID=0x%04X PID=0x%04X usage_page=0x%04X usage=0x%04X",VID,PID,UPG,USA);
            //qDebug() << QString::asprintf("0x%04X %04d",pEDev->usage_page, pEDev->usage) << pEDev->path;
            if(UPG == 0xFF02 && USA == 0x02)
            {
                //qDebug().noquote() << QString::asprintf("VID=0x%04X PID=0x%04X usage_page=0x%04X usage=0x%04X",VID,PID,UPG,USA);
                addDevice(VID,PID,0xFB29,"null",PATH,0,3);
                allPaths.push_back(PATH);
            }

            pEDev = pEDev->next;
        }
        if(pRoot) hid_free_enumeration(pRoot);
    }

    int count = m_tmp.count();
    for(int i=count-1; i>=0; i--)
    {
        if(!allPaths.contains(m_tmp[i]->strPath2))
        {
            if(m_pActDev == m_tmp[i])
            {
                qDebug() << "Lost:" << m_pActDev->strName << "Set AkkoReturn 1";
                m_pSet->setValue("AkkoReturn", 1);
            }
            m_tmp[i]->toShow = false;
        }

        if(time(nullptr) - m_tmp[i]->lastTime>60)
        {
            m_tmp[i]->toShow = false;
        }
    }

    emit oEnumDeiceDone();
    m_bEnuming = false;
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

void MainWindow::showEvent(QShowEvent *event)
{
    setAttribute(Qt::WA_Mapped);

    this->raise();

    if(m_bManHide)
        m_pSet->setValue("AkkoReturn",1);
    m_bManHide = false;

    QMainWindow::showEvent(event);
}

void MainWindow::updateDeviceInfo()
{
    int nShow = 0;
    int count = m_tmp.count();
    for(int i=count-1; i>=0; i--)
    {
        if(m_tmp[i]->toShow) nShow++;
    }

    FrameDeviceShow::HideDevieByIndex(nShow);
    QString strInfo = QString(tr("我的设备")) + QString("(%1)").arg(nShow);
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
            m_pFloatReturn->hide();
            ui->stackedWidget->setCurrentIndex(0);
            ::ShowWindow(s_hWndEmb[0],SW_HIDE);
            ::ShowWindow(s_hWndEmb[2],SW_HIDE);
            ::ShowWindow(s_hWndEmb[3],SW_HIDE);
            QTimer::singleShot(100,this,[=]{ setHubSize(true); });
        }

        ui->scrollArea->update();
    }

    if(e->type() != QEvent::Paint)
    {
        if(obj == m_pFloatReturn || m_pFloatLeft == obj || m_pFloatRight == obj)
        {
            qreal scale = 1.0;
            int nYPos = geometry().height()/2 + 50;
            QPoint P1 = mapToGlobal(QPoint(20,nYPos));
            QPoint P2 = mapToGlobal(QPoint(geometry().width() - 70*scale,nYPos));
            m_pFloatLeft->setGeometry(P1.x(),P1.y(),48*scale,48*scale);
            m_pFloatRight->setGeometry(P2.x(),P2.y(),48*scale,48*scale);

            QPoint P3 = mapToGlobal(QPoint(15,ui->stackedWidget->geometry().top()+5));
            m_pFloatReturn->setGeometry(P3.x()+(m_creator == 2 ? 40 : 0),P3.y(),80*scale,32*scale);
        }

        if(obj == ui->scrollArea->viewport() || m_pFloatLeft == obj || m_pFloatRight == obj)
        {
            if(e->type() == QEvent::Enter)
            {
                m_pTmHide->stop();
                m_pLangMenu->hide();
                if(m_layout->count() >= 3)
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
            p.setRenderHint(QPainter::SmoothPixmapTransform, true);
            QRect rect = m_pFloatReturn->rect();
            QRgb A = qRgba(255,255,255,100);
            p.setBrush(QColor::fromRgba(A));
            p.setPen(QColor::fromRgba(qRgba(100,100,100,100)));
            p.drawRoundedRect(rect.adjusted(1,1,-1,-1),16,16);
            p.drawImage(rect.adjusted(24,2,-24,-2),QImage(":/images/btn-return.png"));
        }
    }

    return QMainWindow::eventFilter(obj, e);
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::NonClientAreaMouseMove)
    {
        show();
        setFocus();
        activateWindow();
        raise();

        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        qDebug() << "非客户区鼠标移动：" << me->globalPos();
        event->accept();
        return true;
    }

    if(event->type() == QEvent::Leave)
    {
        QTimer::singleShot(100,this,[=]{
            if(!geometry().contains(QCursor::pos()))
            {
                m_bActive=false;
            }
        });
        if(!geometry().contains(QCursor::pos()))
            m_pLangMenu->hide();
    }

    if(event->type() == QEvent::Enter)
    {
        m_bActive=true;
    }

    if(event->type() == QEvent::WindowDeactivate)
    {
        if(!geometry().contains(QCursor::pos()))
        {
            m_bActive=false;
        }

        if(!m_closeShow)
        {
            QTimer::singleShot(500,this,[=]{
                if(m_pFloatReturn->isHidden()) return;
            });
        }
    }

    if(event->type() == QEvent::WindowActivate)
    {
        m_bActive = true;
    }

    return QMainWindow::event(event);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    ui->labelLogo->hide();

    int borderRadius = 20;
    QPainterPath path;
    path.addRoundedRect(this->rect(), borderRadius, borderRadius);
    p.setClipPath(path);

    if (ui->stackedWidget->isHidden())
    {
        m_pFloatReturn->hide();
        p.drawImage(this->rect(), QImage(QApplication::applicationDirPath() + (m_bForMGK ? "/images/mogear.png" : "/images/MainPicture.png")));

        QFont font = this->font();
        font.setBold(true);
        font.setPointSize(22);
        p.setFont(font);
        int adjust = this->height() - 100;
        p.setPen(Qt::white);
        p.drawText(this->rect().adjusted(0,adjust,0,0), Qt::AlignCenter, tr("正在搜索设备") + QString("..."));
        if(m_bForMGK)
            p.drawImage(QRect(30,40,427,55), QImage(":/images/Mgklogo.png"));
    }
    else
    {
        p.fillRect(this->rect(), Qt::white);
        p.fillRect(this->rect(), QColor("#FBFBFB"));
        //p.drawImage(this->rect(), QImage(QApplication::applicationDirPath() + (m_bForMGK ? "/images/mogear.png" : "/images/MainPicture.png")));
        if(m_bForMGK)
            p.drawImage(QRect(30,25,315,40), QImage(":/images/Mgklogo.png"));
        else
            p.drawImage(QRect(30,25,136,40), QImage(":/images/AkkoFlag.png"));
    }

    p.setPen(Qt::blue);
    p.drawRoundedRect(this->rect(), borderRadius, borderRadius);

    QMainWindow::paintEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_pLangMenu->hide();
    m_closeShow = true;
    auto res = QMessageBox::question(this, tr("提示"), tr("确定要退出 AKKO 驱动程序？"));
    if(res != QMessageBox::Yes)
    {
        m_closeShow = false;
        event->ignore();
        return;
    }

    killProcess(m_strRyName);
    killProcess(m_strWsName);
    killProcess(m_strByName);
    killProcess(m_strJmName);

    m_Enum->Exit();
    m_pFloatReturn->hide();
    trayIcon->hide();

    for(int i=0; i<10; i++)
    {
        if(!s_hWndEmb[i]) continue;
        ::PostMessage(s_hWndEmb[i],WM_CLOSE,0,0);
    }

    qApp->exit();

    QMainWindow::closeEvent(event);
}

void MainWindow::on_pushButtonExit_clicked()
{
    this->close();
}

void MainWindow::on_pushButtonMin_clicked()
{
    this->showMinimized();
    this->lower();
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
        {
            static int nCount = 0;
            static QTimer *pCntTM = new QTimer(this);
            nCount++;
            pCntTM->stop();
            pCntTM->start(300);
            connect(pCntTM,&QTimer::timeout,this,[=]{ nCount=0; });
            if(nCount >= 2 && (::GetKeyState(VK_CONTROL)&0x800) && (::GetKeyState('D')&0x800) )
                DialogDeviceConnect::instance()->show();
        }
        if (event->pos().y() < 60)
        {
            m_dragPosition = event->globalPosition() - frameGeometry().topLeft();
            m_dragging = true;
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
        m_pFloatReturn->raise();
    }

    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragging = false;

    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::setHubSize(bool origin)
{
    int width  = 1280;
    int height =  900;
    if(!origin) width  = 1520;
    if(!origin) height =  900;

    QSize cs = QApplication::screens().at(0)->size();
    int x = (cs.width() - width)/2 ;
    int y = (cs.height() - height)/2;
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    QRect rcSet(x, y,width,height);

    setGeometry(rcSet);
    setFixedSize(width,height);
}