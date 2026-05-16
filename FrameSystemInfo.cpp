#include "FrameSystemInfo.h"
#include "ui_FrameSystemInfo.h"
#include "ModuleGeneralMasker.h"
#include <QSettings>
#include <QApplication>

FrameSystemInfo::FrameSystemInfo(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameSystemInfo)
{
    ui->setupUi(this);

    static QSettings regSet("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

    ui->checkBox->setChecked(!regSet.value("AkkoHub").toString().isEmpty());

    connect(ui->pushButtonOK,&QPushButton::clicked,this,[=]{
        ModuleGeneralMasker *pTop = static_cast<ModuleGeneralMasker *>(this->parent());
        pTop->setFlag(QDialog::Accepted);
        regSet.remove("AkkoHub");
        if(ui->checkBox->isChecked())
        {
            QString strFile = QString("\"%1\"").arg(QApplication::applicationFilePath().replace("/","\\"));
            regSet.setValue("AkkoHub",strFile);
        }

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

    raise();
}

FrameSystemInfo::~FrameSystemInfo()
{
    delete ui;
}
