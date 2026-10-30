#include "VirtualKeyboard.h"
#include "ui_VirtualKeyboard.h"

VirtualKeyboard::VirtualKeyboard(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::VirtualKeyboard)
{
    ui->setupUi(this);
}

VirtualKeyboard::~VirtualKeyboard()
{
    delete ui;
}

void VirtualKeyboard::changeEvent(QEvent *pEvt)
{
    if(pEvt->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QFrame::changeEvent(pEvt);
}