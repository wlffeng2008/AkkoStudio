#include "FrameKeySetting.h"
#include "ui_FrameKeySetting.h"

#include <QTimer>
#include <QButtonGroup>
#include <QTimer>

#include "ModuleDKSAdjust.h"
#include "ModuleDKSItem.h"
#include "ModuleGeneralMasker.h"
#include "DialogDeviceConnect.h"
#include "ModuleGenKeymapping.h"

#include "DialogVKPicker.h"
#include "DialogFNPicker.h"

FrameKeySetting::FrameKeySetting(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameKeySetting)
{
    ui->setupUi(this);

    {
        QString strStyle(R"(

            QPushButton {
                width: 120px;
                max-height: 32px;
                min-height: 32px;
                border-radius: 16px ;
                font-size:18px;

                padding-left: 25px;
                text-align: left;

                color: #333;
                border: 1px solid transparent;
                background: transparent; }

            QPushButton:checked {
                color: white;
                font-weight:600;
                border: 1px solid #6329B6;
                background: #6329B6; }

            QPushButton:hover {border: 1px solid #6329B6; }

            )") ;

        QLayout *pLayout = ui->frameMode->layout() ;
        pLayout->setSpacing(12) ;
        pLayout->setAlignment(Qt::AlignTop|Qt::AlignHCenter) ;

        QList<QPushButton *>btns = {ui->pushButtonSet1,ui->pushButtonSet2,ui->pushButtonSet3} ;
        QButtonGroup *pBtnGrp = new QButtonGroup(this) ;
        for(int i=0; i<btns.count(); i++)
        {
            QPushButton *btn = btns[i] ;
            btn->setFixedSize(120,24);
            btn->setCheckable(true) ;
            btn->setStyleSheet(strStyle);
            btn->setFocusPolicy(Qt::NoFocus) ;
            btn->setCursor(Qt::PointingHandCursor) ;
            pBtnGrp->addButton(btn,i);
        }
        connect(pBtnGrp,&QButtonGroup::idClicked,this,[=](int id){
            ui->stackedWidget->setCurrentIndex(id) ;
            m_setType = id ;
        });
        ui->pushButtonSet1->click() ;
    }
    {
        QStringList Files={
            "fn.png",
            "kuaijin.png",
            "houtui.png",
            "zanting.png",
            "guanbiyinliang.png",
            "yinliangjia.png",
            "yinliangjian.png",
            "yinyue.png",
            "jisuanqi.png",
            "youjian.png",
            "frame.png",
            "sousuo.png",
            "shouye.png",
            "shuaxin.png",
            "jianpanliangdujia.png",
            "jianpanliangdu.png",
            "fangda.png",
            "suoxiao.png",
            "yuyin.png" };
        //QButtonGroup *pBtnGrp = new QButtonGroup(this) ;
        for(int i=0; i<19; i++)
        {
            QString strName = QString::asprintf("pushButton_F%02d",i+1) ;
            QPushButton *btn = findChild<QPushButton*>(strName) ;
            if(!btn) continue;

            btn->setCheckable(true) ;
            btn->setText("") ;

            QString strStyle=QString(R"(
                QPushButton {
                    icon: url(:/images/macro/fn0/%1);
                    icon-size: 24px 24px;
                    border-radius: 16px ;
                    border: 1px soild #ECECEC;
                    background: #FFFFFF;
                    padding: 2px; }

                QPushButton:checked,pressed {
                    icon: url(:/images/macro/fn1/%2);
                    background-color: #3F3F3F; }

                QPushButton:hover {
                    icon: url(:/images/macro/fn1/%3);
                    background-color: #8F8F8F; }
            )").arg(Files[i],Files[i],Files[i]);

            btn->setStyleSheet(strStyle);
            btn->setFocusPolicy(Qt::NoFocus) ;
            btn->setCursor(Qt::PointingHandCursor) ;
            //pBtnGrp->addButton(btn,i);
        }
    }
    {
        QStringList Values1 ={"8000 hz","4000 hz","2000 hz","1000 hz","500 hz","250 hz","125 hz"} ;
        ui->frameSV1->setValueList(Values1) ;

        QStringList Values2 ={"5 min","15 min","30 min","45 min","60 min"} ;
        ui->frameSV2->setValueList(Values2) ;
        ui->frameSV3->setValueList(Values2) ;

        ui->horizontalSlider->setFixedHeight(40) ;
        connect(ui->horizontalSlider,&QSlider::valueChanged,this,[=](int value){ui->lineEditValue->setText(QString::asprintf("%d ms",value));});

        ui->horizontalSlider->setValue(25) ;

        QTimer *pTMUpdate = new QTimer(this) ;
        connect(pTMUpdate,&QTimer::timeout,this,[=]{
            pTMUpdate->stop();
            QString strTmp = ui->lineEditValue->text().trimmed();
            char szText[100]={0} ;
            strcpy_s(szText,strTmp.toStdString().c_str()) ;
            for(int i=0; i<strlen(szText); i++)
            {
                if(szText[i] == '.' || (szText[i] >= '0' && szText[i] <= '9'))
                    continue ;
                szText[i] = 0 ;
                break;
            }
            strTmp = szText;
            int value = strTmp.toFloat() ;
            if(value>200) value = 200 ;
            if(value<0)   value = 0 ;
            ui->horizontalSlider->setValue(value);
        });

        connect(ui->lineEditValue,&QLineEdit::textEdited,this,[=](const QString&text){
            pTMUpdate->stop() ;
            pTMUpdate->start(300) ;
        });
    }

    {
        ui->labelPress1->installEventFilter(this);
        ui->labelPress2->installEventFilter(this);
        ui->labelPress3->installEventFilter(this);
        ui->labelPress4->installEventFilter(this);
        ui->labelRelease1->installEventFilter(this);
        ui->labelRelease2->installEventFilter(this);
        ui->labelRelease3->installEventFilter(this);
        ui->labelRelease4->installEventFilter(this);
    }

    m_adjust = new ModuleDKSAdjust(this) ;
    m_adjust->setObjectName("DKSAdjust") ;
    m_adjust->setStyleSheet("QFrame#DKSAdjust{background-color:#F6F6F6; border-radius:20px; min-width:200px; min-height:240px;margin:20px;}") ;
    m_adjust->adjustSize();
    m_adjust->update();
    m_adjust->hide();
    connect(m_adjust,&ModuleDKSAdjust::onValueSave,this,[=](const QString&text){
        ui->labelPress1->setText(QString::asprintf("%.2f",text.toFloat()) + " mm") ;
        ui->labelRelease2->setText(QString::asprintf("%.2f",text.toFloat()) + " mm") ;

        ui->labelPress2->setText(QString::asprintf("%.2f",3.9 - text.toFloat()) + " mm") ;
        ui->labelRelease1->setText(QString::asprintf("%.2f",3.9 - text.toFloat()) + " mm") ;
    });

    connect(ui->pushButton_Snap1,&QPushButton::clicked,this,[=]{
        static ModuleGeneralMasker *mask = new ModuleGeneralMasker(nullptr,ui->frameTab2) ;
        mask->setStyleSheet("QDialog { background-color: rgba(255, 255, 255, 0.8); border: none; border-radius: 32px; }");
        mask->show() ;
        m_pMask = mask ;
    });

    connect(ui->tabWidget2,&QTabWidget::currentChanged,this,[=](int index){
        if(index == 3)
        {
        }
    }) ;

    connect(ui->frameDKS1,&ModuleDKSItem::onButtonClicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return ;
        ui->frameDKS1->setText(getKeyString(&VKDlg.m_kd,false));
        //setTo = VKDlg.m_kd;
    });

    connect(ui->frameDKS2,&ModuleDKSItem::onButtonClicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return ;
        ui->frameDKS2->setText(getKeyString(&VKDlg.m_kd,false));
        //setTo = VKDlg.m_kd;
    });

    connect(ui->frameDKS3,&ModuleDKSItem::onButtonClicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return ;
        ui->frameDKS3->setText(getKeyString(&VKDlg.m_kd,false));
        //setTo = VKDlg.m_kd;
    });

    connect(ui->frameDKS4,&ModuleDKSItem::onButtonClicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return ;
        ui->frameDKS4->setText(getKeyString(&VKDlg.m_kd,false));
        //setTo = VKDlg.m_kd;
    });

    connect(ui->frameKeyboard,&ModuleKeyboard::onKeyClicked,this,[=](const QString&text,quint8 hid){
        ui->pushButton_Snap1->setText(text);
        if(m_setType == 0)
        {
            keyData setTo={0};
            if(ui->tabWidgetKey->currentIndex() == 0)
            {
                DialogVKPicker VKDlg(this);
                if(VKDlg.exec() != QDialog::Accepted)
                    return ;
                setTo = VKDlg.m_kd;
            }
            else
            {
                DialogFNPicker FNDlg(this);
                if(FNDlg.exec() != QDialog::Accepted)
                    return;
                setTo = FNDlg.m_kd;
            }

            DialogDeviceConnect *pCnn = DialogDeviceConnect::instance() ;
            pCnn->changeKey(hid,&setTo);

            QString strT1 = getKeyValue(hid);
            QString strT2 = getKeyString(&setTo);
            ui->frameKeyboard->setKeyTip(hid, strT1, strT2);

            if(m_pMask) m_pMask->hide() ;
        }

        if(m_setType == 1)
        {
            int nTab = ui->tabWidget2->currentIndex();
            switch(nTab)
            {
            case 0:
                ui->pushButton_DKS->setText(text);
                break;
            }

        }
    });

    ui->frameKeyboard->setSelectCount(1);



}


