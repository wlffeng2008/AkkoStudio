#include "FrameMain.h"
#include "ui_FrameMain.h"

#include "DialogDeviceConnect.h"

#include <QTimer>
#include <QButtonGroup>

FrameMain::FrameMain(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameMain)
{
    ui->setupUi(this);

    DialogDeviceConnect *pConnector = DialogDeviceConnect::instance();

    connect(ui->frameLEDMode,&ModuleEfMode::onModeChanged,this,[=](int mode,bool on){
        if(mode == 0)
        {
            pConnector->setLEDOn(on);
            return ;
        }
        pConnector->setLEDMode(mode);
    });
    connect(ui->frameLEDMode,&ModuleEfMode::onModePicture,this,[=](int index){
        pConnector->setLEDPicture(index);
    });
    connect(ui->frameLEDBright,&ModuleEfLumi::onBrightChanged,this,[=](int bright){
        pConnector->setLEDBright(bright);
    });
    connect(ui->frameLEDSpeed,&ModuleEfSpeed::onSpeedChanged,this,[=](int speed){
        pConnector->setLEDSpeed(speed);
    });
    connect(ui->frameLEDColor,&ModuleEfColor::onSetColor,this,[=](const QColor&color, int option){
        pConnector->setLEDColor(color,option);

    });

    connect(pConnector,&DialogDeviceConnect::onReadBack,[=](const QByteArray&data){
        quint8 *pPack = (quint8 *)data.data() ;
        quint8 cmd = pPack[0] ;
        if(cmd == CMD_GET_LEDPARAM)
        {
            if(ui->frameLEDMode)
            ui->frameLEDMode->setEfMode(pPack[1]);
            if(ui->frameLEDSpeed)
            ui->frameLEDSpeed->setSpeed(pPack[2]);
            if(ui->frameLEDSpeed)
            ui->frameLEDBright->setBright(pPack[3]);
        }
    });
}

FrameMain::~FrameMain()
{
    delete ui;
}

void FrameMain::setDeviceImage(const QString&strImage)
{
    ui->labelPic->setPixmap(QPixmap(strImage));
}

bool FrameMain::eventFilter(QObject*watched,QEvent*event)
{

    return QFrame::eventFilter(watched,event);
}
