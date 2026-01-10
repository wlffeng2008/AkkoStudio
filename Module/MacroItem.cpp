#include "MacroItem.h"
#include "ui_MacroItem.h"

static MacroItem *activeItem=nullptr ;
MacroItem::MacroItem(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::MacroItem)
{
    ui->setupUi(this);
    setCursor(Qt::PointingHandCursor);
    if(!activeItem) activeItem=this;
    setStyleSheet("QFrame#MacroItem{border-radius: 6px; padding:3px}") ;
    connect(ui->pushButtonEdit,&QPushButton::clicked,this,[=]{
        if(m_bInEditing)
            return;
        ui->labelMacroName->hide();
        ui->lineEditMacroName->show();
        ui->lineEditMacroName->setText(ui->labelMacroName->text());
        ui->lineEditMacroName->setFocus();
        m_bInEditing=true;
    });

    connect(ui->lineEditMacroName,&QLineEdit::editingFinished,this,[=](){
        ui->labelMacroName->show();
        ui->lineEditMacroName->hide();
        ui->labelMacroName->setText(ui->lineEditMacroName->text());
        m_bInEditing=false;
        emit onOperation(2,this);
    });

    connect(ui->pushButtonDelete,&QPushButton::clicked,this,[=]{
        activeItem = nullptr;
        emit onOperation(1,this);
    }) ;

    ui->lineEditMacroName->hide();

    ui->labelMacroName->installEventFilter(this);
}

MacroItem::~MacroItem()
{
    delete ui;
}

void MacroItem::setMacroName(const QString&strName)
{
    ui->labelMacroName->setText(strName.trimmed());
}

QString MacroItem:: getMarcoName()
{
    return ui->labelMacroName->text().trimmed();
}

void MacroItem::setActive()
{
    if(activeItem)
        activeItem->setStyleSheet("QFrame#MacroItem { background-color: transparent;  padding:3px}");
    activeItem=this;
    setStyleSheet("QFrame#MacroItem{ border: 1px solid #6329B6; border-radius: 10px; padding:2px}");
}

bool MacroItem::eventFilter(QObject *watched,QEvent *event)
{
    if(watched == ui->labelMacroName && event->type() == QEvent::MouseButtonDblClick)
    {
    }
    return QFrame::eventFilter(watched,event);
}

bool MacroItem::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        setActive();
        emit onOperation(0,this);
    }

    if(event->type() == QEvent::MouseButtonDblClick)
    {
        setActive();
        ui->pushButtonEdit->click();
        return true;
    }
    return QFrame::event(event);
}