bool FrameKeySetting::eventFilter(QObject*watched,QEvent*event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QLabel *labCilck = nullptr ;
        if(watched == ui->labelPress1   || watched == ui->labelPress3  )  labCilck = ui->labelPress1 ;
        //if(watched == ui->labelPress2   || watched == ui->labelPress4  )  labCilck = ui->labelPress2 ;
        //if(watched == ui->labelRelease1 || watched == ui->labelRelease3)  labCilck = ui->labelRelease1 ;
        if(watched == ui->labelRelease2 || watched == ui->labelRelease4)  labCilck = ui->labelRelease2 ;

        if(labCilck)
        {
            m_toAdjust = labCilck;
            QString strValue = labCilck->text().replace("mm","").trimmed();
            m_adjust->setOriginValue(strValue) ;
            ModuleGeneralMasker M(m_adjust,ui->frameTab2);
            M.setStyleSheet("QDialog { background-color: rgba(240, 240, 240, 0.8);  border: none; border-radius: 32px;}");
            M.exec() ;

            return true ;
        }
    }

    return QFrame::eventFilter(watched,event);
}


FrameKeySetting::~FrameKeySetting()
{
    delete ui;
}

void FrameKeySetting::showEvent(QShowEvent *event)
{
    DialogDeviceConnect *pCnn = DialogDeviceConnect::instance() ;
    QByteArray data = pCnn->getMatix(0);
    for(int i=0; i<128; i++)
    {
        quint8 hid = ::getHid(i);
        keyData kd;
        kd.b0 = data[i*4 +0] ;
        kd.b1 = data[i*4 +1] ;
        kd.b2 = data[i*4 +2] ;
        kd.b3 = data[i*4 +3] ;

        if(isKeyChanged(i,&kd))
        {
            QString strT1 = getKeyValue(hid);
            QString strT2 = getKeyString(&kd);
            //if(isKeyDisabled(i))
            if(kd.b0 == 0 && kd.b1 == 0 && kd.b2 == 0 && kd.b3 == 0)
                strT1="DISABLED";
            ui->frameKeyboard->setKeyTip(hid, strT1, strT2);
        }
    }

    //ui->frameKeyboard->keepSpeacial();
}
