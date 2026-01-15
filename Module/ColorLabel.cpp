#include "ColorLabel.h"

#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QMap>

static QMap<QObject *,ColorLabel *>s_map ;

ColorLabel::ColorLabel(QWidget *parent):QLabel(parent)
{
    if(!s_map[parent]) s_map[parent] = this ;

    setCursor(Qt::PointingHandCursor) ;
}

ColorLabel *ColorLabel::Current(QWidget *parent)
{
    return s_map[parent] ;
}

void ColorLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this) ;
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect rect = this->rect().adjusted(2,2,-2,-2) ;
    int nOffset = rect.height()/2;
    if(rect.height()>rect.width())
        nOffset = rect.width()/2;

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_color);

    int a = 4 ;
    painter.drawRoundedRect(rect.adjusted(a,a,-a,-a),nOffset-a,nOffset-a);

    if(s_map[parent()] == this)
    {
        painter.setPen(QPen(m_color,2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect,nOffset,nOffset);
    }
    event->accept() ;
}

void ColorLabel::mousePressEvent(QMouseEvent *)
{
    ColorLabel *old = s_map[parent()] ;
    s_map[parent()] = this ;
    update() ;
    if(old) old->update();
}
