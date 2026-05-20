#include "ColorSquare.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>

ColorSquare::ColorSquare(QWidget *parent) :
    QFrame(parent),huem(0),sat(0),val(0),
    colorChar('H')
{
    setCursor(Qt::CrossCursor);
    clickX = 124;
    clickY = 124;
}

QColor ColorSquare::color() const
{
    return QColor::fromHsvF(huem,sat,val);
}

void ColorSquare::setColor(QColor c)
{
    huem = c.hueF();
    if ( huem < 0 ) huem = 0;
    sat = c.saturationF();
    val = c.valueF();

    update();
}

void ColorSquare::setHue(qreal h)
{
    m_notify = true ;
    huem = qMax(0.0,qMin(1.0,h));
    update();
}

void ColorSquare::setSaturation(qreal s)
{
    sat = qMax(0.0, qMin(1.0, s));
    update();
}

void ColorSquare::setValue(qreal v)
{
    val = qMax(0.0, qMin(1.0, v));
    update();
}

void ColorSquare::setCheckedColor(char checked)
{
    colorChar = checked;
    update();
}

void ColorSquare::RenderRectangle()
{
    int w = width();
    int h = height();
    m_colorImg = QImage(w+1,h+1, QImage::Format_RGB32);
    for(int i = 0; i <= w; i++)
    {
        for(int j = 0; j <= h; j++)
        {
            switch(colorChar)
            {
            case 'H':
                m_colorImg.setPixel(i,j, QColor::fromHsvF(huem, double(i)/w, double(h-j)/h).rgb());
                break;

            case 'S':
                m_colorImg.setPixel(i,j, QColor::fromHsvF(double(i)/w, sat, double(h-j)/h).rgb());
                break;

            case 'V':
                m_colorImg.setPixel(i,j, QColor::fromHsvF(double(i)/w, double(h-j)/h, val).rgb());
                break;

            case 'R':
            {
                qreal r = QColor::fromHsvF(huem, sat, val).redF();
                m_colorImg.setPixel(i,j, QColor::fromRgbF(r, double(i)/w, double(h-j)/h).rgb());
                break;
            }

            case 'G':
            {
                qreal g = QColor::fromHsvF(huem, sat, val).greenF();
                m_colorImg.setPixel(i,j, QColor::fromRgbF(double(i)/w, g, double(h-j)/h).rgb());
                break;
            }

            case 'B':
            {
                qreal b = QColor::fromHsvF(huem, sat, val).blueF();
                m_colorImg.setPixel(i,j, QColor::fromRgbF(double(i)/w, double(h-j)/h, b).rgb());
                break;
            }
            }
        }
    }

    if(m_notify)
        emit colorSelected(m_colorImg.pixelColor(clickX,clickY));
    m_notify = false ;
}

void ColorSquare::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    RenderRectangle();

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_colorImg);
    painter.drawImage(0, 0, m_colorImg);
    //painter.drawRoundedRect(rect(),20,20);

    double selectorWidth = 8;
    painter.setPen(QPen(val > 0.5 ? Qt::black : Qt::white, 3));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPointF(clickX, clickY), selectorWidth, selectorWidth);
}

void ColorSquare::mousePressEvent(QMouseEvent *ev)
{
    mouseMoveEvent(ev);
    //ColorSquare::mousePressEvent(ev);
}

void ColorSquare::mouseMoveEvent(QMouseEvent *ev)
{
    clickX = ev->pos().x();
    clickY = ev->pos().y();

    if(clickX < 0) clickX = 0;
    if(clickY < 0) clickY = 0;

    if(clickX > size().width())  clickX = size().width()  ;
    if(clickY > size().height()) clickY = size().height() ;

    update();

    emit colorSelected(m_colorImg.pixelColor(clickX,clickY));
    //ColorSquare::mouseMoveEvent(ev);

}
