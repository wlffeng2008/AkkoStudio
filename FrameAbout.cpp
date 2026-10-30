#include "FrameAbout.h"
#include "ui_FrameAbout.h"

#include "ModuleVideoItem.h"

FrameAbout::FrameAbout(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameAbout)
{
    ui->setupUi(this);

    for(int i=0; i<20; i++)
    {
        ModuleVideoItem *btn = new ModuleVideoItem(this);
        btn->setFixedSize(340,280);
        ui->gridLayout->addWidget(btn,i/3,i%3);
    }
}

FrameAbout::~FrameAbout()
{
    delete ui;
}

void FrameAbout::changeEvent(QEvent *pEvt)
{
    if(pEvt->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QFrame::changeEvent(pEvt);
}