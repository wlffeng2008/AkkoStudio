#include "ModuleEfSpeed.h"
#include "ui_ModuleEfSpeed.h"

ModuleEfSpeed::ModuleEfSpeed(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleEfSpeed)
{
    ui->setupUi(this);

    connect(ui->horiCyanSlider,&QSlider::valueChanged,this,[=](int value){
        ui->labelTitle2->setText(QString::asprintf("%d%%",value*25));
        if(!m_bOutSet) emit onSpeedChanged(value);
        m_bOutSet=false;
    });
}

ModuleEfSpeed::~ModuleEfSpeed()
{
    delete ui;
}

void ModuleEfSpeed::setSpeed(quint8 speed)
{
    m_bOutSet = true;
    ui->horiCyanSlider->setValue(speed);
}
