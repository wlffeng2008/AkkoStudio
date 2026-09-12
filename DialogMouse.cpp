#include "DialogMouse.h"
#include "ui_DialogMouse.h"

#include <QLineEdit>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QModelIndex>
#include "colorpicker.h"


void SetLineEditTipColor(QLineEdit *edit,const QColor&color){
    QImage image(edit->size(),QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect(0,0,image.rect().height(),image.rect().height());
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(rect.adjusted(2,2,-2,-2),8,8);

    QString strFile = QApplication::applicationDirPath() + "/images/" + edit->objectName() + ".png";
    image.save(strFile);

    QString strSheet = QString(R"(
    QLineEdit {
        padding-left: %1px;
        padding-top:4px;
        padding-bottom:4px;
        border:1px solid transparent;
        border-radius:4px;
        background-image: url(%2);
        background-repeat: no-repeat;
        background-color: transparent;
        color: white;
        font-size:14px;
    }

    QLineEdit:focus { border:1px solid #2196F3; }

)").arg(rect.height()).arg(strFile);

    edit->setStyleSheet(strSheet);
}


void SetButtonTipColor(QPushButton *button,const QColor&color){
    QImage image(button->size(),QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect(0,0,image.rect().height(),image.rect().height());
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(rect.adjusted(4,4,-6,-6),8,8);

    QString strFile = QApplication::applicationDirPath() + "/images/" + button->objectName() + ".png";
    image.save(strFile);

    QString strSheet = QString(R"(
    QPushButton {
        padding-left: %1px;

        border:1px solid transparent;
        border-radius:8px;
        background-image: url(%2);
        background-repeat: no-repeat;
        background-color: transparent;
        color: white;
        min-height:32px;
        font-size:16px;
        font-weight:500;
        text-align:left;
    }

    QPushButton:checked { border:1px solid %3; }

)").arg(rect.height() + 8).arg(strFile).arg(color.name());

    button->setStyleSheet(strSheet);
}

void SetButtonBackground(QPushButton *button,bool left=true,const QColor&color=QColor("#6329B6")){
    QImage image(button->size(),QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(button->font());
    QRect rect(0,0,image.rect().height(),image.rect().height());

    QString text = button->text() ;
    QRect destRc0 = rect.adjusted(4,4,-4,-4);
    QRect textRc = image.rect().adjusted(rect.height()+10,1,-2,-2);

    if(!left)
    {
        int w = destRc0.width();
        int p = image.width() - w - 2;
        destRc0.setLeft(p);
        destRc0.setWidth(w);

        textRc = image.rect().adjusted(3,1,-rect.height()-10,-2);
    }
    QRect destRc1 = destRc0.adjusted(4,4,-4,-4);
    QString strFile0 = QApplication::applicationDirPath() + QString("/images/back0-") + button->objectName() + QString(".png");
    QString strFile1 = QApplication::applicationDirPath() + QString("/images/back1-") + button->objectName() + QString(".png");

    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(destRc1,destRc1.width()/2,destRc1.width()/2);
    painter.setPen(QPen(color,3));
    painter.setBrush(Qt::NoBrush);
    painter.drawArc(destRc0,0,360*16);

    painter.setPen(Qt::white);
    painter.setBrush(color);
    painter.drawRoundedRect(textRc,12,12);
    painter.drawText(textRc,text,QTextOption(Qt::AlignCenter));
    image.save(strFile1);

    image.fill(Qt::transparent);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.drawRoundedRect(destRc1,destRc1.width()/2,destRc1.width()/2);
    painter.setPen(QPen(Qt::gray,3));
    painter.setBrush(Qt::NoBrush);
    painter.drawArc(destRc0,0,360*16);

    painter.setPen(Qt::white);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(textRc,12,12);
    painter.drawText(textRc,text,QTextOption(Qt::AlignCenter));

    image.save(strFile0);

    QString strSheet = QString(R"(
    QPushButton {
        border:1px solid transparent;
        background-color: transparent;
        background-image: url(%3);
        background-repeat: no-repeat;
        min-height:32px;
        min-width:120px;
    }

    QPushButton:hover { background-image: url(%4); }
    QPushButton:checked { background-image: url(%5); }

)").arg(strFile0).arg(strFile1).arg(strFile1);

    button->setText(" ");
    button->setStyleSheet(strSheet);
}



#include "infortechDevice.h"
#pragma comment(lib,"infortechSdk.lib")

// SDK 单例
InfortechDevice* serviceDevice = nullptr;

// 回调函数
auto deviceStateChange(const InfortechDef::DevMsg& msg) {
    switch (msg.type) {
    case InfortechDef::MsgType::DeviceDisconnect:
        std::cout << "DeviceDisconnect" << std::endl;
        break;

    // 鼠标配对
    case InfortechDef::MsgType::PairFailed:
        std::cout << "PairFailed" << std::endl;
        break;

    case InfortechDef::MsgType::PairTimeout:
        std::cout << "PairTimeout" << std::endl;
        break;

    case InfortechDef::MsgType::PairSucceed:
        std::cout << "PairSucceed" << std::endl;
        break;

    // 固件升级
    case InfortechDef::MsgType::fwUpgradeFailed:
        std::cout << "fwUpgradeFailed" << std::endl;
        break;

    case InfortechDef::MsgType::fwUpgradeProgress:
        std::cout << "fwUpgradeProgress: " << (int)msg.inf->getFwUpProgress()->progress << std::endl;
        break;

    case InfortechDef::MsgType::fwUpgradeSucceed:
        std::cout << "fwUpgradeSucceed" << std::endl;
        break;

    // 设备状态上报
    case InfortechDef::MsgType::DpiChange:
        std::cout << "DpiChange" << std::endl;
        break;

    case InfortechDef::MsgType::RateChange:
        std::cout << "RateChange" << std::endl;
        break;

    case InfortechDef::MsgType::ConfChange:
        std::cout << "ConfChange" << std::endl;
        break;

    case InfortechDef::MsgType::BatteryChange:
        std::cout << "BatteryChange" << std::endl;
        break;

    case InfortechDef::MsgType::ConnectStateChange:
        std::cout << "ConnectStateChange" << std::endl;
        break;

    case InfortechDef::MsgType::TriggerPosChange:
        std::cout << "TriggerPosChange" << std::endl;
        break;

    case InfortechDef::MsgType::CalibrateChange:
        std::cout << "CalibrateChange" << std::endl;
        break;

    default:
        break;
    }
};

DialogMouse::DialogMouse(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogMouse)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::Tool | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);

    ui->stackedWidget->setCurrentIndex(0);
    {
        serviceDevice = InfortechDevice::getInstance();
        serviceDevice->openLog(false);


        // 设置设备信息
        //serviceDevice->addMouseInf(0x1A86, 0x8501);
        //serviceDevice->addDongleInf(0x1A86, 0xD810);
        //serviceDevice->addMouseBootInf(0x1A86, 0x85F1);
        //serviceDevice->addDongleBootInf(0x1A86, 0xD8E0);

        std::vector<InfortechDef::DeviceItem> allDevices = {

            { 0x38EE, 0x0021, 0, 0, 0 },
            { 0x38EE, 0x0047, 0, 0, 0 },
            { 0x38EE, 0x0048, 0, 0, 0 },
            { 0x1A86, 0x8501, 0x85F1, 0xD810, 0xD8E0 },
            { 0x1A86, 0x8502, 0x85F2, 0xD811, 0xD8E1 },
            { 0x1A86, 0x1100, 0x11F0, 0xD100, 0xD1F0 }
        };
        serviceDevice->setSupportDeviceSet(allDevices);

        // 注册通知函数
        serviceDevice->regDevChangeCall(deviceStateChange);

        auto deviceSet = serviceDevice->getDevices();
        qDebug() << deviceSet.size();
    }

    connect(ui->buttonGroup,&QButtonGroup::idClicked,this,[=](int id){
        ui->labelSubTitle->setText(ui->buttonGroup->button(id)->text());
        ui->stackedWidget->setCurrentIndex(abs(id)-2);
    });

    connect(ui->pushButtonClose,&QPushButton::clicked,this,[=]{ this->hide(); });
    connect(ui->pushButtonBack,&QPushButton::clicked,this,[=]{ this->hide(); });

    {
        ui->frameKeyShow->hide();
        ui->frameMouseHold->setFixedWidth(540);

        connect(ui->buttonGroupKeySet,&QButtonGroup::idClicked,this,[=](int id){
            bool bToHide = !ui->frameKeyShow->isHidden();
            ui->frameKeyShow->setHidden(bToHide);
            ui->frameLeft->setHidden(!bToHide);
            QPushButton *pBtn = static_cast<QPushButton *>(ui->buttonGroupKeySet->button(id));
            //qDebug() << pBtn;
        });

        ui->stackedWidgetPickKey->setCurrentIndex(0);
        connect(ui->buttonGroupKeyType,&QButtonGroup::idClicked,this,[=](int id){
            int index = abs(id)-2;
            ui->stackedWidgetPickKey->setCurrentIndex(index);
            QPushButton *pBtn = static_cast<QPushButton *>(ui->buttonGroupKeyType->button(id));
            //qDebug() << pBtn;
        });

        QList<QAbstractButton *> btns  = ui->buttonGroupSuper->buttons();
        foreach (QAbstractButton* btn, btns) {
            btn->setCheckable(true);
            btn->setAutoExclusive(true);
        }

        connect(ui->buttonGroupSuper,&QButtonGroup::idClicked,this,[=](int id){
            int index = abs(id)-2;
            QPushButton *pBtn = static_cast<QPushButton *>(ui->buttonGroupSuper->button(id));
            //qDebug() << pBtn << pBtn->text();
        });
    }

    {
        SetButtonTipColor(ui->pushButtonX0,Qt::red);
        SetButtonTipColor(ui->pushButtonX1,QColor("orange"));
        SetButtonTipColor(ui->pushButtonX2,Qt::yellow);
        SetButtonTipColor(ui->pushButtonX3,Qt::green);
        SetButtonTipColor(ui->pushButtonX4,Qt::cyan);
        SetButtonTipColor(ui->pushButtonX5,Qt::blue);
        SetButtonTipColor(ui->pushButtonX6,QColor("violet"));
        SetButtonTipColor(ui->pushButtonX7,Qt::white);

        SetButtonTipColor(ui->pushButtonY0,Qt::red);
        SetButtonTipColor(ui->pushButtonY1,QColor("orange"));
        SetButtonTipColor(ui->pushButtonY2,Qt::yellow);
        SetButtonTipColor(ui->pushButtonY3,Qt::green);
        SetButtonTipColor(ui->pushButtonY4,Qt::cyan);
        SetButtonTipColor(ui->pushButtonY5,Qt::blue);
        SetButtonTipColor(ui->pushButtonY6,QColor("violet"));
        SetButtonTipColor(ui->pushButtonY7,Qt::white);

        static QPushButton *btnsX[] = {
            ui->pushButtonX0,
            ui->pushButtonX1,
            ui->pushButtonX2,
            ui->pushButtonX3,
            ui->pushButtonX4,
            ui->pushButtonX5,
            ui->pushButtonX6,
            ui->pushButtonX7
        };

        static QPushButton *btnsY[] = {
            ui->pushButtonY0,
            ui->pushButtonY1,
            ui->pushButtonY2,
            ui->pushButtonY3,
            ui->pushButtonY4,
            ui->pushButtonY5,
            ui->pushButtonY6,
            ui->pushButtonY7
        };

        static int valueA[]={400,800,1200,1600,3200,6400,10000,30000};
        static int valueX[]={400,800,1200,1600,3200,6400,10000,30000};
        static int valueY[]={400,800,1200,1600,3200,6400,10000,30000};
        static int selectA = 0 ;
        static int selectX = 0 ;
        static int selectY = 0 ;

        ui->horizontalSliderX->setClipStep(50);
        ui->horizontalSliderY->setClipStep(50);
        ui->lineEditValueX->setFixedHeight(32);
        ui->lineEditValueY->setFixedHeight(32);

        // ui->checkBoxDoubleSet->click();

        connect(ui->checkBoxDoubleSet,&QCheckBox::clicked,this,[=](bool checked){
            ui->label_X->setVisible(checked);
            ui->label_Y->setVisible(checked);
            ui->frameYDPI->setVisible(checked);

            for(int i=0; i<8; i++)
            {
                btnsX[i]->setText(QString("%1").arg(ui->checkBoxDoubleSet->isChecked()?valueX[i]:valueA[i]));
                btnsY[i]->setText(QString("%1").arg(valueY[i]));
            }
            ui->horizontalSliderX->setValue(ui->checkBoxDoubleSet->isChecked()?valueX[selectX]:valueA[selectA]);
            ui->horizontalSliderY->setValue(valueY[selectY]);
        });

        connect(ui->lineEditValueX,&QLineEdit::textChanged,this,[=](const QString&text){
            ui->horizontalSliderX->setValue(text.toInt());
        });


        connect(ui->lineEditValueY,&QLineEdit::textChanged,this,[=](const QString&text){
            ui->horizontalSliderY->setValue(text.toInt());
        });


        connect(ui->buttonGroupX,&QButtonGroup::idClicked,this,[=](int id){
            selectA = abs(id)-2;
            selectX = abs(id)-2;
            ui->horizontalSliderX->setValue(ui->checkBoxDoubleSet->isChecked()?valueX[selectX]:valueA[selectA]);
        });

        connect(ui->horizontalSliderX,&QSlider::valueChanged,this,[=](int value){
            ui->lineEditValueX->setText(QString("%1").arg(value));
            if(ui->checkBoxDoubleSet->isChecked())
                valueX[selectX] = value;
            else
                valueA[selectA] = value;

            btnsX[selectX]->setText(QString("%1").arg(value));
        });

        connect(ui->buttonGroupY,&QButtonGroup::idClicked,this,[=](int id){
            selectY = abs(id)-2;
            ui->horizontalSliderY->setValue(valueY[selectY]);
        });

        connect(ui->horizontalSliderY,&QSlider::valueChanged,this,[=](int value){
            ui->lineEditValueY->setText(QString("%1").arg(value));
            valueY[selectY] = value;
            btnsY[selectY]->setText(QString("%1").arg(value));
        });

    }

    {
        connect(ui->horizontalSlider02,&QSlider::valueChanged,this,[=](int value){
        });

        connect(ui->pushButtonAngleM,&QPushButton::clicked,this,[=]{
            m_nowAngle--;
            if(m_nowAngle < -30) m_nowAngle = -30;
            update();
        });

        connect(ui->pushButtonAngleP,&QPushButton::clicked,this,[=]{
            m_nowAngle++;
            if(m_nowAngle >  30) m_nowAngle = 30;
            update();
        });

        ui->horizontalSlider00->setShowMark();
        ui->horizontalSlider01->setShowMark();
        ui->horizontalSlider02->setShowMark();
    }

    ui->labelAngleShow->setStyleSheet("QLabel{background-color:transparent;}");
    ui->labelAngleShow->installEventFilter(this);


    {
        connect(ui->horizontalSliderLumi,&QSlider::valueChanged,this,[=](int value){
            ui->spinBoxLumi->setValue(value);
        });

        connect(ui->spinBoxLumi,&QSpinBox::valueChanged,this,[=](int value){
            ui->horizontalSliderLumi->setValue(value);
        });

        QStringList colors = {"#73F2FD","#E9381D","#71FB48","#FDFF4E","#EA36B0","#C6FD4B","#51A4F9","#D1307D","#6A8B28","#BC92BC"};
        QList<QAbstractButton*> btns = ui->buttonGroupClr->buttons();
        for(int i=0; i<10; i++)
        {
            btns[i]->setStyleSheet(
                QString(R"(
                QPushButton{
                    border-radius:6px;
                    height:32px;
                    width:32px;
                    background-color: %1;
                    border: 2px solid transparent;
                }
                QPushButton:checked{border: 2px solid red;}
            )").arg(colors[i]));

            btns[i]->setFixedSize(32,32);
        }

        static QPushButton *pColorBrn = nullptr; // ui->pushButtonC0;
        connect(ui->buttonGroupClr,&QButtonGroup::idClicked,this,[=](int id){
            pColorBrn = static_cast<QPushButton *>(ui->buttonGroupClr->button(id));
        });
        connect(ui->widgetColor,&ColorPicker::colorChanged,this,[=](const QColor&color){
            if(!pColorBrn) return;
            pColorBrn->setStyleSheet(QString(R"(  QPushButton{ background-color: %1; } )").arg(color.name()));
        });

        connect(ui->buttonGroupLMode,&QButtonGroup::idClicked,this,[=](int id){
            int index = abs(id) - 2;
            qDebug() << "Light Mode: " << index;
        });

        connect(ui->buttonGroupLSpeed,&QButtonGroup::idClicked,this,[=](int id){
            int index = abs(id) - 2;
            qDebug() << "Light Speed: " << index;
        });
    }
    {

        m_pModel = new QStandardItemModel(this);
        m_pModel->setHorizontalHeaderLabels(QString("名称,类型,名称,类型,名称,类型,名称,类型").split(','));
        ui->tableViewMContent->setModel(m_pModel);

        QHeaderView *pHeader = ui->tableViewMContent->horizontalHeader();
        pHeader->setSectionResizeMode(QHeaderView::Stretch);

        for(int m=0;m<20;m++)
        {
            QList<QStandardItem*>test;
            for(int i=0; i<8; i++)
            {
                QStandardItem *item = new QStandardItem("test");
                test.push_back(item);
            }
            m_pModel->appendRow(test);
        }

        //------------------------------------

        m_pMList = new QStandardItemModel(this);
        m_pMList->setHorizontalHeaderLabels(QString("名称,类型,名称").split(','));

        ui->tableViewMList->setModel(m_pMList);

        ui->tableViewMList->setShowGrid(false);
        ui->tableViewMList->verticalHeader()->hide();

        pHeader = ui->tableViewMList->horizontalHeader();
        pHeader->setSectionResizeMode(QHeaderView::Stretch);
        pHeader->setSectionResizeMode(1,QHeaderView::Fixed);
        pHeader->setSectionResizeMode(2,QHeaderView::Fixed);
        pHeader->resizeSection(1,36);
        pHeader->resizeSection(2,36);
        pHeader->hide();

        m_MLDele1 = new ImageDelegate(":/images/mouse/edit-0.png",this);
        m_MLDele2 = new ImageDelegate(":/images/mouse/delete-0.png",this);

        QFont font = ui->tableViewMList->font();
        font.setBold(true);
        font.setPixelSize(16);

        ui->tableViewMList->setItemDelegateForColumn(1, m_MLDele1);
        ui->tableViewMList->setItemDelegateForColumn(2, m_MLDele2);

        connect(m_MLDele1,&ImageDelegate::clicked,this,[=](const QModelIndex&index){
            QModelIndex test =m_pMList->index(index.row(),0);
            ui->tableViewMList->edit(test);
        });

        connect(m_MLDele2,&ImageDelegate::clicked,this,[=](const QModelIndex&index){
            m_pMList->removeRows(index.row(),1);
        });

        m_MLDele1->installEventFilter(this);
        m_MLDele2->installEventFilter(this);
        for(int m=0;m<10;m++)
        {
            QList<QStandardItem*>test;
            for(int i=0; i<3; i++)
            {
                QStandardItem *item = new QStandardItem("");
                test.push_back(item);
            }
            test[0]->setText("新宏008");
            test[0]->setFont(font);

            test[1]->setEditable(false);
            test[2]->setEditable(false);

            test[1]->setSelectable(false);
            test[2]->setSelectable(false);
            m_pMList->appendRow(test);
            ui->tableViewMList->setRowHeight(m,36);
        }
    }
    QTimer::singleShot(1000,this,[=]{
        setFixedSize(1200,768);
        ui->label_12->setFixedHeight(20);
        ui->label_15->setFixedHeight(20);
        ui->label_13->setFixedHeight(20);
        ui->label_48->setFixedHeight(20);
    });
}

DialogMouse::~DialogMouse()
{
    delete ui;
}

bool DialogMouse::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->labelAngleShow)
    {
        QRect rect = ui->labelAngleShow->rect();
        int nW = rect.width();
        int nCX = nW/2;
        int ra0 = nCX-1;
        int ra1 = nCX-25;

        float start = 270;

        for(int i=start; i<450; i+=1)
        {
            QPoint p0(nCX+ra0*cos(i*3.1415936/180),nCX-ra0*sin(i*3.1415936/180));
            if(rect.contains(p0))
            {
                start = i;
                break;
            }
        }

        float full = 180 + (360 - start) * 2.0;
        float fStep = full * 1.0 / 60;

        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *pMEvt = static_cast<QMouseEvent *>(event);

            double dx = pMEvt->pos().x() - nCX;
            double dy = pMEvt->pos().y() - nCX;
            double rad = qAtan2(dy, dx);
            double deg = fmod(360 - qRadiansToDegrees(rad),360);

            m_nowAngle = 30 - fmod((360 + deg - start),360) / fStep;
            ui->labelAngleShow->update();
        }

        if(event->type() == QEvent::Paint)
        {
            QPainter paniter(ui->labelAngleShow);
            paniter.setRenderHint(QPainter::Antialiasing);
            paniter.fillRect(rect,Qt::transparent);

            paniter.setPen(QPen(QColor("#373737"),4));
            for(int i=0; i<=60; i++)
            {
                float curAngle = (start + i * fStep) * 3.1415936/180;
                QPoint p0(nCX+ra0*cos(curAngle),nCX-ra0*sin(curAngle));
                QPoint p1(nCX+ra1*cos(curAngle),nCX-ra1*sin(curAngle));
                if(!rect.contains(p0))
                    break;

                paniter.drawLine(p0,p1);
            }

            int nAngle = m_nowAngle;
            if(nAngle<-30) nAngle = -30;
            if(nAngle>30)  nAngle = 30;
            float curAngle = (start + (30 - nAngle) * fStep) * 3.1415936/180;
            paniter.setPen(QPen(Qt::white,4));
            QPoint p0(nCX+ra0*cos(curAngle),nCX-ra0*sin(curAngle));
            QPoint p1(nCX+(ra1-8)*cos(curAngle),nCX-(ra1-8)*sin(curAngle));
            paniter.drawLine(p0,p1);
        }
    }

    return QDialog::eventFilter(watched, event);
}

void DialogMouse::updateName(const QString&name)
{
    ui->labelName->setText(name);
}

void DialogMouse::changeEvent(QEvent *pEvt)
{
    if(pEvt->type() == QEvent::LanguageChange)
    {
       ui->retranslateUi(this);

        SetButtonBackground(ui->pushButtonMkey0,false);
        SetButtonBackground(ui->pushButtonMkey3,false);
        SetButtonBackground(ui->pushButtonMkey4,false);

        SetButtonBackground(ui->pushButtonMkey1);
        SetButtonBackground(ui->pushButtonMkey2);
        SetButtonBackground(ui->pushButtonMkey5);
    }

    QDialog::changeEvent(pEvt);
}

void DialogMouse::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (event->pos().y() < 60)
        {
            m_dragPosition = event->globalPosition() - frameGeometry().topLeft();
            m_dragging = true;
        }
    }

    QDialog::mousePressEvent(event);
}

void DialogMouse::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_dragging)
    {
        QPointF MP = event->globalPosition() - m_dragPosition;
        move(MP.toPoint());
    }

    QDialog::mouseMoveEvent(event);
}

void DialogMouse::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragging = false;

    QDialog::mouseReleaseEvent(event);
}
