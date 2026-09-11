#include "CustumSlider.h"

#include <QPainter>
#include <QMouseEvent>

CustumSlider::CustumSlider(QWidget *parent):QSlider(parent)
{

}

void CustumSlider::updateValue()
{
    int newValue = 0;
    if(this->orientation() == Qt::Horizontal)
    {
        int w = width();
        newValue = ((int)round(mapFromGlobal(cursor().pos()).x() * (maximum() - minimum()) * 1.0 / w + minimum()));
    }
    else
    {
        int h = height();
        newValue = ((int)round((h - mapFromGlobal(cursor().pos()).y()) * (maximum() - minimum()) * 1.0 / h + minimum()));
    }

    this->setValue( newValue );
}

void CustumSlider::setValue(int value)
{
    int clip = m_clipStep;
    if(clip<1) clip = 1;
    int newValue =  value / clip * clip;

    QSlider::setValue(newValue);
}


void CustumSlider::setValue(const QString&value)
{
    int val = value.toInt();

    this->setValue( val );
}

QString CustumSlider::getValue()
{
    return QString("%1").arg(QSlider::value());
}

void CustumSlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);

    if(event->button() == Qt::LeftButton)
    {
        updateValue();
        m_draging = true;
    }
}

void CustumSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QSlider::mouseReleaseEvent(event);

    m_draging = false;
}

void CustumSlider::mouseMoveEvent(QMouseEvent *event)
{
    QSlider::mouseMoveEvent(event);

    if(m_draging) updateValue();
}

void CustumSlider::paintEvent(QPaintEvent *ev)
{
    QSlider::paintEvent(ev);

    if(m_showMark)
    {
        int count = maximum() - minimum() + 1;
        if(count<2)
            return;
        float step = width()*1.0/(count-1);
        if(count>100 || step<5)
            return;

        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor("#6E6E6E")));
        for(int i=0; i<=count; i++)
        {
            float x = i*step-i;
            if(x>width()) x = width()-6;
            QRect rect(x,height()/2-3,6,6);
            painter.drawRoundedRect(rect,3,3);
        }
    }

}
