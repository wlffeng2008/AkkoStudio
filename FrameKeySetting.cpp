#include "FrameKeySetting.h"
#include "ui_FrameKeySetting.h"

#include <QTimer>
#include <QWheelEvent>
#include <QButtonGroup>

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

    DialogDeviceConnect *pCnn = DialogDeviceConnect::instance();
    {
        QString strStyle(R"(

            QPushButton {
                width: 120px;
                max-height: 32px;
                min-height: 32px;
                border-radius: 17px;
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

        QLayout *pLayout = ui->frameMode->layout();
        pLayout->setSpacing(12);
        pLayout->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

        QList<QPushButton *>btns = {ui->pushButtonSet1,ui->pushButtonSet2,ui->pushButtonSet3};
        QButtonGroup *pBtnGrp = new QButtonGroup(this);
        for(int i=0; i<btns.count(); i++)
        {
            QPushButton *btn = btns[i];
            btn->setFixedSize(120,32);
            btn->setCheckable(true);
            btn->setStyleSheet(strStyle);
            btn->setFocusPolicy(Qt::NoFocus);
            btn->setCursor(Qt::PointingHandCursor);
            pBtnGrp->addButton(btn,i);
        }
        connect(pBtnGrp,&QButtonGroup::idClicked,this,[=](int id){
            ui->stackedWidget->setCurrentIndex(id);
            m_setType = id;
            ui->frameKeyboard->setEnabled(id != 2);
        });
        ui->pushButtonSet1->click();
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
            QString strName = QString::asprintf("pushButton_F%02d",i+1);
            QPushButton *btn = findChild<QPushButton*>(strName);
            if(!btn) continue;

            btn->setCheckable(true);
            btn->setText("");

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
            btn->setFocusPolicy(Qt::NoFocus);
            btn->setCursor(Qt::PointingHandCursor);
            //pBtnGrp->addButton(btn,i);
        }
    }
    {
        QStringList Values1 ={"8000 hz","4000 hz","2000 hz","1000 hz","500 hz","250 hz","125 hz"} ;
        ui->frameSV1->setValueList(Values1) ;

        QStringList Values2 ; //={"5 min","15 min","30 min","45 min","60 min"} ;
        for(int i=0; i<=1800; i++)
            Values2.push_back(QString("%1 min").arg(i));
        ui->frameSV2->setValueList(Values2);
        ui->frameSV3->setValueList(Values2);

        ui->horizontalSlider->setFixedHeight(40);
        connect(ui->horizontalSlider,&QSlider::valueChanged,this,[=](int value){ui->lineEditValue->setText(QString::asprintf("%d ms",value));});

        ui->horizontalSlider->setValue(25);

        QTimer *pTMUpdate = new QTimer(this);
        connect(pTMUpdate,&QTimer::timeout,this,[=]{
            pTMUpdate->stop();
            QString strTmp = ui->lineEditValue->text().trimmed();
            char szText[100]={0} ;
            strcpy_s(szText,strTmp.toStdString().c_str());
            for(int i=0; i<strlen(szText); i++)
            {
                if(szText[i] == '.' || (szText[i] >= '0' && szText[i] <= '9'))
                    continue ;
                szText[i] = 0;
                break;
            }
            strTmp = szText;
            int value = strTmp.toFloat();
            if(value>200) value = 200;
            if(value<0)   value = 0;
            ui->horizontalSlider->setValue(value);
        });

        connect(ui->lineEditValue,&QLineEdit::textEdited,this,[=](const QString&text){
            pTMUpdate->stop();
            pTMUpdate->start(300);
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
        ui->lineEditV1->installEventFilter(this);
        ui->lineEditV2->installEventFilter(this);
        ui->lineEditV3->installEventFilter(this);
    }

    m_adjust = new ModuleDKSAdjust(this);
    m_adjust->setObjectName("DKSAdjust");
    m_adjust->setStyleSheet("QFrame#DKSAdjust{background-color:#F6F6F6; border-radius:20px; min-width:200px; min-height:240px;margin:20px;}");
    m_adjust->adjustSize();
    m_adjust->update();
    m_adjust->hide();
    connect(m_adjust,&ModuleDKSAdjust::onValueSave,this,[=](const QString&text){
        m_DKSLen = text.toFloat();
        ui->labelPress1->setText(QString::asprintf("%.2f mm",m_DKSLen));
        ui->labelRelease2->setText(QString::asprintf("%.2f mm",m_DKSLen));

        //ui->labelPress2->setText(QString::asprintf("%.2f mm",3.9 - text.toFloat()));
        //ui->labelRelease1->setText(QString::asprintf("%.2f mm",3.9 - text.toFloat()));
    });

    connect(ui->pushButton_Snap1,&QPushButton::clicked,this,[=]{

        // static ModuleGeneralMasker *mask = new ModuleGeneralMasker(nullptr,ui->frameTab2) ;
        // mask->setStyleSheet("QDialog { background-color: rgba(255, 255, 255, 0.8); border: none; border-radius: 32px; }");
        // mask->show();
        // m_pMask = mask;

    });

    connect(ui->tabWidget2,&QTabWidget::currentChanged,this,[=](int index){
    }) ;

    // DKS
    connect(ui->frameDKS1,&ModuleDKSItem::onButtonClicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return;
        ui->frameDKS1->setText(getKeyString(&VKDlg.m_kd,false));
        m_DKS1 = VKDlg.m_kd;
    });

    connect(ui->frameDKS2,&ModuleDKSItem::onButtonClicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return;
        ui->frameDKS2->setText(getKeyString(&VKDlg.m_kd,false));
        m_DKS2 = VKDlg.m_kd;
    });

    connect(ui->frameDKS3,&ModuleDKSItem::onButtonClicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return;
        ui->frameDKS3->setText(getKeyString(&VKDlg.m_kd,false));
        m_DKS3 = VKDlg.m_kd;
    });

    connect(ui->frameDKS4,&ModuleDKSItem::onButtonClicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return;
        ui->frameDKS4->setText(getKeyString(&VKDlg.m_kd,false));
        m_DKS4 = VKDlg.m_kd;
    });

    connect(ui->pushButton_OKDKS,&QPushButton::clicked,this,[=]{
        if(m_DKSHid == 0)
            return;
        pCnn->changeKey(m_DKSHid,&m_DKS1,0,false);
        pCnn->changeKey(m_DKSHid,&m_DKS2,1,false);
        pCnn->changeKey(m_DKSHid,&m_DKS3,2,false);
        pCnn->changeKey(m_DKSHid,&m_DKS4,3,true);

        pCnn->send65Cmd(0x07,m_DKSHid,0x02,false);
        pCnn->send65Cmd(0x04,m_DKSHid,m_DKSLen*200,false);
        pCnn->send65Cmd(0x08,m_DKSHid,0x55565B6F,true);
        refresh();
    });

    // MT
    connect(ui->pushButton_MT1,&QPushButton::clicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return;
        ui->pushButton_MT1->setText(getKeyString(&VKDlg.m_kd,false));
        m_MT1 = VKDlg.m_kd;
    });

    connect(ui->pushButton_MT2,&QPushButton::clicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return;
        ui->pushButton_MT2->setText(getKeyString(&VKDlg.m_kd,false));
        m_MT2 = VKDlg.m_kd;
    });

    connect(ui->pushButton_OKMT,&QPushButton::clicked,this,[=]{
        if(m_MTHid == 0)
            return;
        pCnn->changeKey(m_MTHid,&m_MT1,0,false);
        pCnn->changeKey(m_MTHid,&m_MT2,1,true);

        pCnn->send65Cmd(0x07,m_MTHid,0x03,false);
        pCnn->send65Cmd(0x05,m_MTHid,ui->horizontalSlider->value() / 100,true);
        refresh();
    });

    // TGL
    connect(ui->pushButton_TGL1,&QPushButton::clicked,this,[=]{
        DialogVKPicker VKDlg(this);
        if(VKDlg.exec() != QDialog::Accepted)
            return;
        ui->pushButton_TGL1->setText(getKeyString(&VKDlg.m_kd,false));
        m_TGL1 = VKDlg.m_kd;
    });
    connect(ui->radioButton_TGL1,&QRadioButton::clicked,this,[=]{ui->pushButton_OKTGL->click();});
    connect(ui->radioButton_TGL2,&QRadioButton::clicked,this,[=]{ui->pushButton_OKTGL->click();});
    connect(ui->pushButton_OKTGL,&QPushButton::clicked,this,[=]{
        if(m_TGLHid == 0)
            return;
        pCnn->changeKey(m_TGLHid,&m_TGL1,0,true);
        pCnn->send65Cmd(0x07,m_TGLHid,ui->radioButton_TGL1->isChecked()?0x04:0x05,true);
        refresh();
    });

    // Snap
    connect(ui->pushButton_OKSnap,&QPushButton::clicked,this,[=]{
        if(m_SnapHid1 == 0 || m_SnapHid2 == 0)
            return;
        pCnn->send65Cmd(0x07,m_SnapHid1,0x07,false);
        pCnn->send65Cmd(0x07,m_SnapHid2,0x07,false);
        pCnn->send65Cmd(0x09,m_SnapHid1,getIndex(m_SnapHid2),false);
        pCnn->send65Cmd(0x09,m_SnapHid2,getIndex(m_SnapHid1),true);
        refresh();
    });

    connect(ui->frameKeyboard,&ModuleKeyboard::onKeyClicked,this,[=](const QString&text,quint8 hid){

        int index = getIndex(hid);

        if(m_setType == 0)
        {
            keyData setTo = {0};
            if(ui->tabWidgetKey->currentIndex() == 0)
            {
                DialogVKPicker VKDlg(this);
                if(VKDlg.exec() != QDialog::Accepted)
                    return;
                setTo = VKDlg.m_kd;
            }
            else
            {
                DialogFNPicker FNDlg(this);
                if(FNDlg.exec() != QDialog::Accepted)
                    return;
                setTo = FNDlg.m_kd;

                if(FNDlg.m_macro)
                {
                    ModuleMacroManager *pMM = ModuleMacroManager::instance();
                    pCnn->setMacro(hid,setTo.b0,setTo.b1,setTo.b2,pMM->packMacroPack(pMM->getMarcoProject(setTo.b2)));
                    refresh();
                    return;
                }
            }

            pCnn->changeKey(hid,&setTo);

            QString strT1 = getKeyValue(hid);
            QString strT2 = getKeyString(&setTo);
            ui->frameKeyboard->setKeyTip(hid, strT1, strT2);

            if(m_pMask) m_pMask->hide();
        }

        if(m_setType == 1)
        {
            int nTab = ui->tabWidget2->currentIndex();
            quint8 type = pCnn->getKeyType(hid);
            QStringList res = pCnn->getKeyString(hid);

            if(type == 2)
            {
                ui->frameDKS1->setText(res[0]);
                ui->frameDKS2->setText(res[1]);
                ui->frameDKS3->setText(res[2]);
                ui->frameDKS4->setText(res[3]);
                ui->pushButton_DKS->setText(text);

                pCnn->getKeydata(&m_DKS1,index,0);
                pCnn->getKeydata(&m_DKS2,index,1);
                pCnn->getKeydata(&m_DKS3,index,2);
                pCnn->getKeydata(&m_DKS4,index,3);
                m_DKSLen = pCnn->get65Value(0x04,index)/200.0;
                ui->labelPress1->setText(QString::asprintf("%.2f mm", m_DKSLen));
                ui->labelRelease2->setText(QString::asprintf("%.2f mm", m_DKSLen));
            }

            if(type == 3)
            {
                ui->pushButton_MT1->setText(res[0]);
                ui->pushButton_MT2->setText(res[1]);
                ui->pushButton_MT->setText(text);

                pCnn->getKeydata(&m_MT1,index,0);
                pCnn->getKeydata(&m_MT2,index,1);
            }

            if(type == 4 || type == 5)
            {
                ui->pushButton_TGL1->setText(res[0]);
                ui->pushButton_TGL->setText(text);
                ui->radioButton_TGL1->setChecked(type == 4);
                ui->radioButton_TGL2->setChecked(type == 5);

                pCnn->getKeydata(&m_TGL1,index,0);
            }

            if(type == 7)
            {
                m_SnapHid1 = hid;
                m_SnapHid2 = pCnn->getSnapkey(index);
                ui->pushButton_Snap2->setText(::getKeyValue(m_SnapHid2));
            }

            switch(nTab)
            {
            case 0: // DKS
                m_DKSHid = hid;
                ui->pushButton_DKS->setText(text);
                break;

            case 1: // MT
                m_MTHid = hid;
                ui->pushButton_MT->setText(text);
                break;

            case 2: // MT
                m_TGLHid = hid;
                ui->pushButton_TGL->setText(text);
                break;

            case 3: // Snap
                if(ui->radioButton_Snap1->isChecked())
                {
                    m_SnapHid1 = hid;
                    ui->pushButton_Snap1->setText(text);
                }
                else
                {
                    m_SnapHid2 = hid;
                    ui->pushButton_Snap2->setText(text);
                }
                break;
            }
        }
    });

    connect(ui->frameSV1,&ModuleScrollValue::onIndexChanged,this,[=](int index){ pCnn->setReport(index); });
    connect(ui->frameSV2,&ModuleScrollValue::onIndexChanged,this,[=](int index){ pCnn->setSleepTime(index,1); });
    connect(ui->frameSV3,&ModuleScrollValue::onIndexChanged,this,[=](int index){ pCnn->setSleepTime(index,0); });

    connect(ui->horizontalSlider24Deep,&QSlider::valueChanged,this,[=](int value){ ui->lineEditV1->setText(QString("%1").arg(value)); });
    connect(ui->horizontalSliderBTDeep,&QSlider::valueChanged,this,[=](int value){ ui->lineEditV2->setText(QString("%1").arg(value)); });
    connect(ui->horizontalSliderDebounce,&QSlider::valueChanged,this,[=](int value){ ui->lineEditV3->setText(QString("%1").arg(value)); });

    connect(ui->lineEditV1,&QLineEdit::textChanged,this,[=](const QString&text){
        ui->horizontalSlider24Deep->setValue(text.toInt());
        if(!m_bUpdating) pCnn->setSleepTime(text.toInt(),3);
    });
    connect(ui->lineEditV2,&QLineEdit::textChanged,this,[=](const QString&text){
        ui->horizontalSliderBTDeep->setValue(text.toInt());
        if(!m_bUpdating) pCnn->setSleepTime(text.toInt(),2);
    });
    connect(ui->lineEditV3,&QLineEdit::textChanged,this,[=](const QString&text){
        ui->horizontalSliderDebounce->setValue(text.toInt());
        if(!m_bUpdating) pCnn->setDebounce(text.toInt());
    });

    ui->tabWidgetKey->setCurrentIndex(0);
    ui->tabWidget2->setCurrentIndex(0);
    ui->frameKeyboard->setSelectCount(1);
}


