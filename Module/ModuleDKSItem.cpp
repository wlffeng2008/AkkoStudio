#include "ModuleDKSItem.h"
#include "ui_ModuleDKSItem.h"

#include <QPainter>
#include <QMouseEvent>

ModuleDKSItem::ModuleDKSItem(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleDKSItem)
{
    ui->setupUi(this);

    ui->pushButton_FuncKey->installEventFilter(this);
    setMouseTracking(true);
}

ModuleDKSItem::~ModuleDKSItem()
{
    delete ui;
}

void ModuleDKSItem::setText(const QString&text)
{
    ui->pushButton_FuncKey->setText(text);
    if(text.isEmpty())
        ui->pushButton_FuncKey->setText(tr("未设置功能"));
}

void ModuleDKSItem::setData(quint8 data)
{
    ui->frameSet->showValue(data);
}

quint32 ModuleDKSItem::getData()
{
    return ui->frameSet->combineValue();
}

bool ModuleDKSItem::eventFilter(QObject*watched,QEvent*event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        if(watched == ui->pushButton_FuncKey)
            emit onButtonClicked();
    }

    return QFrame::eventFilter(watched, event);
}


void ModuleDKSItem::changeEvent(QEvent *pEvt)
{
    if(pEvt->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QFrame::changeEvent(pEvt);
}