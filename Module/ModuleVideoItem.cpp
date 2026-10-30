#include "ModuleVideoItem.h"
#include "ui_ModuleVideoItem.h"

ModuleVideoItem::ModuleVideoItem(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleVideoItem)
{
    ui->setupUi(this);
}

ModuleVideoItem::~ModuleVideoItem()
{
    delete ui;
}

void ModuleVideoItem::changeEvent(QEvent *pEvt)
{
    if(pEvt->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QFrame::changeEvent(pEvt);
}