bool FrameKeySetting::eventFilter(QObject*watched,QEvent*event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QLabel *labCilck = nullptr;
        if(watched == ui->labelPress1   || watched == ui->labelPress3  )  labCilck = ui->labelPress1;
        //if(watched == ui->labelPress2   || watched == ui->labelPress4  )  labCilck = ui->labelPress2 ;
        //if(watched == ui->labelRelease1 || watched == ui->labelRelease3)  labCilck = ui->labelRelease1 ;
        if(watched == ui->labelRelease2 || watched == ui->labelRelease4)  labCilck = ui->labelRelease2;

        if(labCilck)
        {
            m_toAdjust = labCilck;
            QString strValue = labCilck->text().replace("mm","").trimmed();
            m_adjust->setOriginValue(strValue);

            ModuleGeneralMasker M(m_adjust,ui->frameTab2);
            M.setStyleSheet("QDialog { background-color: rgba(240, 240, 240, 0.8);  border: none; border-radius: 32px;}");
            M.exec();

            return true;
        }
    }

    if (event->type() == QEvent::Wheel)
    {
        if( watched == ui->lineEditV1 ||
            watched == ui->lineEditV2 ||
            watched == ui->lineEditV3 )
        {
            QWheelEvent *pQW = static_cast<QWheelEvent *>(event);
            QPoint angleDelta = pQW->angleDelta();
            QLineEdit *pLE = static_cast<QLineEdit *>(watched);
            int value = pLE->text().toInt();
            if(angleDelta.y()>0)
                value ++ ;
            else
                value -- ;
            pLE->setText(QString("%1").arg(value));
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
    refresh();
}

void FrameKeySetting::refresh()
{
    DialogDeviceConnect *pCnn = DialogDeviceConnect::instance();
    ModuleMacroManager *pMM = ModuleMacroManager::instance();
    QByteArray data = pCnn->getMatix(0);
    if(data.size() < 20)
        return;

    for(int i=0; i<128; i++)
    {
        quint8 hid = ::getHid(i);
        keyData kd;
        pCnn->getKeydata(&kd,i,0);

        quint8 type = pCnn->getKeyType(hid);

        if(isKeyChanged(i,&kd) || type != 0)
        {
            QString strT1 = getKeyValue(hid);
            QString strT2 = getKeyString(&kd);

            QStringList res = pCnn->getKeyString(hid);

            if(type == 2)
            {
                strT2 = QString(tr("动态键程")) + QString("(DKS):\n");
                strT2 += res[0] + QString(":\n");
                strT2 += res[1] + QString(":\n");
                strT2 += res[2] + QString(":\n");
                strT2 += res[3];
            }

            if(type == 3)
            {
                strT2 = QString(tr("按住单击")) + QString("(MT):\n");
                strT2 += res[0] + QString(":\n");
                strT2 += res[1];
            }

            if(type == 4 || type == 5)
            {
                strT2 = QString(tr("切换开关")) + QString("(TGL):\n");
                strT2 += res[0];
            }

            if(type == 7)
            {
                strT2 = QString(tr("SnapKey")) + QString(":\n");
                strT2 += res[0];
            }

            if(kd.b0 == 0x09) // Macro
            {
                MacroProject *prj = pMM->getMarcoProject(kd.b2);
                if(prj)
                {
                    strT2 = QString(tr("宏设置")) + QString(":\n");
                    strT2 += prj->name;
                }
            }

            if(kd.b0 == 0 && kd.b1 == 0 && kd.b2 == 0 && kd.b3 == 0)
                strT1 = QString("DISABLED");
            ui->frameKeyboard->setKeyTip(hid, strT1.trimmed(), strT2.trimmed());
        }
        else
        {
            ui->frameKeyboard->setKeyTip(hid, "", "");
        }
    }
    m_bUpdating=true;

    QTimer::singleShot(500,this,[=]{ m_bUpdating = false; });
    ui->frameSV1->setIndex(pCnn->getReport());
    ui->frameSV2->setIndex(pCnn->getSleepTime()->time24/60);
    ui->frameSV3->setIndex(pCnn->getSleepTime()->timeBt/60);

    ui->lineEditV1->setText(QString("%1").arg(pCnn->getSleepTime()->timeD24/60));
    ui->lineEditV2->setText(QString("%1").arg(pCnn->getSleepTime()->timeDBt/60));
    ui->lineEditV3->setText(QString("%1").arg(pCnn->getDebounce()));
}
