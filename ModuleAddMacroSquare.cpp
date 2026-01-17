#include "ModuleAddMacroSquare.h"
#include "qdialog.h"
#include "ui_ModuleAddMacroSquare.h"

#include "ModuleGeneralMasker.h"

ModuleAddMacroSquare::ModuleAddMacroSquare(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleAddMacroSquare)
{
    ui->setupUi(this);

    connect(ui->pushButtonOK,&QPushButton::clicked,this,[=]{
        ModuleGeneralMasker *pTop = static_cast<ModuleGeneralMasker *>(this->parent());
        pTop->setFlag(QDialog::Accepted);
        hide();
    });

    connect(ui->pushButtonCancel,&QPushButton::clicked,this,[=]{
        ModuleGeneralMasker *pTop = static_cast<ModuleGeneralMasker *>(this->parent());
        pTop->setFlag(QDialog::Rejected);
        hide();
    });
}

ModuleAddMacroSquare::~ModuleAddMacroSquare()
{
    delete ui;
}

quint8 ModuleAddMacroSquare::type()
{
    if(ui->radioButton1->isChecked()) return Qt::LeftButton  ;
    if(ui->radioButton2->isChecked()) return Qt::MiddleButton;
    if(ui->radioButton3->isChecked()) return Qt::RightButton ;
    return 0;
}

quint8 ModuleAddMacroSquare::mKey()
{
    if(ui->checkBox1->isChecked()) return 0;
    if(ui->checkBox2->isChecked()) return 1;
    if(ui->checkBox3->isChecked()) return 2;
    return 0;
}

quint8 ModuleAddMacroSquare::bKey()
{
    return ui->lineEditKey->text().toInt();
}

quint8 ModuleAddMacroSquare::xPos()
{
    return ui->lineEditX->text().toInt();
}

quint8 ModuleAddMacroSquare::yPos()
{
    return ui->lineEditY->text().toInt();
}
