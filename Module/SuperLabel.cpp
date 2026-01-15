#include "SuperLabel.h"
#include <QEvent>

static QMap<QObject *,SuperLabel *>s_group ;
static QMap<QObject *,QString>s_tipStyle ;

static QString s_strDefTipStyle(R"(
    background-color: white;
    border: 1px solid #DEDEDE;
    border-radius: 16px;
    padding-left: 16px;
    padding-right: 16px;
    min-height:46px;
    max-height:46px;
    font-size: 16px;
    font-weight:400;
)");

void CustomTooltip::setDefTipStyle(const QString &stryle)
{
    s_strDefTipStyle = stryle ;
}

void CustomTooltip::setGroupTipStyle(QObject *parent, QString&style)
{
    s_tipStyle[parent] = style;
}

void SuperLabel::setGroupTipStyle(QObject *parent, QString&style)
{
    s_tipStyle[parent] = style;
}

CustomTooltip::CustomTooltip(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet(s_strDefTipStyle);

    content = new QLabel(this);
    content->setWordWrap(true);
    content->setAlignment(Qt::AlignCenter);

    m_timer = new QTimer(this) ;
    connect(m_timer,&QTimer::timeout,this,[=]{
        if(m_bAutohide)
        {
            m_timer->stop();
            hide();
        }
    });
}

void CustomTooltip::enterEvent(QEnterEvent *event)
{
    m_timer->stop();
    QWidget::enterEvent(event);
}

void CustomTooltip::leaveEvent(QEvent *event)
{
    m_timer->stop();
    m_timer->start(1000);
    QWidget::leaveEvent(event);
}

void CustomTooltip::mousePressEvent(QMouseEvent *event)
{
    emit onClicked();

    hide() ;
    m_timer->stop();
    QWidget::mousePressEvent(event) ;
}

void CustomTooltip::focusOutEvent(QFocusEvent *event)
{
    hide() ;
    QWidget::focusOutEvent(event) ;
}

void CustomTooltip::showEvent(QShowEvent *event)
{
    m_timer->stop() ;
    m_timer->start(3000) ;
    QWidget::showEvent(event) ;
}

void CustomTooltip::setText(const QString&text)
{
    content->setText(text);
    content->adjustSize();
    adjustSize();
    content->update(); 
}

void CustomTooltip::setTextStyle(const QString& stryle)
{
    content->setStyleSheet(stryle);
}


//-------------------------------------

SuperLabel::SuperLabel(QWidget *parent)
    : QLabel{parent}
{
    if(!s_group[parent]) s_group[parent] = this ;

    QTimer::singleShot(100,this,[=]{
        tooltip = new CustomTooltip(this);
        tooltip->setText(this->toolTip());

        timer = new QTimer(this) ;
        timer->setSingleShot(true);
        timer->setInterval(100);

        m_bEmpty = this->toolTip().isEmpty() ;
        setToolTip("") ;

        connect(timer, &QTimer::timeout, this, [=]() {
            if(m_bEmpty)  return ;
            QPoint pos = mapToGlobal(QPoint(width()+5,(height() - tooltip->height())/2));
            tooltip->move(pos);
            tooltip->show();
        });
    });
    setAlignment(Qt::AlignCenter) ;
}

bool SuperLabel::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::Enter:
        timer->start();
        break;

    case QEvent::Leave:
        timer->stop();
        tooltip->hide();
        break;

    case QEvent::MouseButtonPress:
        tooltip->hide();
        break;

    default:
        break;
    }
    return QLabel::event(event);
}

void SuperLabel::setOwnSheet(const QString&strGetfocus,const QString&strLostfocus)
{
    m_strSheetGetfocus = strGetfocus;
    m_strSheetLostfocus= strLostfocus;
}

void SuperLabel::setImages(const QString&strGetfocus, const QString&strLostfocus)
{
    m_strGetfocus  = strGetfocus;
    m_strLostfocus = strLostfocus;
    setFocus(false) ;
}

void SuperLabel::setFocus(bool foucs)
{
    m_bFoucs = foucs ;
    //setScaledContents(true) ;
    setPixmap(QPixmap(foucs ? m_strGetfocus : m_strLostfocus));
    setStyleSheet(foucs ? m_strSheetGetfocus : m_strSheetLostfocus);
    if(!foucs)
        return ;

    SuperLabel *pLast = s_group[this->parent()] ;
    if(pLast && pLast != this)
        pLast->setFocus(false) ;
    s_group[this->parent()] = this ;
}
