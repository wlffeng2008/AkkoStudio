#include "DialogVKPicker.h"
#include "ui_DialogVKPicker.h"

#include <QApplication>
#include <QStyleOption>
#include <QScreen>
#include <QStyleOption>
#include <QPainter>
#include <QEvent>

static QPoint getGlobalPos(QWidget *widget) {
    QPoint pos = widget->pos();
    QWidget *w = widget->parentWidget();
    if(w && !(w->windowFlags() & Qt::Popup))
        pos = w->mapToGlobal(pos) ;
    // while (w)
    // {
    //     if()
    //         pos += w->pos();
    //     w = w->parentWidget();
    // }
    return pos;
}

DialogVKPicker::DialogVKPicker(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogVKPicker)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet("QDialog { background-color: rgba(180, 180, 180, 0.9);  border: none; border-radius: 24px;}");

    QRect geoMetry = QApplication::primaryScreen()->geometry();
    if(parent)
    {
        geoMetry = parent->frameGeometry() ;//parent->mapToGlobal(parent->pos());
        QPoint globalPos = getGlobalPos(parent);
        geoMetry = QRect(globalPos.x(),globalPos.y(),geoMetry.width(),geoMetry.height()) ;
    }
    setGeometry(geoMetry);
    setFixedSize(geoMetry.width(), geoMetry.height());

    connect(ui->pushButtonCancel,&QPushButton::clicked,this,[=]{ this->reject(); });
    connect(ui->pushButtonOK,&QPushButton::clicked,this,[=]{ this->accept(); });

    ui->labelSelect1->installEventFilter(this);
    ui->labelSelect2->installEventFilter(this);
    ui->labelSelect3->installEventFilter(this);

    m_selIds[0]=0;
    m_selIds[1]=0;
    m_selIds[2]=0;

    connect(ui->buttonGroup,&QButtonGroup::buttonClicked,this,[=](QAbstractButton *btn){

        QString text = btn->text();
        quint8 hid = btn->objectName().right(3).toUInt();
        qDebug() << "DialogVKPicker" << text << hid;
        if(m_selIds[0] == 0)
        {
            m_selIds[0]=hid;
            ui->labelSelect1->setText(text);
            qDebug() << "get1" ;
            return ;
        }

        if(m_selIds[1] == 0)
        {
            m_selIds[1]=hid;
            ui->labelSelect2->setText(text);
            qDebug() << "get2" ;
            return ;
        }

        if(m_selIds[2] == 0)
        {
            m_selIds[2]=hid;
            ui->labelSelect3->setText(text);
            qDebug() << "get3" ;
            return ;
        }
    });
}

DialogVKPicker::~DialogVKPicker()
{
    delete ui;
}

bool DialogVKPicker::eventFilter(QObject *watched,QEvent *event)
{
    if(event->type()==QEvent::MouseButtonPress)
    {
        if(watched == ui->labelSelect1)
        {
            m_selIds[0] = 0 ;
            ui->labelSelect1->setText("--");
        }

        if(watched == ui->labelSelect2)
        {
            m_selIds[1] = 0 ;
            ui->labelSelect2->setText("--");
        }

        if(watched == ui->labelSelect3)
        {
            m_selIds[2] = 0 ;
            ui->labelSelect3->setText("--");
        }
    }

    return QDialog::eventFilter(watched,event);
}


void DialogVKPicker::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QDialog::paintEvent(event);
}
