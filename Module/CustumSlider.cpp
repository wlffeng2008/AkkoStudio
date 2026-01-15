#include "CustumSlider.h"

#include <QPainter>
#include <QMouseEvent>

CustumSlider::CustumSlider(QWidget *parent):QSlider(parent)
{

}

void CustumSlider::updateValue()
{
    if(this->orientation() == Qt::Horizontal)
    {
        int w = width() ;
        setValue(mapFromGlobal(cursor().pos()).x()*(maximum() - minimum()) / w + minimum()) ;
    }
    else
    {
        int h = height() ;
        setValue((h - mapFromGlobal(cursor().pos()).y()) * (maximum() - minimum()) / h + minimum()) ;
    }
    update() ;
}

void CustumSlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);

    if(event->button() == Qt::LeftButton)
    {
        updateValue();
        m_draging = true ;
    }
}

void CustumSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QSlider::mouseReleaseEvent(event);
    m_draging = false ;
}

void CustumSlider::mouseMoveEvent(QMouseEvent *event)
{
    QSlider::mouseMoveEvent(event);
    if(m_draging) updateValue();
}

void CustumSlider::paintEvent(QPaintEvent *ev)
{
    QSlider::paintEvent(ev) ;

    QPainter painter(this) ;

}
