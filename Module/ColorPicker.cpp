#include "ColorPicker.h"

#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
#include <QPainterPath>

ColorPicker::ColorPicker(QWidget *parent)
    : QWidget{parent}
{
    setMinimumSize(320,260);
}

QColor ColorPicker::selectedColor() const
{
    double hNorm = qBound(0.0, m_hue / 360.0, 1.0);
    double sNorm = qBound(0.0, m_sat, 1.0);
    double vNorm = qBound(0.0, m_val, 1.0);
    return QColor::fromHsvF(hNorm, sNorm, vNorm);
}

void ColorPicker::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    int margin = 1;
    // 上方SV面板
    m_svRect = QRect(margin, margin, width()-margin*2, height()*0.72);
    // 下方V明度条
    m_vRect = QRect(margin, m_svRect.bottom()+2, width()-margin*2, 24);
}

void ColorPicker::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(),18,18);
    painter.setClipPath(path);

    drawSVPanel(&painter);
    drawVSlider(&painter);

    QPoint pt(
        m_svRect.left() + m_hue / 360.0 * m_svRect.width(),
        m_svRect.top() + (1.0 - m_sat) * m_svRect.height()
        );
    painter.setPen(QPen(Qt::black,1));
    painter.drawEllipse(pt, 8,8);
    painter.setPen(QPen(Qt::white,1));
    painter.drawEllipse(pt, 6,6);
}

void ColorPicker::drawSVPanel(QPainter *p)
{
    QImage img(m_svRect.size(), QImage::Format_RGB32);
    for(int y=0;y<img.height();y++)
    {
        float sat = 1.0f - (float)y / img.height();
        QRgb *line = (QRgb*)img.scanLine(y);
        for(int x=0;x<img.width();x++)
        {
            float hue = 360.0f * (float)x / img.width();
            QColor c = QColor::fromHsvF(hue/360.0, sat, m_val);
            line[x] = c.rgb();
        }
    }
    p->drawImage(m_svRect, img);
}

void ColorPicker::drawVSlider(QPainter *p)
{
    QLinearGradient grad(m_vRect.left(),0, m_vRect.right(),0);
    grad.setColorAt(0, QColor::fromHsvF(m_hue/360.0, m_sat, 0.0));
    grad.setColorAt(1, QColor::fromHsvF(m_hue/360.0, m_sat, 1.0));
    p->fillRect(m_vRect, grad);

    int cx = m_vRect.left() + m_val * m_vRect.width();
    int cy = m_vRect.center().y();
    p->setPen(QPen(Qt::black,1));
    p->drawEllipse(QPoint(cx,cy), 8,8);
    p->setPen(QPen(Qt::white,1));
    p->drawEllipse(QPoint(cx,cy), 6,6);
}

void ColorPicker::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    if(m_svRect.contains(pos))
    {
        m_bDraggingSV = true;
        updateColorFromPos(pos);
    }
    else if(m_vRect.contains(pos))
    {
        m_bDraggingV = true;
        updateColorFromPos(pos);
    }
}

void ColorPicker::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bDraggingSV || m_bDraggingV)
    {
        updateColorFromPos(event->pos());
    }
}

void ColorPicker::updateColorFromPos(const QPoint &pos)
{
    if(m_svRect.contains(pos) && m_bDraggingSV)
    {
        double fx = (pos.x() - m_svRect.left()) / (double)m_svRect.width();
        double fy = (pos.y() - m_svRect.top()) / (double)m_svRect.height();
        m_hue = qBound(0.0, fx * 360.0, 360.0);
        m_sat = qBound(0.0, 1.0 - fy, 1.0);
    }
    else if(m_vRect.contains(pos) && m_bDraggingV)
    {
        double fv = (pos.x() - m_vRect.left()) / (double)m_vRect.width();
        m_val = qBound(0.0, fv, 1.0);
    }

    emit colorChanged(selectedColor());

    update();
}