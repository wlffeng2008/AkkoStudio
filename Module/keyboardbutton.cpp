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
    if(label1)
    {
        label1->setText(text);
        label1->adjustSize();
    }
}

void keySetTooltip::setText2(const QString&text)
{
    if(label2)
    {
        label2->setText(text);
        label2->adjustSize();
    }
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

static QString strStyle1(R"(
        QPushButton {
                border: 1px solid #EAEAEA;
                color: black;
                background-color: #FBFBFB;
                border-radius: 14px;
                padding: 2px 2px;
                outline: none;
                min-width:42px;
                max-width:542px;
                min-height:42px;
            }

        QPushButton:hover { background-color: #EAEAEA; border: 1px solid #EAEAEA;}
        QPushButton:pressed { background-color: #3F3F3F; }
        QPushButton:checked { background-color: #3F3F3F; color: white; }
        QPushButton:disabled { background-color: #EAEAEA; color: #8C8C8C; }
    )");

static QString strStyle2(R"(
        QPushButton {
                border: 1px solid #EAEAEA;
                color: white;
                background-color: #6329B6;
                border-radius: 14px;
                padding: 2px 2px;
                outline: none;
                min-width:42px;
                max-width:542px;
                min-height:42px;
            }

        QPushButton:hover { background-color: #6329B6; border: 1px solid #EAEAEA;}
        QPushButton:pressed { background-color: #6329B6; }
        QPushButton:checked { background-color: #6329B6; color: white; }
        QPushButton:disabled { background-color: #6329B6; color:white; }
    )");

void KeyboardButton::setTipText(const QString&strText1,const QString&strText2)
{
    if(strText1.isEmpty() || strText2.isEmpty())
    {
        setStyleSheet(strStyle1);
        if(m_tip) delete m_tip ; m_tip = nullptr ;
        return ;
    }

    if(!m_tip) m_tip = new keySetTooltip(this);

    m_tip->setText1(strText1);
    m_tip->setText2(strText2);
    setStyleSheet(strStyle2);
}

bool KeyboardButton::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Enter:
    {
        if(m_tip)
        {
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
            m_tip->show();
        }
    }
        break;

    case QEvent::Leave:
        if(m_tip)
        {
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
    QPushButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if(m_showMtFlag)
        painter.drawImage(0,0,QImage(m_mtFlag));
}
