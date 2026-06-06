#include "FrameDeviceHolder.h"
#include "ui_FrameDeviceHolder.h"

#include <QPainter>

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
    ui->labelBatt->setHidden(true);

    DialogDeviceConnect *pCnnt = new DialogDeviceConnect(this);
    ModuleGenKeymapping *pKmap = new ModuleGenKeymapping(this);
    Q_UNUSED(pCnnt)
    Q_UNUSED(pKmap)

    FrameKeySetting *pKS = new FrameKeySetting(this);
    FrameMain *pFM = new FrameMain(this);
    pKS->hide();
    FrameMagic *pMagic = new FrameMagic(this);
    FrameMacro *pMacro = new FrameMacro(this);
    FrameLight *pLight = new FrameLight(this);
    FrameAbout *pAbout = new FrameAbout(this);

    m_pFrames.push_back(pFM);
    m_pFrames.push_back(pKS);
    m_pFrames.push_back(pMagic);
    m_pFrames.push_back(pMacro);
    m_pFrames.push_back(pLight);
    m_pFrames.push_back(pAbout);

    ui->horizontalLayoutBR->addWidget(pFM);
    ui->horizontalLayoutBR->addWidget(pKS);
    ui->horizontalLayoutBR->addWidget(pMagic);
    ui->horizontalLayoutBR->addWidget(pMacro);
    ui->horizontalLayoutBR->addWidget(pLight);
    ui->horizontalLayoutBR->addWidget(pAbout);
    ui->horizontalLayoutBR->setAlignment(Qt::AlignRight);

    for(QFrame*frame:std::as_const(m_pFrames))
        frame->setFixedSize(1124,740);

    m_pLBtns.push_back(ui->label1);
    m_pLBtns.push_back(ui->label2);
    m_pLBtns.push_back(ui->label3);
    m_pLBtns.push_back(ui->label4);
    m_pLBtns.push_back(ui->label5);
    m_pLBtns.push_back(ui->label6);
    m_pLBtns.push_back(ui->label7);
    m_pLBtns.push_back(ui->label8);

    int index = 0;
    static QStringList images={"shouye","jianweishezhi","cizhoushezhi","hongshezhi","dengxiaoshezhi","guanyu","",""};
    foreach (SuperLabel *label, m_pLBtns) {
        label->setCursor(Qt::PointingHandCursor);
        label->installEventFilter(this);
        label->setAlignment(Qt::AlignCenter);
        label->setOwnSheet("QLabel { background-color: #6329B6; border-radius: 16px;}","QLabel { background-color: transparent; }");
        QString strImg = images[index++] + ".png";
        label->setImages(QString(":/images/leftbtns/1/") + strImg,QString(":/images/leftbtns/0/") + strImg);
    }
    clickLabel(ui->label1);

    ui->label6->setHidden(true);
    ui->label7->setHidden(true);
    m_pLBtns[6]->setImages(QString(":/images/User.png"),QString(":/images/User.png"));
    m_pLBtns[7]->setImages(QString(":/images/btn-return.png"),QString(":/images/btn-return.png"));

    connect(ui->pushButtonFixline,&QPushButton::clicked,this,[=]{
        LinearFixing1 T("","",this->parentWidget()->parentWidget()->parentWidget()->parentWidget());
        T.exec();
    });

    connect(ui->pushButtonReset,&QPushButton::clicked,this,[=]{
        DialogDeviceConnect::instance()->reset();
    });

    ui->pushButtonPairing->hide();
    connect(ui->pushButtonPairing,&QPushButton::clicked,this,[=]{
        ModuleLinear *pTest = new ModuleLinear(this);
        pTest->setObjectName("TestLinear");
        pTest->layout()->setContentsMargins(20,20,20,20);
        pTest->setStyleSheet("QFrame#TestLinear{background-color:white;min-height:320px; border-radius:24px;}");
        ModuleGeneralMasker T(pTest, this);
        T.exec();
    });

    {
        QString strStyle(R"(
            QPushButton {
                border-radius: 16px;

                outline: none;
                width: 80px;
                height: 30px;

                color: black;
                border: 1px solid #ECECEC;
                background: #ECECEC; }

            QPushButton:checked {
                color: white;
                border: 1px solid #6329B6;
                background: #6329B6; }
            QPushButton:hover { border: 1px solid #6329B6; }
            )");
        ui->pushButtonLayer0->setStyleSheet(strStyle);
        ui->pushButtonLayer1->setStyleSheet(strStyle);
        ui->pushButtonLayer2->setStyleSheet(strStyle);
        ui->pushButtonLayer3->setStyleSheet(strStyle);
    }

    connect(ui->buttonGroupLayer,&QButtonGroup::idClicked,this,[=](int clikedId){
        // qDebug() << "buttonGroupLayer" << clikedId;
        switch (clikedId) {
        case -2: pCnnt->setProfile(0); break;
        case -3: pCnnt->setProfile(1); break;
        case -4: pCnnt->setProfile(2); break;
        case -5: pCnnt->setProfile(3); break;
        default: break;
        }
    });

    connect(pCnnt,&DialogDeviceConnect::onReadDone,this,[=]{
        pKS->reshowTip();
        qDebug() << "DialogDeviceConnect::onReadDone";
    });

    ui->pushButtonLayer0->installEventFilter(this);
    ui->pushButtonLayer1->installEventFilter(this);
    ui->pushButtonLayer2->installEventFilter(this);
    ui->pushButtonLayer3->installEventFilter(this);
}

FrameDeviceHolder::~FrameDeviceHolder()
{
    delete ui;
}

void FrameDeviceHolder::setDevice(void *device,const QString&strImage,const QString&strName)
{
    m_device = device;
    FrameMain *pFM = (FrameMain *)m_pFrames[0];
    pFM->setDeviceImage(strImage);
    ui->labelName->setText(strName);
}

void FrameDeviceHolder::updateLayer(int layer)
{
    switch (layer) {
    case 0: ui->pushButtonLayer0->click(); break;
    case 1: ui->pushButtonLayer1->click(); break;
    case 2: ui->pushButtonLayer2->click(); break;
    case 3: ui->pushButtonLayer3->click(); break;
    default: break;
    }
}

void FrameDeviceHolder::updateBattery(void *device,const QString&battImg,const QString&typeImg,const QString&tip,const QString&qss)
{
    if(m_device != device)
        return;

    ui->labelBatt->setHidden(battImg.isEmpty());
    ui->labelBatt->setPixmap(QPixmap(battImg));
    ui->labelBatt->setStyleSheet(qss);
    ui->labelBatt->setToolTip(tip);
    ui->labelConn->setPixmap(QPixmap(typeImg));
}

void FrameDeviceHolder::changeEvent(QEvent *pEvt)
{
    if(pEvt->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        ui->label1->updateToolTip(tr("首页"));
        ui->label2->updateToolTip(tr("键盘设置"));
        ui->label3->updateToolTip(tr("磁轴设置"));
        ui->label4->updateToolTip(tr("宏设置"));
        ui->label5->updateToolTip(tr("灯光设置"));
        ui->label6->updateToolTip(tr("关于"));
        ui->label7->updateToolTip(tr("用户"));
        ui->label8->updateToolTip(tr("返回"));
    }

    QFrame::changeEvent(pEvt);
}


void FrameDeviceHolder::clickLabel(QLabel *label, int index)
{
    if(index == 6 || index == 7)
    {
        emit onReturn();
        return;
    }

    ((SuperLabel *)label)->setHold();

    foreach (QFrame *pFM, m_pFrames) {
        pFM->hide();
    }

    m_pFrames[index]->show();

    ui->horizontalFrame->setVisible(index == 1);
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
                clickLabel(label,i);
                break;
            }
        }

        if(watch == ui->pushButtonLayer0) updateLayer(0);
        if(watch == ui->pushButtonLayer1) updateLayer(1);
        if(watch == ui->pushButtonLayer2) updateLayer(2);
        if(watch == ui->pushButtonLayer3) updateLayer(3);
    }

    return QFrame::eventFilter(watch, event);
}


