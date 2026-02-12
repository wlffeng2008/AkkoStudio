#include "FrameDeviceHolder.h"
#include "ui_FrameDeviceHolder.h"

#include <QPainter>
#include <QGraphicsDropShadowEffect>

#include "FrameMain.h"
#include "FrameKeySetting.h"
#include "FrameMacro.h"
#include "FrameMagic.h"
#include "FrameLight.h"
#include "FrameAbout.h"
#include "LinearFixing1.h"
#include "ModuleGeneralMasker.h"
#include "ModuleLinear.h"
#include "DialogDeviceConnect.h"


FrameDeviceHolder::FrameDeviceHolder(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameDeviceHolder)
{
    ui->setupUi(this);

    DialogDeviceConnect *pCnnt = new DialogDeviceConnect(this);
    ModuleGenKeymapping *pKmap = new ModuleGenKeymapping(this);

    FrameKeySetting *pKS = new FrameKeySetting(this);
    FrameMain *pFM = new FrameMain(this);
    pKS->hide();
    FrameMagic *pMagic = new FrameMagic(this);
    FrameMacro *pMacro = new FrameMacro(this);
    FrameLight *pLight = new FrameLight(this);
    FrameAbout *pAbout = new FrameAbout(this);

    m_pFrames.push_back(pFM) ;
    m_pFrames.push_back(pKS) ;
    m_pFrames.push_back(pMagic) ;
    m_pFrames.push_back(pMacro) ;
    m_pFrames.push_back(pLight) ;
    m_pFrames.push_back(pAbout) ;

    ui->horizontalLayoutBR->addWidget(pFM);
    ui->horizontalLayoutBR->addWidget(pKS);
    ui->horizontalLayoutBR->addWidget(pMagic);
    ui->horizontalLayoutBR->addWidget(pMacro);
    ui->horizontalLayoutBR->addWidget(pLight);
    ui->horizontalLayoutBR->addWidget(pAbout);

    for(QFrame*frame:std::as_const(m_pFrames))
        frame->setFixedSize(1124,800);

    m_pLBtns.push_back(ui->label1);
    m_pLBtns.push_back(ui->label2);
    m_pLBtns.push_back(ui->label3);
    m_pLBtns.push_back(ui->label4);
    m_pLBtns.push_back(ui->label5);
    m_pLBtns.push_back(ui->label6);
    m_pLBtns.push_back(ui->label7);

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

    m_pLBtns[6]->setImages(QString(":/images/User.png"),QString(":/images/User.png"));

    connect(ui->pushButtonReturn,&QPushButton::clicked,this,[=]{
        emit onReturn();
    });

    connect(ui->pushButtonFixline,&QPushButton::clicked,this,[=]{
        LinearFixing1 T("","",this) ;
        T.exec() ;
    });

    connect(ui->pushButtonReset,&QPushButton::clicked,this,[=]{
        DialogDeviceConnect::instance()->reset();
    });

    ui->pushButtonPairing->hide();
    connect(ui->pushButtonPairing,&QPushButton::clicked,this,[=]{

        ModuleLinear *pTest = new ModuleLinear(this) ;
        pTest->setObjectName("TestLinear");
        pTest->layout()->setContentsMargins(20,20,20,20) ;
        pTest->setStyleSheet("QFrame#TestLinear{background-color:white;min-height:320px; border-radius:24px;}") ;
        ModuleGeneralMasker T(pTest ,this) ;
        T.exec();
    });
}

FrameDeviceHolder::~FrameDeviceHolder()
{
    delete ui;
}

void FrameDeviceHolder::setDeviceImage(const QString&strImage)
{
    FrameMain *pFM = (FrameMain *)m_pFrames[0];
    pFM->setDeviceImage(strImage);
}


void FrameDeviceHolder::clickLabel(QLabel *label, int index)
{
    if(index == 6)
    {
        hide();
        return;
    }

    ((SuperLabel *)label)->setFocus();

    for(int i=0; i<m_pFrames.count(); i++)
    {
        QFrame *pFM = m_pFrames[i];
        pFM->hide();
    }
    QFrame *pFM = m_pFrames[index];
    pFM->show();
}

bool FrameDeviceHolder::eventFilter(QObject *watch, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        int nCount = m_pLBtns.count();
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

    return QFrame::eventFilter(watch, event);
}

void FrameDeviceHolder::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect rect = this->rect();
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(QPen(QColor(200, 200, 200), 1));
    //painter.drawRoundedRect(rect, 12, 12);

    event->accept() ;
}

