#include "ModuleDKSItem.h"
#include "ui_ModuleDKSItem.h"

#include <QPainter>
#include <QMouseEvent>

ModuleDKSItem::ModuleDKSItem(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleDKSItem)
{
    ui->setupUi(this);

    ui->labelSwitch1->installEventFilter(this);
    ui->labelSwitch2->installEventFilter(this);
    ui->labelSwitch3->installEventFilter(this);
    ui->labelSwitch4->installEventFilter(this);
    setMouseTracking(true) ;

    setSwitch(0);
    setSwitch(1);
    setSwitch(2);
    setSwitch(3);

    connect(&m_TMReset,&QTimer::timeout,this,[=]{
        if(!m_dragging)
        {
            m_TMReset.stop();
            m_dragX1 = 0;
        }
    });

    connect(ui->pushButton_FuncKey,&QPushButton::clicked,this,[=]{
        emit onButtonClicked();
    });
}

ModuleDKSItem::~ModuleDKSItem()
{
    delete ui;
}

void ModuleDKSItem::setSwitch(int index,bool on)
{
    QList<QLabel*> labels={ui->labelSwitch1,ui->labelSwitch2,ui->labelSwitch3,ui->labelSwitch4};
    m_switchs[index] = on;
    labels[index]->setPixmap(QPixmap(on?":/images/k/ic_selected.png":":/images/k/ic_add.png"));
}

void ModuleDKSItem::setText(const QString&text)
{
    ui->pushButton_FuncKey->setText(text);
    if(text.isEmpty())
        ui->pushButton_FuncKey->setText(tr("未设置功能"));
}

void ModuleDKSItem::setData(quint8 data)
{

}

quint8 ModuleDKSItem::getData()
{
    return 4;
}

bool ModuleDKSItem::eventFilter(QObject*watched,QEvent*event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        m_dragging = false ;
        QList<QLabel*> labels={ui->labelSwitch1,ui->labelSwitch2,ui->labelSwitch3,ui->labelSwitch4};
        for(int i=0; i<4; i++)
        {
            if(watched == labels[i])
            {
                setSwitch(i,!m_switchs[i]);
                return true ;
            }
        }
    }

    return QFrame::eventFilter(watched, event);
}

bool ModuleDKSItem::event(QEvent*event)
{
    QMouseEvent *pMEvent = static_cast<QMouseEvent *>(event) ;
    if(event->type() == QEvent::MouseButtonPress)
    {
        if(m_dragX1 == 0)
            m_dragX1 = pMEvent->pos().x() ;
        m_dragging = true ;
    }
    if(event->type() == QEvent::MouseButtonRelease)
    {
        m_dragging = false ;
    }

    if(event->type() == QEvent::MouseMove && m_dragging)
    {
        m_TMReset.stop() ;
        m_dragX2 = pMEvent->pos().x() ;
        QList<QLabel*> labels={ui->labelSwitch1,ui->labelSwitch2,ui->labelSwitch3,ui->labelSwitch4};
        for(int i=0; i<3; i++)
        {
            QRect rc = labels[i]->geometry() ;
            if(rc.right()<m_dragX2 && rc.left()>m_dragX1)
                labels[i]->hide();
            else
                labels[i]->show();
        }

        bool allshow = true;
        for(int i=0; i<3; i++)
        {
            if(labels[i]->isHidden())
            {
                allshow = false;
                break;
            }
        }
        if(allshow)
        {
            m_dragX2=0;
            m_TMReset.start(500) ;
        }

        update() ;
    }

    return QFrame::event(event);
}


void ModuleDKSItem::paintEvent(QPaintEvent*event)
{
    QPainter painter(this) ;
    painter.setRenderHint(QPainter::Antialiasing);

    int x1 = m_dragX1;
    int x2 = m_dragX2;

    QList<QLabel*> labels={ui->labelSwitch1,ui->labelSwitch2,ui->labelSwitch3,ui->labelSwitch4};
    for(int i=0; i<4; i++)
    {
        QPoint pos = labels[i]->pos();
        if(x1<pos.x())
        {
            x1 = pos.x() - 15;
            break;
        }
    }

    for(int i=0; i<4; i++)
    {
        QPoint pos = labels[3-i]->geometry().bottomRight()  ;
        if(x2>pos.x())
        {
            x2 = pos.x() + 50 ;
            break;
        }
    }

    if(x1<130) x1 = 130;
    if(x2>368) x2 = 368;

    int nW = x2-x1 ;
    if(nW<10)
    {
        QFrame::paintEvent(event);
        return ;
    }

    QRect rect(x1,8,nW,24) ;
    painter.setPen(0x6329B6) ;
    painter.setBrush(0x6329B6) ;
    painter.drawRoundedRect(rect,12,12);
    painter.drawPixmap(rect.right()-20,10,QPixmap(":/images/k/ic_selected.png"));
}
