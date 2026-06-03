#include "FrameMagic.h"
#include "ui_FrameMagic.h"

#include <QTimer>
#include <QButtonGroup>

#include "keyboardbutton.h"
#include "DialogDeviceConnect.h"

FrameMagic::FrameMagic(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameMagic)
{
    ui->setupUi(this);

    {
        QString strStyle(R"(

            QPushButton {
                width: 120px;
                max-height: 32px;
                min-height: 32px;
                border-radius: 16px;
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

        QLayout *pLayout = ui->frameBLeft->layout();
        pLayout->setSpacing(12);
        pLayout->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

        QList<QPushButton *>btns = {ui->pushButtonSet1,ui->pushButtonSet2,ui->pushButtonSet3,ui->pushButtonSet4};
        QButtonGroup *pBtnGrp = new QButtonGroup(this);
        for(int i=0; i<4; i++)
        {
            QPushButton *btn = btns[i];
            btn->setFixedSize(120,24);
            btn->setCheckable(true);
            btn->setStyleSheet(strStyle);
            btn->setFocusPolicy(Qt::NoFocus);
            btn->setCursor(Qt::PointingHandCursor);
            pBtnGrp->addButton(btn,i);
        }

        DialogDeviceConnect *pCnn = DialogDeviceConnect::instance();

        static QList<quint32> colors = {0x9B9B9B, 0x7969F3, 0xF369A7, 0xC7D827, 0xFF8E32, 0xBF36FF, 0x6B9CFF, 0x2E2EB9, 0x47CA3B, 0x6969F3, 0x2FC1F3, 0x870339, 0x206599, 0xFF6D6B, 0x363636, 0x2258A9, 0x149819};

        static QList<quint32> typeIds = {0,1,2,3,4,5,7,15,24,25,31,63,71,72,95,118};

        static QStringList names = {tr("高特"),tr("磁玉"),tr("磁玉pro"),tr("磁玉gaming"),tr("天王"),tr("万磁王"),tr("机械轴"),tr("凯华轴"),tr("星引力"),tr("炫光"),tr("闪电"),tr("星耀"),tr("矮磁轴","冠泰轴"),tr("星芒磁轴"),tr("白泽轴"),tr("自定义")};

        connect(pBtnGrp,&QButtonGroup::idClicked,this,[=](int id){
            ui->stackedWidget->setCurrentIndex(id);

            if(id == 2)
            {
                for(int i=0; i<128; i++)
                {
                    quint8 hid = ::getHid(i);
                    quint8 type= pCnn->get65Value(0xFC,i);
                    qDebug()<<i << hid << type;
                    //if(type > 6) type = 0;
                    int index = typeIds.indexOf(type);
                    if(index < 0) index = 0;
                    ui->frameKeyboard->setMtColor(hid,colors[index]);
                }
            }
            ui->frameKeyboard->showMtFlag(id == 2);
        });


        connect(ui->buttonGroupMT,&QButtonGroup::idClicked,this,[=](int id){
            qDebug() << "buttonGroupMT" << id;
            QList<quint32> types = {25,31,95,1,2,3,4,5};
            m_mtType = types[abs(id)-2];

            int index = typeIds.indexOf(m_mtType);
            if(index < 0) index = 0;
            m_selColor = colors[index];
        });

        connect(ui->frameKeyboard,&ModuleKeyboard::onKeyClicked,this,[=](const QString&text,quint8 hid){
            int page = ui->stackedWidget->currentIndex();
            if(page == 0)
            {
            }

            if(page == 1)
            {
            }

            if(page == 2)
            {
                QColor color = m_selColor;
                ui->frameKeyboard->setMtColor(hid,color);
                pCnn->send65Cmd(0xFC,hid,m_mtType,true);
            }
        });

        ui->pushButtonSet1->click();
        ui->pushButtonSet4->hide();
        ui->pushButtonMT0->click();
    }

    ui->frameDead->setText(tr("顶部死区"),tr("底部死区"));

    {
        // static QString strStyle1(R"(
        // QPushButton {
        //     border: none;
        //     border-radius: 0px;
        //     color: black;
        //     padding: 0 ;
        //     background-color: #E4E4E4;
        //         min-width:20px;
        //         min-height:20px;
        //         max-width:20px;
        //         max-height:20px;
        //         icon-size: 20px;
        //     }

        //     QPushButton:hover { background-color: #E4E4E4;}

        //     )");
        // ui->pushButtonM1->setStyleSheet(strStyle1);
        // ui->pushButtonM2->setStyleSheet(strStyle1);
        // ui->pushButtonP1->setStyleSheet(strStyle1);
        // ui->pushButtonP2->setStyleSheet(strStyle1);

        ui->horizontalSlider2->setStyleSheet(R"(
        QSlider::sub-page:horizontal { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #C3FFFD, stop:1 #39E1DC);  border-radius: 6px;}
        QSlider::handle:horizontal {
            width: 20px;
            height: 16px;
            margin: -6px 0;
            border-radius: 12px;
            background: white;
            border: 2px solid #5CFFFA ; }
        QSlider::handle:hover { background: #F0F0F0;}
        QSlider::handle:pressed { background: #E0E0E0; border-color: #2D7FDD; }
    )");

        ui->horizontalSlider1->setFixedHeight(40);
        ui->horizontalSlider2->setFixedHeight(40);
        connect(ui->horizontalSlider1,&QSlider::valueChanged,this,[=](int value){ui->lineEditValue1->setText(QString::asprintf("%.03f",value/1000.0));});
        connect(ui->horizontalSlider2,&QSlider::valueChanged,this,[=](int value){ui->lineEditValue2->setText(QString::asprintf("%.03f",2.7-value/1000.0));});

        ui->horizontalSlider1->setValue(2050);
        ui->horizontalSlider2->setValue(1950);

        ui->pushButtonM1->setAutoRepeat(true);
        ui->pushButtonM1->setAutoRepeatInterval(100);
        connect(ui->pushButtonM1,&QPushButton::pressed,this,[=]{
            int value = ui->horizontalSlider1->value() - 5;
            ui->horizontalSlider1->setValue(value);
        });

        ui->pushButtonP1->setAutoRepeat(true);
        ui->pushButtonP1->setAutoRepeatInterval(100) ;
        connect(ui->pushButtonP1,&QPushButton::pressed,this,[=]{
            int value = ui->horizontalSlider1->value() + 5;
            ui->horizontalSlider1->setValue(value);
        });

        QTimer *pTMUpdate1 = new QTimer(this);
        QTimer *pTMUpdate2 = new QTimer(this);

        connect(ui->lineEditValue1,&QLineEdit::textEdited,this,[=](const QString&text){
            pTMUpdate1->stop();
            pTMUpdate1->start(300);
        });
        connect(pTMUpdate1,&QTimer::timeout,this,[=]{
            pTMUpdate1->stop() ;
            QString strTmp = ui->lineEditValue1->text();
            char szText[100]={0} ;
            strcpy_s(szText, strTmp.toStdString().c_str());
            for(int i=0; i<strlen(szText); i++)
            {
                if(szText[i] == '.' || (szText[i] >= '0' && szText[i] <= '9'))
                    continue;
                szText[i] = 0;
                break;
            }
            strTmp = szText;
            int value = strTmp.toFloat() * 1000;
            if(value>2500) value = 2500;
            if(value<200)  value = 200;
            ui->horizontalSlider1->setValue(value);
        });

        ui->pushButtonM2->setAutoRepeat(true);
        ui->pushButtonM2->setAutoRepeatInterval(100);
        connect(ui->pushButtonM2,&QPushButton::pressed,this,[=]{
            int value = ui->horizontalSlider2->value() - 5;
            ui->horizontalSlider2->setValue(value);
        });

        ui->pushButtonP2->setAutoRepeat(true);
        ui->pushButtonP2->setAutoRepeatInterval(100);
        connect(ui->pushButtonP2,&QPushButton::pressed,this,[=]{
            int value = ui->horizontalSlider2->value() + 5;
            ui->horizontalSlider2->setValue(value);
        });

        connect(ui->lineEditValue2,&QLineEdit::textEdited,this,[=](const QString&text){
            pTMUpdate2->stop();
            pTMUpdate2->start(300);
        });
        connect(pTMUpdate2,&QTimer::timeout,this,[=]{
            pTMUpdate2->stop();
            QString strTmp = ui->lineEditValue2->text();
            char szText[100]={0} ;
            strcpy_s(szText,strTmp.toStdString().c_str());
            for(int i=0; i<strlen(szText); i++)
            {
                if(szText[i] == '.' || (szText[i] >= '0' && szText[i] <= '9'))
                    continue;
                szText[i] = 0;
                break;
            }
            strTmp = szText;
            int value = strTmp.toFloat() * 1000;
            if(value>2500) value = 2500;
            if(value<200)  value = 200;
            ui->horizontalSlider2->setValue(value);
        });

        connect(ui->checkBox3,&QCheckBox::clicked,this,[=](bool checked){
            ui->labelRelease->setHidden(!checked);
            ui->frameRelease->setHidden(!checked);
        });

        ui->checkBox3->click();
    }

    ui->frameKeyboard->showMtFlag();
}

bool FrameMagic::eventFilter(QObject *watched, QEvent *event)
{
    return QFrame::eventFilter(watched, event);
}

FrameMagic::~FrameMagic()
{
    delete ui;
}
