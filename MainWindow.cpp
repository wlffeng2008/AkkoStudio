#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include <QRadioButton>
#include <QSpacerItem>
#include <QButtonGroup>
#include <QMovie>
#include <QGraphicsDropShadowEffect>
#include <QSystemTrayIcon>
#include <QMenu>       // 用于托盘菜单
#include <QAction>     // 用于菜单动作

#include "FrameMain.h"
#include "FrameKeySetting.h"
#include "FrameMacro.h"
#include "FrameMagic.h"
#include "FrameLight.h"
#include "FrameAbout.h"
#include "ModuleLangMenu.h"
#include "LinearFixing1.h"
#include "ModuleGeneralMasker.h"
#include "ModuleGenKeymapping.h"
#include "ModuleLinear.h"
#include <QMenu>
#include <QAction>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint|Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("QMainWindow{background-color: rgba(255, 255, 255, 1); border: 1px solid skyblue; border-radius: 12px; }");

    ModuleGenKeymapping *km = new ModuleGenKeymapping(this);
    km->hide();

    // QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    // shadowEffect->setBlurRadius(15);    // 阴影模糊半径，值越大越模糊
    // shadowEffect->setColor(QColor(0, 0, 0, 160)); // 阴影颜色和透明度 (RGBA)
    // shadowEffect->setOffset(0, 0);      // 阴影偏移量，设为(0,0)四周均匀
    // this->setGraphicsEffect(shadowEffect);
    // centralWidget()->setContentsMargins(10, 10, 10, 10);

    setWindowTitle(QString("AKKO Studio -- By QT") + QT_VERSION_STR);
    resize(1280,900) ;

    {
        setFocusPolicy(Qt::StrongFocus) ;

        ui->pushButtonPaire->hide();

        m_pDevice = new DialogDeviceConnect(this);

        FrameKeySetting *pKSet = new FrameKeySetting(this);

        FrameMain  *pFMain = new FrameMain (this);
        FrameMagic *pMagic = new FrameMagic(this);
        FrameMacro *pMacro = new FrameMacro(this);
        FrameLight *pLight = new FrameLight(this);
        FrameAbout *pAbout = new FrameAbout(this);
        pKSet->hide() ;

        m_pFrames.push_back(pFMain);
        m_pFrames.push_back(pKSet );
        m_pFrames.push_back(pMagic);
        m_pFrames.push_back(pMacro);
        m_pFrames.push_back(pLight);
        m_pFrames.push_back(pAbout);

        ui->horizontalLayoutBR->addWidget(pFMain);
        ui->horizontalLayoutBR->addWidget(pKSet );
        ui->horizontalLayoutBR->addWidget(pMagic);
        ui->horizontalLayoutBR->addWidget(pMacro);
        ui->horizontalLayoutBR->addWidget(pLight);
        ui->horizontalLayoutBR->addWidget(pAbout);

        for(QFrame*frame:std::as_const(m_pFrames))
            frame->setFixedSize(1124,800) ;

        m_pLBtns.push_back(ui->label1);
        m_pLBtns.push_back(ui->label2);
        m_pLBtns.push_back(ui->label3);
        m_pLBtns.push_back(ui->label4);
        m_pLBtns.push_back(ui->label5);
        m_pLBtns.push_back(ui->label6);
        m_pLBtns.push_back(ui->label7);

        ui->label7->hide();

        int index = 0 ;
        static QStringList images={"shouye","jianweishezhi","cizhoushezhi","hongshezhi","dengxiaoshezhi","guanyu","",""};
        foreach (SuperLabel *label, m_pLBtns) {
            label->setCursor(Qt::PointingHandCursor);
            label->installEventFilter(this) ;
            label->setAlignment(Qt::AlignCenter) ;
            label->setOwnSheet("QLabel { background-color: #6329B6; border-radius: 16px;}","QLabel { background-color: transparent; }") ;
            QString strImg = images[index++] + ".png";
            label->setImages(QString(":/images/leftbtns/1/") + strImg,QString(":/images/leftbtns/0/") + strImg) ;
        }
        clickLabel(ui->label1) ;

        m_pLBtns[6]->setImages(QString(":/images/User.png"),QString(":/images/User.png")) ;

        QString strStyle(R"(

        QPushButton {
            color: #333;
            margin-left:24px;
            border: none;
            font: 500 16px "MiSans";
            background: transparent;
            spacing:20px; }

        )") ;
        ui->pushButtonLang->setStyleSheet(strStyle);
        ui->pushButtonFixed->setStyleSheet(strStyle);
        ui->pushButtonReset->setStyleSheet(strStyle);
        ui->pushButtonPaire->setStyleSheet(strStyle);

        m_pLangMenu = new ModuleLangMenu(this) ;
        connect(m_pLangMenu,&ModuleLangMenu::onLangChanged,this,[=](int langId,const QString&lang){
            ui->pushButtonLang->setText(QString(" ")+lang) ;
        }) ;

        connect(ui->pushButtonLang,&QPushButton::clicked,this,[=]{
            ModuleLangMenu *pLangMenu = m_pLangMenu ;
            if(!pLangMenu->isHidden())
                return ;

            pLangMenu->setParent(nullptr) ;
            QRect btnRect = ui->pushButtonLang->geometry() ;
            QPoint PT = mapToGlobal(btnRect.bottomLeft());
            pLangMenu->setGeometry(PT.x() + 25,PT.y()+30,110,250);
            pLangMenu->setWindowFlags(Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint|Qt::Tool|Qt::Dialog|Qt::Popup);
            pLangMenu->show() ;
        });

        connect(ui->pushButtonReset,&QPushButton::clicked,this,[=]{
            m_pDevice->reset() ;
        });

        connect(ui->pushButtonFixed,&QPushButton::clicked,this,[=]{
            LinearFixing1 T("","",this) ;
            T.exec() ;
        });

        connect(ui->pushButtonPaire,&QPushButton::clicked,this,[=]{

            ModuleLinear *pTest = new ModuleLinear(this) ;
            pTest->setObjectName("TestLinear");
            pTest->layout()->setContentsMargins(20,20,20,20) ;
            pTest->setStyleSheet("QFrame#TestLinear{background-color:white;min-height:320px; border-radius:24px;}") ;
            ModuleGeneralMasker T(pTest ,this) ;
            T.exec() ;
        });
    }

    QTimer *pTMConnect = new QTimer(this) ;

    connect(ui->pushButtonDevice,&QPushButton::clicked,this,[=]{
        m_pDevice->show() ;
    });

    connect(ui->pushButtonEnter,&QPushButton::clicked,this,[=]{
        if(m_nStatus==1)
        {
            ui->stackedWidget->setCurrentIndex(1);
        }
        else
        {
            setConnect(0);
            pTMConnect->start(5000);
            m_pDevice->startConnect();
        }
    });

    connect(pTMConnect,&QTimer::timeout,this,[=]{
        ui->stackedWidget->setCurrentIndex(0);
        setConnect(2);
    });

    connect(ui->pushButtonExit,&QPushButton::clicked,this,[=]{
        close() ;
    });

    QList<QPushButton *>btns = findChildren<QPushButton *>();
    for(QPushButton *btn:std::as_const(btns))
    {
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFocusPolicy(Qt::ClickFocus) ;
    }

    QMovie *pWMovie = new QMovie(":/images/inworking.gif");
    ui->labelGif->setMovie(pWMovie);
    ui->labelGif->setFixedSize(282,60) ;
    pWMovie->start();

    connect(m_pDevice,&DialogDeviceConnect::onConnect,this,[=]{
        pTMConnect->stop();
        setConnect(1);
    });
    connect(m_pDevice,&DialogDeviceConnect::onDisconnect,this,[=]{
        ui->stackedWidget->setCurrentIndex(0);
        pTMConnect->stop();
        setConnect(2);
    });

    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButtonEnter->click() ;

    {
        QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
        trayIcon->setIcon(QIcon(":/images/logo.png"));
        trayIcon->setToolTip("AKKO Cloud Driver");
        trayIcon->show();

        connect(trayIcon,&QSystemTrayIcon::activated,this,[=](QSystemTrayIcon::ActivationReason reason){
            if(reason != QSystemTrayIcon::Context)
            {
                if(isHidden())
                    showNormal() ;
                else
                    hide() ;
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

    m_pDevice->readAllData();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::clickLabel(QLabel *label, int index)
{
    if(index == 6)
    {
        // hide() ;
        return ;
    }

    ((SuperLabel *)label)->setFocus();

    for(int i=0; i<m_pFrames.count(); i++)
    {
        m_pFrames[i]->hide();
    }
    m_pFrames[index]->show();
}

bool MainWindow::eventFilter(QObject *watch, QEvent *event)
{
    //qDebug() << event->type() ;
    if(event->type() == QEvent::MouseButtonRelease)
    {
        int nCount = m_pLBtns.count() ;
        for(int i=0; i<nCount; i++)
        {
            if(m_pLBtns[i] == watch)
            {
                QLabel* label = qobject_cast<QLabel*>(watch);
                clickLabel(label,i) ;
                return true ;
            }
        }
    }
    if(event->type() == QEvent::Paint || event->type() == QEvent::WindowDeactivate || event->type() == QEvent::WindowActivate)
    {
        m_pLangMenu->hide() ;
    }

    return QMainWindow::eventFilter(watch, event);
}

void MainWindow::setConnect(int nFlag)
{
    m_nStatus = nFlag;
    ui->labelStatus->setTextFormat(Qt::RichText);
    ui->pushButtonEnter->setHidden(true);
    ui->labelGif->setHidden(true) ;
    ui->labelKeyboard->setHidden(true) ;

    QString strText0(tr("欢迎使用AKKO产品，"));
    QString strText1(tr("连接成功"));
    QString strText2(tr("暂未搜索到有效设备，请检查设备是否已正常连接"));
    QString strText3(tr("正在搜索设备......"));

    switch(nFlag)
    {
    case 1:
        ui->labelStatus->setText(QString(R"(<html><head/><body><p><span style=" font-size:16pt;">%1</span><span style=" font-size:16pt; color:#35ac4f;">%2</span></p></body></html>)").arg(strText0,strText1)) ;
        ui->pushButtonEnter->setText(tr("立即进入")) ;
        ui->pushButtonEnter->setHidden(false);
        ui->labelKeyboard->setHidden(false) ;
        break;

    case 2:
        ui->labelStatus->setText(QString(R"(<html><head/><body><p><span style=" font-size:16pt;">%1</span></p></body></html>)").arg(strText2));
        ui->pushButtonEnter->setText(tr("重新搜索")) ;
        ui->pushButtonEnter->setHidden(false);
        break;

    default:
        ui->labelStatus->setText(QString(R"(<html><head/><body><p><span style=" font-size:16pt;">%1</span></p></body></html>)").arg(strText3));
        ui->labelGif->setHidden(false) ;
        break;
    }
    update() ;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_pDevice->disconnect() ;

    QCoreApplication::exit() ;
    QMainWindow::closeEvent(event);
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
    //qDebug() << "MainWindow::focusOutEvent" ;
    QMainWindow::focusOutEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //qDebug() <<"MainWindow::keyPressEvent:" << event->nativeScanCode() << event->nativeVirtualKey() ;
    if(event->key() == Qt::Key_Escape)
    {
        //close() ;
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    //qDebug() <<"MainWindow::keyReleaseEvent:" << event->nativeScanCode() << event->nativeVirtualKey() ;
    if(event->key() == Qt::Key_Escape)
    {
        //close() ;
        return ;
    }

    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(event->pos().y() < 100)
        {
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
            m_dragging = true ;
            return ;
        }
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_dragging) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
    // QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
    // QMainWindow::mouseReleaseEvent(event);
}


void MainWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QMainWindow::paintEvent(event);
}

