#include "ModuleLinear.h"
#include "ui_ModuleLinear.h"

#include <QTimer>
#include <QSlider>

ModuleLinear::ModuleLinear(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleLinear)
{
    ui->setupUi(this);

    ui->verticalSlider1->setStyleSheet(R"(
        QSlider::groove:vertical {  width: 24px; background: #DCDCDC; border-radius: 12px;}
        QSlider::groove:vertical:disabled { background: lightgray; }
        QSlider::handle:vertical:disabled { background: white; border: 2px solid #B3B3B3; }
        QSlider::sub-page:vertical { background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #FDDFBA, stop:1 #FFA73C);  border-radius: 12px; }
        QSlider::sub-page:vertical:disabled { background: lightgray; }
        QSlider::handle:vertical {
            width: 32px;
            height: 10px;
            margin: 0 -6px;
            border-radius: 5px;
            border: 2px solid white;
            background: #FFAF4E;}
        QSlider::handle:hover { background: #F0F0F0;}
        QSlider::handle:pressed { background: #E0E0E0; border-color: #2D7FDD; }
    )");

    ui->verticalSlider2->setStyleSheet(R"(
        QSlider::groove:vertical { width: 24px; background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #39E1DC, stop:1 #C3FFFD); border-radius: 12px;}
        QSlider::groove:vertical:disabled { background: lightgray; }
        QSlider::handle:vertical:disabled { background: white; border: 2px solid #B3B3B3; }
        QSlider::sub-page:vertical { background: #DCDCDC;  border-radius: 12px; }
        QSlider::sub-page:vertical:disabled { background: lightgray; }
        QSlider::handle:vertical {
            width: 32px;
            height: 10px;
            margin: 0 -6px;
            border-radius: 5px;
            border: 2px solid white;
            background: #7DFFFB;}
        QSlider::handle:hover { background: #F0F0F0;}
        QSlider::handle:pressed { background: #E0E0E0; border-color: #2D7FDD; }
    )");

    ui->verticalSlider1->setFixedWidth(40);
    ui->verticalSlider2->setFixedWidth(40);

    connect(ui->verticalSlider1,&QSlider::valueChanged,this,[=](int value){
        ui->lineEditValue1->blockSignals(true);
        value = (4000 - value/100*100);
        ui->lineEditValue1->setText(QString::asprintf("%.02f",value/1000.0));
        ui->lineEditValue1->blockSignals(false);
        emit onSetValue(value/1000.0,0);
    });
    connect(ui->verticalSlider2,&QSlider::valueChanged,this,[=](int value){
        ui->lineEditValue2->blockSignals(true);
        ui->lineEditValue2->setText(QString::asprintf("%.03f",(value/5*5)/1000.0));
        ui->lineEditValue2->blockSignals(true);
        emit onSetValue((value/5*5)/1000.0,1);
    });

    ui->verticalSlider1->setValue(3700);
    ui->verticalSlider2->setValue(400);

    ui->pushButtonM1->setAutoRepeat(true);
    ui->pushButtonM1->setAutoRepeatInterval(100);

    connect(ui->pushButtonM1,&QPushButton::pressed,this,[=]{
        int value = ui->verticalSlider1->value() - 100;
        ui->verticalSlider1->setValue(value);
    });

    ui->pushButtonP1->setAutoRepeat(true);
    ui->pushButtonP1->setAutoRepeatInterval(100);
    connect(ui->pushButtonP1,&QPushButton::pressed,this,[=]{
        int value = ui->verticalSlider1->value() + 100;
        ui->verticalSlider1->setValue(value);
    });

    connect(ui->lineEditValue1,&QLineEdit::textChanged,this,[=](const QString&text){
        QString strTmp = ui->lineEditValue1->text().trimmed();
        char szText[100]={0};
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
        if(value>4000) value = 4000;
        if(value<0)    value = 0;
        ui->verticalSlider1->blockSignals(true);
        ui->verticalSlider1->setValue(4000-value);
        ui->verticalSlider1->blockSignals(false);
    });

    ui->pushButtonM2->setAutoRepeat(true);
    ui->pushButtonM2->setAutoRepeatInterval(100) ;
    connect(ui->pushButtonM2,&QPushButton::pressed,this,[=]{
        int value = ui->verticalSlider2->value() - 5;
        ui->verticalSlider2->setValue(value);
    }) ;

    ui->pushButtonP2->setAutoRepeat(true);
    ui->pushButtonP2->setAutoRepeatInterval(100);
    connect(ui->pushButtonP2,&QPushButton::pressed,this,[=]{
        int value = ui->verticalSlider2->value() + 5;
        ui->verticalSlider2->setValue(value);
    }) ;

    connect(ui->lineEditValue2,&QLineEdit::textChanged,this,[=](const QString&text){
        QString strTmp = ui->lineEditValue2->text().trimmed();
        char szText[100]={0};
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
        if(value>4000) value = 4000;
        if(value<0) value = 0;

        ui->verticalSlider2->blockSignals(true);
        ui->verticalSlider2->setValue(value);
        ui->verticalSlider2->blockSignals(true);
    });

    ui->verticalSlider1->installEventFilter(this);
    ui->verticalSlider2->installEventFilter(this);
}

ModuleLinear::~ModuleLinear()
{
    delete ui;
}

bool ModuleLinear::eventFilter(QObject*watched,QEvent*event)
{
    if (event->type() == QEvent::MouseButtonRelease && (watched == ui->verticalSlider1 || watched == ui->verticalSlider2))
    {
        QSlider *pSlider = static_cast<QSlider *>(watched);
        if(pSlider->isEnabled())
        {
            int h = pSlider->height();
            int value = (pSlider->maximum() - pSlider->minimum()) * (h - pSlider->mapFromGlobal(cursor().pos()).y()) / h;
            pSlider->setValue(value + pSlider->minimum());
        }
    }

    return QFrame::eventFilter(watched,event);
}

void ModuleLinear::setText(const QString&text1,const QString&text2)
{
    ui->labelTitleS1->setText(text1 + ("(mm)"));
    ui->labelTitleS2->setText(text2 + ("(mm)"));
}


void ModuleLinear::setValue(float value, bool left)
{
    if(left)
        ui->verticalSlider1->setValue(4000-value*1000);
    else
        ui->verticalSlider2->setValue(value*1000);
}

float ModuleLinear::getValue(bool left)
{
    if(left)
        return ui->lineEditValue1->text().toFloat();

    return ui->lineEditValue2->text().toFloat();
}

void ModuleLinear::hidePannel(bool left,bool hide)
{
    if(left)
        ui->leftPan->setHidden(hide);
    else
        ui->rightPan->setHidden(hide);
}

void ModuleLinear::enablePannel(bool left,bool enable)
{
    if(left)
    {
        ui->leftPan->setEnabled(enable);
        ui->verticalSlider1->setEnabled(enable);
    }
    else
    {
        ui->rightPan->setEnabled(enable);
        ui->verticalSlider2->setEnabled(enable);
    }
}

