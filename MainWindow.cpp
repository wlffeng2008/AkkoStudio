#include "MainWindow.h"
#include "ModuleLangMenu.h"
#include "ui_MainWindow.h"

#include "FrameDeviceShow.h"

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

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "AkkoDeviceBase.h"
#include "DialogDeviceConnect.h"
#include "FrameDeviceHolder.h"

static HWND s_hWndEmb0 = NULL;
static HWND s_hWndEmb1 = NULL;
static QSettings settings("HKEY_CURRENT_USER\\Software\\Akko",QSettings::NativeFormat);


#include<tlhelp32.h>
void killProcess(QString processName)
{
    if (processName.isEmpty())
    {
        return;
    }

    HANDLE handle32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == handle32Snapshot)
        return;

    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(PROCESSENTRY32);

    BOOL bProcess = Process32First(handle32Snapshot, &pEntry);

    while (bProcess)
    {
        QString thisProcessName = QString::fromWCharArray(pEntry.szExeFile);
        if (thisProcessName == processName) {
            //qInfo() << "找到目标进程";
            HANDLE handLe = OpenProcess(PROCESS_TERMINATE, FALSE, pEntry.th32ProcessID);
            if (handLe == NULL) {
                qCritical() << "没有打开目标进程";
                return;
            }
            BOOL ret = TerminateProcess(handLe, 0);
            Q_UNUSED(ret);
            //qInfo() << "关闭目标进程是否成功:" << (ret ? "成功" : "失败");
        }
        bProcess = Process32Next(handle32Snapshot, &pEntry);
    }
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

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
        ui->pushButtonLang->setText(QString(" ") + lang);

        {
            QStringList langs = {"cn","en","tw","jp","kr","ru","vi","pt","th","de","fr","sv","it","tr"};
            settings.setValue("LanguageCode",langs[langId]);
        }

        {
            int id = langId;
            switch(langId)
            {
            case 0:
            case 1:
                break;
            default:
                id=2;
                break;
            }
            int langs[3] = {2052,1033,1049};
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


    m_layout = ui->scrollAreaWidgetContents->layout();

    ui->labelPrev->setHidden(true);
    ui->labelNext->setHidden(true);

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
    m_pFloatLeft->raise();
    m_pFloatRight->raise();

    m_pFloatLeft->setWindowOpacity(0.6);
    m_pFloatRight->setWindowOpacity(0.6);

    ui->labelPrev->installEventFilter(this);
    ui->labelNext->installEventFilter(this);
    m_pFloatLeft->installEventFilter(this);
    m_pFloatRight->installEventFilter(this);
    m_pFloatReturn->installEventFilter(this);
    ui->scrollArea->viewport()->installEventFilter(this);

    m_pTmHide = new QTimer(this);
    connect(m_pTmHide,&QTimer::timeout,this,[=]{
        m_pTmHide->stop();
        m_pFloatLeft->hide();
        m_pFloatRight->hide();
    });

    enumDevice();
    connect(ui->pushButtonScan, &QPushButton::clicked, this, [=] { enumDevice(); });

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
    settings.setValue("DevicePath","Hello-path-1234567890");
    settings.setValue("iotManagerInitialized",false);

    QTimer *pTMRet = new QTimer(this);
    pTMRet->start(100);
    connect(pTMRet,&QTimer::timeout,this,[=]{
        if(settings.value("AkkoReturn", 0).toInt() == 1)
        {
            settings.setValue("AkkoReturn", 0);
            ui->stackedWidget->setCurrentIndex(0);
            m_pFloatReturn->hide();
        };
    });

    //qDebug()<< QProcess::systemEnvironment();
    killProcess("akko.exe");
    killProcess("Akko Cloud Driver v4.exe");
    QProcess::startDetached("akko.exe", QStringList{"/super"});
    QProcess::startDetached("RyExe/Akko Cloud Driver v4.exe", QStringList{});

    QTimer *pTMFindWnd = new QTimer(this);
    pTMFindWnd->start(100);

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
                style &= ~WS_VISIBLE;
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
                style &= ~WS_VISIBLE;
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

        HWND hParentWnd = (HWND)ui->frameEmb->winId();
        if(s_hWndEmb0)
        {
            ::SetParent(s_hWndEmb0,hParentWnd);
            ::SetWindowPos(s_hWndEmb0, HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE);
        }

        if(s_hWndEmb1)
        {
            ::SetParent(s_hWndEmb1,hParentWnd);
            ::SetWindowPos(s_hWndEmb1, HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE);
        }
        static int nCount = 0;
        if(nCount ++ > 10)
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
                    showNormal();
                else
                    this->hide();
            }
        }) ;

        QMenu *trayMenu = new QMenu(this);

        QAction *showAction = new QAction(tr("显示窗口"), this);
        QAction *hideAction = new QAction(tr("隐藏窗口"), this);
        QAction *exitAction = new QAction(tr("退出程序"), this);

        trayMenu->addAction(showAction);
        trayMenu->addAction(hideAction);
        trayMenu->addSeparator();
        trayMenu->addAction(exitAction);

        connect(exitAction, &QAction::triggered, this, &QApplication::quit);
        connect(showAction, &QAction::triggered, this, &QMainWindow::showNormal);
        connect(hideAction, &QAction::triggered, this, &QMainWindow::hide);
        trayIcon->setContextMenu(trayMenu);
    }


    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->frameHold,&FrameDeviceHolder::onReturn,this,[=]{
        ui->stackedWidget->setCurrentIndex(0);
    });

    //resize(2560,1800);
}

