#include "FrameSystemInfo.h"
#include "ui_FrameSystemInfo.h"
#include "ModuleGeneralMasker.h"

FrameSystemInfo::FrameSystemInfo(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameSystemInfo)
{
    ui->setupUi(this);

    connect(ui->pushButtonOK,&QPushButton::clicked,this,[=]{
        ModuleGeneralMasker *pTop = static_cast<ModuleGeneralMasker *>(this->parent());
        pTop->setFlag(QDialog::Accepted);
        hide();
    });

    connect(ui->pushButtonClose,&QPushButton::clicked,this,[=]{
        ModuleGeneralMasker *pTop = static_cast<ModuleGeneralMasker *>(this->parent());
        pTop->setFlag(QDialog::Rejected);
        hide();
    });

    connect(ui->pushButtonCancel,&QPushButton::clicked,this,[=]{
        ModuleGeneralMasker *pTop = static_cast<ModuleGeneralMasker *>(this->parent());
        pTop->setFlag(QDialog::Rejected);
        hide();
    });
    //show();
}

FrameSystemInfo::~FrameSystemInfo()
{
    delete ui;
}
