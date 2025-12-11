#include "keyboardbutton.h"
#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

keySetTooltip::keySetTooltip(QWidget *parent):QWidget(parent)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    label1 = new QLabel(this);
    label2 = new QLabel(this);
    label1->setAlignment(Qt::AlignCenter);
    label2->setAlignment(Qt::AlignCenter);
    label1->setStyleSheet("QLabel{color:white;background-color:transparent;}");
    label2->setStyleSheet("QLabel{color:white;background-color:transparent;}");

    QVBoxLayout *pBox = new QVBoxLayout(this) ;
    pBox->addWidget(label1);
    pBox->addWidget(label2);
    pBox->setContentsMargins(10,2,10,16);
    pBox->setSpacing(10);
}

void keySetTooltip::setText1(const QString&text)
{
    label1->setText(text);
    label2->setText(text);
}

void keySetTooltip::setText2(const QString&text)
{
    label2->setText(text);
    label2->adjustSize();
}

void keySetTooltip::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = this->rect() ;

    painter.setPen(0x6329B6);
    painter.setBrush(0x6329B6);
    painter.drawRoundedRect(rect.adjusted(0,0,0,-10),8,8);

    QRect triRC = this->rect() ;
    triRC.setTop(triRC.bottom()-12);
    int nImgX = rect.center().x() ;
    int nTriLen = 14;
    QPolygonF triangle;
    triangle<<QPoint(nImgX,triRC.top()) ;
    triangle<<QPoint(nImgX+nTriLen,triRC.top()) ;
    triangle<<QPoint(nImgX+nTriLen/2,triRC.top() + nTriLen/2 * sqrt(3)) ;

    painter.drawPolygon(triangle);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);
    painter.drawLine(rect.left()+10,rect.center().y()-5,rect.right()-10,rect.center().y()-5);
}

//----------------------------------------

KeyboardButton::KeyboardButton(QWidget *parent):QPushButton(parent)
{
    m_mtFlag = ":/images/mt0.png" ;

    QTimer::singleShot(50,this,[=]{

    });
}

void KeyboardButton::setTipText(const QString&strText1,const QString&strText2)
{
    m_tip = new keySetTooltip(this);
    m_tip->setText1(strText1);
    m_tip->setText2(strText2);

    timer = new QTimer(this) ;
    timer->setSingleShot(true);
    timer->setInterval(50);

    connect(timer, &QTimer::timeout, this, [=]() {
        if(m_tip)
        {
            m_tip->show();
            m_tip->updateGeometry();
        }
        timer->stop();
    });
}

bool KeyboardButton::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Enter:
    {
        if(m_tip)
        {
            timer->stop();
            if(m_firstShow)
            {
                m_tip->show();
                m_tip->hide();
                m_firstShow=false;
            }
            QPoint pos = parentWidget()->mapToGlobal( mapToParent( QPoint(rect().center().x()-m_tip->width()/2-5,rect().top()-m_tip->height())));
            m_tip->move(pos);
            //m_tip->setGeometry(pos.x(),pos.y(),m_tip->width(),m_tip->height());
            m_tip->updateGeometry();
            m_tip->update() ;
            m_tip->repaint() ;
            timer->start();
        }
    }
        break;

    case QEvent::Leave:
        if(m_tip)
        {
            timer->stop();
            m_tip->hide();
        }
        break;

    default:
        break;
    }

    return QPushButton::event(event);
}

void KeyboardButton::setMtFlag(const QString&flag)
{
    m_mtFlag=flag;
}

void KeyboardButton::showMtFlag(bool show)
{
    m_showMtFlag=show;
}

void KeyboardButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event) ;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if(m_tip)
    {
        QRect rect = this->rect() ;

        painter.setPen(0x6329B6);
        painter.setBrush(0x6329B6);
        painter.drawRoundedRect(rect.adjusted(0,0,0,-10),8,8);
    }
    if(m_showMtFlag)
        painter.drawImage(0,0,QImage(m_mtFlag));
}
