#include "FrameMouse.h"
#include "qlineedit.h"
#include "ui_FrameMouse.h"


#include <QTimer>

FrameMouse::FrameMouse(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameMouse)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->buttonGroup,&QButtonGroup::idClicked,this,[=](int id){
        ui->stackedWidget->setCurrentIndex(abs(id)-2);
    });

    QTimer::singleShot(100,this,[=]{
        ui->labelGohome->installEventFilter(this);
        ui->labelGohome->updateToolTip(tr("返回首页"));
    });

}

FrameMouse::~FrameMouse()
{
    delete ui;
}

void FrameMouse::updateName(const QString&name)
{
    ui->labelName->setText(name);
}


bool FrameMouse::eventFilter(QObject *watch, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        if(ui->labelGohome == watch)
            emit onReturn();
    }

    return QFrame::eventFilter(watch, event);
}