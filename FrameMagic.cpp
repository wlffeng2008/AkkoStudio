#include "FrameMagic.h"
#include "ui_FrameMagic.h"

#include "DialogDeviceConnect.h"

#include <QTimer>
#include <QButtonGroup>
#include <QPainter>
#include <QPainterPath>


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

        QList<QPushButton *>btns = {ui->pushButtonSet0,ui->pushButtonSet1,ui->pushButtonSet2,ui->pushButtonSet3,ui->pushButtonSet4};
        QButtonGroup *pBtnGrp = new QButtonGroup(this);
        for(int i=0; i<5; i++)
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

        static QList<quint32> colors = {0x9B9B9B, 0x7969F3, 0xF369A7, 0xC7D827, 0xFF8E32, 0xFF6BF3, 0x6B9CFF, 0x2E2EB9, 0x6969F3, 0x2FC1FF, 0x870339, 0x206599, 0xFF6D6B, 0x363636, 0x2258A9, 0x149819};

        static QList<quint32> typeIds = {0,1,2,3,4,5,7,15,24,25,31,63,71,72,95,118};

        static QStringList names = {tr("高特"),tr("磁玉"),tr("磁玉pro"),tr("磁玉gaming"),tr("天王"),tr("万磁王"),tr("机械轴"),tr("凯华轴"),tr("星引力"),tr("炫光"),tr("闪电"),tr("星耀"),tr("矮磁轴","冠泰轴"),tr("星芒磁轴"),tr("白泽轴"),tr("自定义")};

        connect(pBtnGrp,&QButtonGroup::idClicked,this,[=](int id){
            ui->stackedWidget->setCurrentIndex(id);
            ui->frameKeyboard->selectAll(false);
            ui->frameKeyboard->setEnabled( !(id == 3 || id == 4) );

            if(id == 2)
            {
                for(int i=0; i<128; i++)
                {
                    quint8 hid = ::getHid(i);
                    quint8 type= pCnn->get65Value(0xFC,i);
                    int index = typeIds.indexOf(type);
                    if(index < 0) index = 0;
                    //qDebug()<<i << hid << type << Qt::hex << colors[index] << index;
                    ui->frameKeyboard->setMtColor(hid,colors[index]);
                }
            }

            ui->frameKeyboard->showMtFlag(id == 2);
        });


        connect(ui->buttonGroupMT,&QButtonGroup::idClicked,this,[=](int id){
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

        ui->pushButtonSet0->click();
        ui->pushButtonMT0->click();
    }

    ui->frameDead->setText(tr("顶部死区"),tr("底部死区"));

    {
        ui->horizontalSlider2->setStyleSheet(R"(
        QSlider::sub-page:horizontal { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #C3FFFD, stop:1 #39E1DC);  border-radius: 6px;}
        QSlider::sub-page:horizontal:disabled { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 white, stop:1 #B3B3B3);  border-radius: 6px; }
        QSlider::handle:horizontal:disabled { border: 2px solid #B3B3B3; }
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

        connect(ui->checkBoxFullRT,&QCheckBox::clicked,this,[=](bool checked){
            ui->frame0->setEnabled(checked);
            ui->frame1->setEnabled(checked);
            ui->frame2->setEnabled(false);

            ui->frameLinear->enablePannel(false,false);

            ui->checkBoxRTPress->setChecked(false);
            ui->checkBoxUnpress->setChecked(false);
            ui->checkBoxUnpress->setEnabled(!checked);
        });

        connect(ui->checkBoxRTPress,&QCheckBox::clicked,this,[=](bool checked){
            ui->frame2->setEnabled(checked);
        });

        connect(ui->checkBoxUnpress,&QCheckBox::clicked,[=](bool checked){
            ui->frameLinear->enablePannel(false,checked);
        });

        ui->checkBoxRTPress->click();
        ui->checkBoxFullRT->click();

        ui->frameLinear->enablePannel(false,false);
    }

    {
        DialogDeviceConnect *pCnn = DialogDeviceConnect::instance();
        connect(pCnn,&DialogDeviceConnect::onReadDone,this,[=]{
            ui->checkBoxTouch->setChecked(pCnn->getKBOption(2));
            ui->labelTunch->setText(ui->checkBoxTouch->isChecked() ? tr("已开启") : tr("已关闭"));

            quint8 opt3 = pCnn->getKBOption(3);
            ui->radioButtonStab0->setChecked(opt3 == 0);
            ui->radioButtonStab1->setChecked(opt3 == 1);
            ui->radioButtonStab2->setChecked(opt3 == 2);
            ui->radioButtonStab3->setChecked(opt3 == 3);
            ui->radioButtonStab4->setChecked(opt3 == 4);
        });

        connect(ui->checkBoxTouch,&QCheckBox::clicked,this,[=](bool checked){
            pCnn->setKBOption(2, checked ? 1 : 0);
            ui->labelTunch->setText(checked ? tr("已开启") : tr("已关闭"));
        });

        connect(ui->buttonGroupStab,&QButtonGroup::idClicked,this,[=](int id){
            quint8 opt3 = abs(id)-2;
            pCnn->setKBOption(3, opt3);
        });

        connect(ui->frameKeyboard,&ModuleKeyboard::onSelect,this,[=]{
            QList<quint8>hids;
            ui->frameKeyboard->getSelected(hids);
            ui->labelSelectKey->setText(QString(tr("已选择按键数量")) + QString(": %1").arg(hids.count()));
        });

        connect(ui->pushButtonSetDeathzone,&QPushButton::clicked,this,[=]{
            QList<quint8>hids;
            ui->frameKeyboard->getSelected(hids);
            int count=hids.count();
            if(count)
            {
                quint32 v0 = ui->frameLinear->getValue() * 200;
                quint32 v1 = ui->frameLinear->getValue(false) * 200;
                for(int i=0; i<count; i++)
                {
                    quint8 index = getHid(hids[i]);
                    pCnn->send65Cmd(0x06,index,v0,false);
                    pCnn->send65Cmd(0x06,index,v1,i == count-1);
                }
            }
        });

        connect(ui->checkBoxKeytest0,&QCheckBox::clicked,this,[=](bool checked){
        });

        connect(ui->checkBoxKeytest1,&QCheckBox::clicked,this,[=](bool checked){
        });
    }

    ui->labelPress->installEventFilter(this);

    ui->frameKeyboard->showMtFlag();
    srand(time(nullptr));
    QTimer *pTMset = new QTimer(this);
    connect(pTMset,&QTimer::timeout,this,[=]{
        if(ui->checkBoxKeytest0->isChecked())
            ui->labelPress->update();
    });
    pTMset->start(100);
}

bool FrameMagic::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Paint)
    {
        if(watched == ui->labelPress)
        {
            QPainter painter(ui->labelPress);
            painter.setRenderHints(QPainter::Antialiasing);

            painter.setPen(QPen(Qt::blue,2));
            QRect rect = ui->labelPress->rect().adjusted(4,4,-4,-4);
            //painter.drawRoundedRect(rect,20,20);

            painter.setRenderHints(QPainter::Antialiasing,false);
            QFont font = painter.font();
            font.setPixelSize(8);
            painter.setFont(font);
            painter.setPen(QPen(Qt::gray,1));
            float step = rect.height()/34.0;
            for(int i=0; i<35; i++)
            {
                int offset = (i%5 == 0 ? 10 : 5);
                painter.drawLine(QPoint(rect.right() - 20, rect.top() + i*step),QPoint(rect.right() - 20 + offset, rect.top() + i*step));
                painter.drawLine(QPoint(rect.left() + 40, rect.top() + i*step),QPoint(rect.left() + 40 - offset, rect.top() + i*step));

                if(i%5 == 0)
                painter.drawText(QPoint(rect.left()+2, rect.top() + i*step + 4),QString::asprintf("%.2f",i/10.0));
            }

            QRect sub = ui->labelPress->rect().adjusted(50,4,-30,-4);
            QPainterPath path;
            path.addRoundedRect(sub,10,10);
            painter.setClipPath(path);
            painter.setRenderHints(QPainter::Antialiasing);

            painter.fillRect(sub,Qt::gray);
            //painter.drawRoundedRect(sub,4,4);
            painter.fillRect(sub.adjusted(0,sub.height()-20,0,0),Qt::black);
            painter.fillRect(sub.adjusted(0,0,0,  -rand()%(sub.height()-20)),0x6329B6);

        }
    }
    return QFrame::eventFilter(watched, event);
}

FrameMagic::~FrameMagic()
{
    delete ui;
}
