#include "ModuleAddMacroSquare.h"
#include "qdialog.h"
#include "qevent.h"
#include "ui_ModuleAddMacroSquare.h"
#include "ModuleGenKeymapping.h"
#include "ModuleGeneralMasker.h"

ModuleAddMacroSquare::ModuleAddMacroSquare(bool enablePostion, QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleAddMacroSquare)
{
    ui->setupUi(this);

    if(!enablePostion)
        ui->framePosition->setDisabled(true);

    connect(ui->pushButtonOK,&QPushButton::clicked,this,[=]{
        ModuleGeneralMasker *pTop = static_cast<ModuleGeneralMasker *>(this->parent());
        pTop->setFlag(QDialog::Accepted);
        hide();
    });

    connect(ui->pushButtonInsert,&QPushButton::clicked,this,[=]{
        //ModuleGeneralMasker *pTop = static_cast<ModuleGeneralMasker *>(this->parent());
        //pTop->setFlag(QDialog::Rejected);
        //hide();
        emit insert();
    });
}

ModuleAddMacroSquare::~ModuleAddMacroSquare()
{
    delete ui;
}

void ModuleAddMacroSquare::keyPressEvent(QKeyEvent *event)
{
    int code = event->nativeScanCode();
    int hid = ::getKeyHid(code);
    m_hid = hid;
    QString text = event->text();
    ui->lineEditKey->setText(text.toUpper());

    m_nativeVK = event->nativeVirtualKey();
    m_rawKey = event->key();

    QFrame::keyPressEvent(event);
}

quint8 ModuleAddMacroSquare::type()
{
    if(ui->radioButton1->isChecked()) return 0; // key
    if(ui->radioButton2->isChecked()) return 1; // mouse
    if(ui->radioButton3->isChecked()) return 2; // position
    return 0;
}

quint16 ModuleAddMacroSquare::kNativeVK(){ return m_nativeVK;}

quint16 ModuleAddMacroSquare::kRawKey(){return m_rawKey;}

quint8 ModuleAddMacroSquare::mKey()
{
    if(ui->checkBox1->isChecked()) return Qt::LeftButton  ;
    if(ui->checkBox2->isChecked()) return Qt::MiddleButton;
    if(ui->checkBox3->isChecked()) return Qt::RightButton ;
    return 0;
}

quint8 ModuleAddMacroSquare::bKey()
{
    return m_hid;
}

quint16 ModuleAddMacroSquare::xPos()
{
    return ui->spinBoxX->value();
}

quint16 ModuleAddMacroSquare::yPos()
{
    return ui->spinBoxY->value();
}
