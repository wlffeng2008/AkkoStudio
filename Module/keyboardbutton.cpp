#include "keyboardbutton.h"
#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QFrame>

keySetTooltip::keySetTooltip(QWidget *parent):QWidget(parent)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    label1 = new QLabel(this);
    label2 = new QLabel(this);
    midLine = new QFrame(this);
    //midLine->setLineWidth(2);
    midLine->setFixedHeight(2);

    label1->setAlignment(Qt::AlignCenter);
    label2->setAlignment(Qt::AlignCenter);
    label1->setStyleSheet("QLabel{color:black;background-color:transparent;}");
    label2->setStyleSheet("QLabel{color:black;background-color:transparent;}");
    midLine->setStyleSheet("QFrame{color:black;background-color:black;}");

    QVBoxLayout *pBox = new QVBoxLayout(this) ;
    pBox->addWidget(label1);
    pBox->addWidget(midLine);
    pBox->addWidget(label2);
    pBox->setContentsMargins(10,8,10,16);
    pBox->setSpacing(4);

    label1->hide();
    midLine->hide();
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

    painter.setPen(0xD8BDFF);
    painter.setBrush(0xFAF7FF);
    painter.drawRoundedRect(rect.adjusted(0,0,0,-10),12,12);

    QRect triRC = this->rect() ;
    triRC.setTop(triRC.bottom()-10);
    int nImgX = rect.center().x() ;
    int nTriLen = 14;
    QPolygonF triangle;
    triangle<<QPoint(nImgX,triRC.top()) ;
    triangle<<QPoint(nImgX+nTriLen,triRC.top()) ;
    triangle<<QPoint(nImgX+nTriLen/2,triRC.top() + nTriLen/2 * sqrt(3)) ;
    painter.drawPolygon(triangle);

    painter.setBrush(0xFAF7FF);
    painter.setPen(0xFAF7FF);
    painter.drawLine(QPoint(nImgX,triRC.top()),QPoint(nImgX+nTriLen,triRC.top()));
    painter.fillRect(nImgX,triRC.top()-2,nTriLen,3,0xFAF7FF);

    painter.setBrush(Qt::black);
    painter.setPen(Qt::black);
    //painter.drawLine(rect.left()+10,rect.center().y()-5,rect.right()-10,rect.center().y()-5);
}

//----------------------------------------

KeyboardButton::KeyboardButton(QWidget *parent):QPushButton(parent)
{
    m_mtFlag = ":/images/mt0.png" ;
    setTipText();
}

static QString strBasic(R"(
    QPushButton {
            color: black;
            border: 1px solid #EAEAEA;
            background-color: #FBFBFB;
            padding: 2px 2px;
            outline: none;
            font-weight: 600;

            [==set==]
        }

    QPushButton:hover    { background-color: #D8D8D8; border: 1px solid #EAEAEA;}
    QPushButton:pressed  { background-color: #3F3F3F; color: white; }
    QPushButton:checked  { background-color: #3F3F3F; color: white; }
    QPushButton:disabled { background-color: #EAEAEA; color: gray; }
)");


static QString strSetTip(R"(
    QPushButton {
            color: black;
            border: 1px solid #D8BDFF;
            background-color: #FAF7FF;
            padding: 2px 2px;
            outline: none;
            font-weight:600;

            [==set==]
        }

    QPushButton:hover    { background-color: #D8D8D8; border: 1px solid #EAEAEA;}
    QPushButton:pressed  { background-color: #D8BDFF; color: white; }
    QPushButton:checked  { background-color: #D8BDFF; color: white; }
    QPushButton:disabled { background-color: #EAEAEA; color: gray; }
)");


static QString strVolP(R"(

    min-width:20px !important;
    max-width:20px !important;
    border-radius: 0px;
    border-top-left-radius: 20px !important;
    border-bottom-left-radius: 20px !important;

)");

static QString strVolM(R"(

    min-width:20px !important;
    max-width:20px !important;
    border-radius: 0px;
    border-top-right-radius: 20px !important;
    border-bottom-right-radius: 20px !important;

)");


bool KeyboardButton::hasTip()
{
    return  (m_tip != nullptr) ;
}

void KeyboardButton::setTipText(const QString&strText1,const QString&strText2)
{
    QString strName=objectName();
    QString strSet("");
    if(strName.contains("_Hid234"))
        strSet = strVolP ;
    if(strName.contains("_Hid233"))
        strSet = strVolM ;

    if(strText1.isEmpty() || strText2.isEmpty())
    {
        QString strStyle = strBasic ;
        strStyle.replace("[==set==]",strSet);
        setStyleSheet(strStyle);
        if(m_tip)
            delete m_tip ;
        m_tip = nullptr ;
        return ;
    }

    if(!m_tip) m_tip = new keySetTooltip(this);

    m_tip->setText1(strText1);
    m_tip->setText2(strText2);

    QString strStyle = strSetTip ;
    strStyle.replace("[==set==]",strSet);
    setStyleSheet(strStyle);
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
