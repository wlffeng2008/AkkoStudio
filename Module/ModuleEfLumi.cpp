#include "ModuleEfLumi.h"
#include "ui_ModuleEfLumi.h"

ModuleEfLumi::ModuleEfLumi(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleEfLumi)
{
    ui->setupUi(this);

    connect(ui->horizontalSlider,&QSlider::valueChanged,this,[=](int value){
        ui->labelTitle2->setText(QString::asprintf("%d%%",value*25));
        if(!m_bOutSet) emit onBrightChanged(value);
        m_bOutSet = false;
    });
}

ModuleEfLumi::~ModuleEfLumi()
{
    delete ui;
}

void ModuleEfLumi::setBright(quint8 bright)
{
    m_bOutSet = true;
    ui->horizontalSlider->setValue(bright);
}