void MainWindow::addDevice(quint32 id,const QString &path, int creator)
{
    AkkoDeviceInfo *dev = getDevice(id);
    if (dev)
    {
        // FrameDeviceShow *device = new FrameDeviceShow(this);
        FrameDeviceShow *device = FrameDeviceShow::getFrameShow(m_layout->count(),this);
        if (dev->type == 0)device->setFixedWidth(970);
        if (dev->type == 1)device->setFixedWidth(280);
        if (dev->type == 2)device->setFixedWidth(280);
        device->m_sa = ui->scrollArea;
        device->m_device = dev;
        device->setName(dev->name);
        device->setPath(path);
        device->setCreator(creator);
        device->show();
        m_layout->addWidget(device);

        if(!device->m_bConacted)
        {
            device->m_bConacted = true;
            connect(device,&FrameDeviceShow::onClicked,this,[=](void *device,const QString&path,const QString&image,int creator){

                m_creator = creator;
                m_pLangMenu->hide();
                AkkoDeviceInfo *dev = static_cast<AkkoDeviceInfo *>(device);
                if(dev->id == 2807)
                {
                    DialogDeviceConnect::instance()->DoConnectDevice(dev->PID);
                    ui->stackedWidget->setCurrentIndex(2);
                    ui->frameHold->setDeviceImage(image);
                    return;
                }

                HWND hWnd = s_hWndEmb0;
                if(creator == 0)
                {
                    settings.setValue("iotManagerInitialized",true);
                    settings.setValue("DevicePath",path);
                    ::SetWindowPos(s_hWndEmb1, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);
                    ui->frameEmb->setStyleSheet("background-color: rgb(240, 240, 240); border-bottom-left-radius: 20px; border-bottom-right-radius:20px;");
                }
                else
                {
                    hWnd = s_hWndEmb1;
                    settings.setValue("AkkoDeviceIndex",dev->type == 0 ? 5 : 9);
                    ::SetWindowPos(s_hWndEmb0, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW);
                    ui->frameEmb->setStyleSheet("background-color: rgb(30, 30, 30); border-bottom-left-radius: 20px; border-bottom-right-radius:20px;");
                }

                ::SetWindowPos(hWnd, HWND_TOP, 0, 0, ui->frameEmb->width(), ui->frameEmb->height()-20, SWP_SHOWWINDOW | SWP_FRAMECHANGED);

                QTimer::singleShot(50,this,[=]{
                    m_pFloatReturn->show();
                    ui->stackedWidget->setCurrentIndex(1);
                    static QTimer *pTmChk = new QTimer(this);
                    pTmChk->stop();
                    pTmChk->start(100);
                    connect(pTmChk,&QTimer::timeout,this,[=]{
                        HWND hWnd = m_creator == 0 ? s_hWndEmb0 : s_hWndEmb1;
                        ::SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
                    });
                });
            });
        }
    }
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

    QStringList VIDList;
    QList<quint16> VidList = {0x3151, 0x38EE, 0x25A7, 0x05AC, 0x0461};
    foreach (quint16 VID, VidList)
    {
        hid_device_info *pRoot = hid_enumerate(VID, 0);
        hid_device_info *pTemp = pRoot;
        while (pTemp)
        {
            // qDebug()<< pTemp->path << pTemp->usage << pTemp->usage_page;
            if (pTemp->usage_page == 0xFFFF && pTemp->usage == 2)
            {
                hid_device *pDev = hid_open_path(pTemp->path);
                if (!pDev) continue;

                quint16 PID = pTemp->product_id;

                VIDList.push_back(QString::asprintf("%04X", PID));

                QByteArray cmd(120, 0);
                cmd[1] = 0x8F;
                cmd[8] = 0xFF - 0x8F;

                hid_send_feature_report(pDev, (quint8 *)cmd.data(), 65);
                QThread::msleep(5);

                char buf[128] = {0};
                int nlen = hid_get_feature_report(pDev, (quint8 *)buf, 65);
                if (nlen > 0)
                {
                    QByteArray data(buf + 1, nlen - 1);
                    quint32 id = *(quint32 *)(data.data() + 1);
                    qDebug() << "get_:" << data.left(16).toHex(' ').toUpper() << id << Qt::hex << PID;

                    addDevice(id,pTemp->path,0);
                }

                hid_close(pDev);
            }
            pTemp = pTemp->next;
        }
        hid_free_enumeration(pRoot);
    }

    QString strCmd0("04 00 00 1a 06 00 00 00");
    QString strCmd1("04 00 00 30 06 00 00 00");
    QList<quint16>VShengs={0x38EE,0x320F};
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

                VIDList.push_back(QString::asprintf("%04X", PID));

                quint8 szBuf[128] = {0};
                QByteArray cmd = QByteArray::fromHex(strCmd0.toLatin1());
                hid_write(pDev,(quint8*)cmd.data(),cmd.size());
                QThread::msleep(5);
                int len = hid_read(pDev,szBuf,16);

                QByteArray Log((char *)szBuf,len);
                qDebug() << "read:" << Log.left(16).toHex(' ').toUpper() << Qt::hex << PID;
                quint8 device = szBuf[11];

                cmd = QByteArray::fromHex(strCmd1.toLatin1());
                hid_write(pDev,(quint8*)cmd.data(),cmd.size());
                QThread::msleep(5);
                hid_read(pDev,szBuf,16);
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
                switch(PID)
                {
                case 0x000B:
                case 0x000C:
                    if(device == 0)
                        addDevice(4,"",1);
                    else
                        addDevice(6,"",1);
                    break;

                case 0x0010:
                case 0x0011:
                case 0x000D:
                case 0x000F:
                    if(device == 0) addDevice( 9,"",1);
                    if(device == 4) addDevice(10,"",1);
                    if(device == 1) addDevice( 7,"",1);
                    if(device == 3) addDevice( 8,"",1);
                    break;

                case 0x5151:
                case 0x5152:
                    addDevice(5,"",1);
                    break;

                case 0x0007:
                case 0x0008:
                    addDevice(11,"",1);
                    break;
                }

            }
            pTemp = pTemp->next;
        }
        hid_free_enumeration(pRoot);
    }

    QString strInfo = QString(tr("我的设备")) + QString("(%1)").arg(m_layout->count());
    ui->labelInfo0->setText(strInfo);
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
            ui->stackedWidget->setCurrentIndex(0);
            m_pFloatReturn->hide();
        }

        ui->scrollArea->update();
    }

    if(e->type() != QEvent::Paint)
    {
        qreal scale = 1.0;
        int nYPos = geometry().height()/2 + 50;
        QPoint P1 = mapToGlobal(QPoint(20,nYPos));
        QPoint P2 = mapToGlobal(QPoint(geometry().width() - 60*scale,nYPos));
        m_pFloatLeft->setGeometry(P1.x(),P1.y(),48*scale,48*scale);
        m_pFloatRight->setGeometry(P2.x(),P2.y(),48*scale,48*scale);
        QPoint P3 = mapToGlobal(QPoint(5,50));
        m_pFloatReturn->setGeometry(P3.x(),P3.y(),80*scale,32*scale);

        if(obj == ui->scrollArea->viewport() || m_pFloatLeft == obj || m_pFloatRight == obj)
        {
            if(e->type() == QEvent::Enter)
            {
                m_pTmHide->stop();
                m_pLangMenu->hide();
                m_pFloatLeft->show();
                m_pFloatRight->show();
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
            QStyleOption opt;
            opt.initFrom(m_pFloatLeft);
            QPainter p(m_pFloatLeft);
            p.setRenderHint(QPainter::Antialiasing, true);
            //style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
            //p.setBrush(QBrush(QColor(0,255,0,200)));
            //p.drawRoundedRect(m_pFloatLeft->rect(),24,24);
            p.drawImage(m_pFloatLeft->rect(),QImage(":/images/a-left.png"));
        }
        if(m_pFloatRight == obj)
        {
            QStyleOption opt;
            opt.initFrom(m_pFloatRight);
            QPainter p(m_pFloatRight);
            p.setRenderHint(QPainter::Antialiasing, true);
            //style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
            //p.setBrush(QBrush(QColor(0,255,0,200)));
            //p.drawRoundedRect(m_pFloatRight->rect(),24,24);
            p.drawImage(m_pFloatRight->rect(),QImage(":/images/a-right.png"));
        }

        if(m_pFloatReturn == obj)
        {
            QPainter p(m_pFloatReturn);
            p.setRenderHint(QPainter::Antialiasing, true);
            QRect rect = m_pFloatReturn->rect();
            QRgb A = qRgba(255,255,255,100);
            p.setBrush(QColor::fromRgba(A));
            p.drawRoundedRect(rect,16,16);
            p.drawImage(rect.adjusted(24,5,-24,0),QImage(":/images/btn-return.png"));
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

    int borderRadius = 20;
    QPainterPath path;
    path.addRoundedRect(this->rect(), borderRadius, borderRadius);

    p.setClipPath(path);
    if (ui->stackedWidget->isHidden())
    {
        p.drawImage(this->rect(), QImage("./images/MainPicture.png"));
        QFont font = this->font();
        font.setBold(true);
        font.setPointSize(50);
        p.setFont(font);
        int adjust = this->height() - 250;
        p.drawText(this->rect().adjusted(0,adjust,0,0), Qt::AlignCenter,QString(">>  ") + tr("请插入设备") + QString("  <<"));
    }
    else
    {
        p.fillRect(this->rect(), Qt::white);
    }

    p.setPen(Qt::blue);
    p.drawRoundedRect(this->rect(), borderRadius, borderRadius);

    QMainWindow::paintEvent(event);
}

void MainWindow::on_pushButtonExit_clicked()
{
    auto res = QMessageBox::question(this,tr("提示"),tr("确定要退出 AKKO 驱动程序？"));
    if(res == QMessageBox::Yes)
    {
        m_pFloatReturn->hide();
        qApp->exit();
    }
}

void MainWindow::on_pushButtonMin_clicked()
{
    this->showMinimized();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_pLangMenu->hide();
    if (event->button() == Qt::LeftButton)
    {
        if (event->pos().y() < 200)
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

        QPoint P3 = mapToGlobal(QPoint(5,35));
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
