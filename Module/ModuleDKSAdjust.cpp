#include "ModuleDKSAdjust.h"
#include "ui_ModuleDKSAdjust.h"

#include <QTimer>

ModuleDKSAdjust::ModuleDKSAdjust(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleDKSAdjust)
{
    ui->setupUi(this);

    QString strStyle(R"(
        QPushButton {
                    border: none;
                    border-radius: 12px;
                    padding: 1px 1px;
                    color: black;
                    background-color: #F6F6F6;
                    min-width:24px;
                    max-width:542px;
                    min-height:24px;}

        QPushButton:hover { background-color: transparent;}
        )");

    QTimer::singleShot(100,this,[=]{
        ui->pushButtonCancel->setStyleSheet(strStyle) ;
        ui->pushButtonOK->setStyleSheet(strStyle + "QPushButton {background-color: #B3B3B3;} QPushButton:hover { background-color: #B3B3B3;}") ;
        ui->pushButtonM1->setStyleSheet(strStyle) ;
        ui->pushButtonP1->setStyleSheet(strStyle) ;
    });

    static QString strStyle3(R"(

        QLineEdit {
            border: 1px solid #EDEDED;
            border-radius: 6px;
            min-height: 20px;
            min-width: 60px;
            color: black;
            text-align: center;
            background-color: #EDEDED;
            font-size: 14px ;
            font-weight: 500 ; }

            QLineEdit:hover { background-color: #F0F0F0;}

        )") ;

    ui->lineEditValue1->setStyleSheet(strStyle3);

    ui->verticalSlider1->setFixedWidth(40) ;

    connect(ui->verticalSlider1,&QSlider::valueChanged,this,[=](int value){ui->lineEditValue1->setText(QString::asprintf("%.03f",value/1000.0));});

    ui->verticalSlider1->setValue(700) ;

    ui->pushButtonM1->setAutoRepeat(true);
    ui->pushButtonM1->setAutoRepeatInterval(100) ;
    connect(ui->pushButtonM1,&QPushButton::pressed,this,[=]{
        int value = ui->verticalSlider1->value() - 5 ;
        ui->verticalSlider1->setValue(value);
    }) ;

    ui->pushButtonP1->setAutoRepeat(true);
    ui->pushButtonP1->setAutoRepeatInterval(100) ;
    connect(ui->pushButtonP1,&QPushButton::pressed,this,[=]{
        int value = ui->verticalSlider1->value() + 5 ;
        ui->verticalSlider1->setValue(value);
    }) ;

    QTimer *pTMUpdate1 = new QTimer(this) ;

    connect(ui->lineEditValue1,&QLineEdit::textEdited,this,[=](const QString&text){
        pTMUpdate1->stop() ;
        pTMUpdate1->start(300) ;
    });

    connect(pTMUpdate1,&QTimer::timeout,this,[=]{
        pTMUpdate1->stop() ;
        QString strTmp = ui->lineEditValue1->text().trimmed() ;
        char szText[100]={0} ;
        strcpy_s(szText, strTmp.toStdString().c_str()) ;
        for(int i=0; i<strlen(szText); i++)
        {
            if(szText[i] == '.' || (szText[i] >= '0' && szText[i] <= '9'))
                continue ;
            szText[i] = 0 ;
            break;
        }
        strTmp = szText;
        int value = ui->lineEditValue1->text().toFloat() * 1000;
        if(value>4000) value = 4000 ;
        if(value<0) value = 0 ;
        ui->verticalSlider1->setValue(value);
    });

    connect(ui->pushButtonCancel,&QPushButton::clicked,this,[=]{ close(); }) ;

    connect(ui->pushButtonOK,&QPushButton::clicked,this,[=]{

        QString strTmp = ui->lineEditValue1->text().trimmed() ;
        emit onValueSave(strTmp);
        close();
    }) ;
}

ModuleDKSAdjust::~ModuleDKSAdjust()
{
    delete ui;
}

void ModuleDKSAdjust::setOriginValue(const QString&strValue)
{
    ui->lineEditValue1->setText(strValue) ;
}

void ModuleDKSAdjust::changeEvent(QEvent *pEvt)
{
    if(pEvt->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QFrame::changeEvent(pEvt);
}