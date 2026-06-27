#include "FrameMain.h"
#include "ui_FrameMain.h"

#include "DialogDeviceConnect.h"
#include "modulertset.h"
#include "modulelinear.h"

#include <QTimer>
#include <QButtonGroup>

FrameMain::FrameMain(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameMain)
{
    ui->setupUi(this);

    DialogDeviceConnect *pCnn = DialogDeviceConnect::instance();

    connect(ui->frameLEDMode,&ModuleEfMode::onModeChanged,this,[=](int mode,quint8 opt){
        if(mode == -1)
        {
            pCnn->setLEDOn(opt);
            return;
        }
        pCnn->setLEDMode(mode,opt);
    });

    connect(ui->frameLEDBright,&ModuleEfLumi::onBrightChanged,this,[=](int bright){
        pCnn->setLEDBright(bright);
    });
    connect(ui->frameLEDSpeed,&ModuleEfSpeed::onSpeedChanged,this,[=](int speed){
        pCnn->setLEDSpeed(speed);
    });
    connect(ui->frameLEDColor,&ModuleEfColor::onSetColor,this,[=](const QColor&color, int option){
        pCnn->setLEDColor(color,option);
    });

    connect(ui->frameLEDColor,&ModuleEfColor::onSetSideLed,this,[=](const QByteArray&data){
        pCnn->setSideLed(data);
    });

    connect(pCnn,&DialogDeviceConnect::onReadBack,[=](const QByteArray&data){
        quint8 *pPack = (quint8 *)data.data();
        quint8 cmd = pPack[0];
        if(cmd == CMD_GET_LEDPARAM)
        {
            if(ui->frameLEDMode)  ui->frameLEDMode->setEfMode(pPack[1]);
            if(ui->frameLEDSpeed) ui->frameLEDSpeed->setSpeed(4 - pPack[2]);
            if(ui->frameLEDSpeed) ui->frameLEDBright->setBright(pPack[3]);
        }

        if(cmd == CMD_GET_SLEDPARAM)
        {
            ui->frameLEDColor->updateData(data);
        }
    });
    connect(ui->frameRTSetting,&ModuleRtSet::onSetValue,this,[=](float value,int type){
        //qDebug() << value << type;
        pCnn->send65Cmd(0x00,0xFF,value*200,true);
        pCnn->send65Cmd(0x01,0xFF,value*200,true);
    });
    connect(ui->frameLinearSet,&ModuleLinear::onSetValue,this,[=](float value,int type){
        //qDebug() << value << type;
        if(type == 0) pCnn->send65Cmd(0x02,0xFF,value*200,true);
        if(type == 1) pCnn->send65Cmd(0x03,0xFF,value*200,true);
    });

    ui->labelPic->hide();
}

FrameMain::~FrameMain()
{
    delete ui;
}

void FrameMain::setDeviceImage(const QString&strImage)
{
    ui->labelPic->show();
    ui->labelPic->setPixmap(QPixmap(strImage));
}

bool FrameMain::eventFilter(QObject*watched,QEvent*event)
{
    return QFrame::eventFilter(watched,event);
}
