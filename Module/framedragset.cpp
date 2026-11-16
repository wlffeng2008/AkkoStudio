#include "framedragset.h"
#include "ui_framedragset.h"

#include <QPainter>
#include <QPaintEvent>
#include <QCursor>

FrameDragset::FrameDragset(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameDragset)
{
    ui->setupUi(this);


}

FrameDragset::~FrameDragset()
{
    delete ui;
}

void FrameDragset::setCursorShape(quint8 shape)
{
    static  quint8  setShape = 0;

    if(setShape == shape && shape != 0)
        return ;
    setShape = shape;
    switch(shape)
    {
    case 0:
        QApplication::restoreOverrideCursor();
        break;

    case 1:
        QApplication::setOverrideCursor(Qt::PointingHandCursor);
        break;

    case 2:
        QApplication::setOverrideCursor(Qt::SizeHorCursor);
        break;
    }
}

static int m_rectHW = 24;

void FrameDragset::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);


    //painter.setBrush(Qt::darkCyan);
    //painter.drawRoundedRect(rect().adjusted(0,0,-1,-1),10,10);

    for(int i=3; i>=0; i--)
    {
        m_rects[i].setHeight(m_rectHW);
        //qDebug() << m_rects[i];
        if(m_values[i])
        {
            painter.setPen(0x6329B6);
            painter.setBrush(0x6329B6);
            painter.drawRoundedRect(m_rects[i],m_rectHW/2,m_rectHW/2);
            painter.drawPixmap(m_rects[i].right()-20,4,QPixmap(":/images/k/ic_selected.png"));
        }
        else
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::NoBrush);
            painter.drawPixmap(m_rects[i],QPixmap(":/images/k/add-circle.png"));
        }
        //painter.setBrush(m_values[i] ? Qt::red : Qt::blue);
    }
}

void FrameDragset::leaveEvent(QEvent *event)
{
    setCursorShape(0);
    QFrame::leaveEvent(event);
}

void FrameDragset::showValue(quint8 value)
{
    m_values[0] = ((value >> 0) & 0x03);
    m_values[1] = ((value >> 2) & 0x03);
    m_values[2] = ((value >> 4) & 0x03);
    m_values[3] = ((value >> 6) & 0x03);

    m_rects[0].setWidth(m_rectHW);
    m_rects[1].setWidth(m_rectHW);
    m_rects[2].setWidth(m_rectHW);
    m_rects[3].setWidth(m_rectHW);

    if(m_values[0] == 2) m_rects[0].setWidth(m_step);
    if(m_values[1] == 2) m_rects[1].setWidth(m_step);
    if(m_values[2] == 2) m_rects[2].setWidth(m_step);

    if(m_values[1] == 0x03)
    {
        m_rects[1].setWidth(m_step * 2);
        m_rects[2].setWidth(m_rectHW);
    }

    if(m_values[0] == 0x03)
    {
        m_rects[0].setWidth(m_step * 2);
        m_rects[1].setWidth(m_rectHW);
    }

    if(m_values[0] == 0x03 && m_values[1] == 0x03)
    {
        m_rects[0].setWidth(m_step * 3);
        m_rects[1].setWidth(m_rectHW);
        m_rects[2].setWidth(m_rectHW);
    }

    update();
}

quint8 FrameDragset::combineValue()
{
    return m_combine;
}

void FrameDragset::mousePressEvent(QMouseEvent *event)
{
    m_pressed = true;

    m_clickPt = event->pos();
    if(m_hit != -1)
        m_clickpadding = m_rects[m_hit].width();
    //qDebug() << m_clickPt;

    QFrame::mousePressEvent(event);
}

void FrameDragset::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_hit != -1)
    {
        if(!m_draging)
        {
            m_values[m_hit] = !m_values[m_hit];
            int nW = m_rects[m_hit].width();

            if(m_hit == 0)
            {
                if(nW > 2*m_step)
                {
                    m_values[1] = 0;
                    m_values[2] = 0;
                }
                else if(nW >= m_step)
                {
                    m_values[1] = 0;
                }
            }

            if(m_hit == 1)
            {
                if(nW >= m_step)
                {
                    m_values[2] = 0;
                }
            }
        }
        else
        {
            int nW = event->pos().x() - m_clickPt.x() + m_clickpadding;

            if(m_hit == 0)
            {
                if(m_clickpadding > 2*m_step)
                {
                    m_values[1] = 0;
                    m_values[2] = 0;
                }

                if(m_clickpadding > m_step)
                {
                    m_values[1] = 0;
                }

                if(m_clickpadding <= m_rectHW)
                {
                    m_values[0] = 1;
                }

                if(nW > m_step*2)
                {
                    m_values[0] = 3; // b 11
                    m_values[1] = 3; // b 11
                    m_values[2] = 2; // b 10
                }
                else if(nW > m_step)
                {
                    m_values[0] = 3; // b 11
                    m_values[1] = 2; // b 10
                }
                else if(nW > m_rectHW)
                {
                    m_values[0] = 2; // b 10
                }
                else
                {
                    m_values[0] = 1;
                }
            }

            if(m_hit == 1)
            {
                if(m_clickpadding<=m_rectHW)
                {
                    m_values[1] = 1;
                }

                if(nW>m_step)
                {
                    m_values[1] = 3; // b 11
                    m_values[2] = 2; // b 10
                }
                else if(nW>m_rectHW)
                {
                    m_values[1] = 2 ; // b 10
                }
                else
                {
                    m_values[1] = 1;
                }
            }

            if(m_hit == 2)
            {
                if(nW>m_rectHW)
                {
                    m_values[2] = 2 ; // b 10
                }
                else
                {
                    m_values[2] = 1;
                }
            }

        }
    }
    quint8 value = ((m_values[3] << 6) | ( m_values[2] << 4) | ( m_values[1] << 2) | ( m_values[0]));
    showValue(value);
    m_combine = value;

    //qDebug() << QString::asprintf("0x%02X",value);
    update();
    m_draging = false;
    m_pressed = false;
    setCursorShape(0);

    QFrame::mouseReleaseEvent(event);
}

void FrameDragset::mouseMoveEvent(QMouseEvent *event)
{
    int move = event->pos().x() - m_clickPt.x();
    if(m_draging)
    {
        int nW = move + m_clickpadding;
        if(nW<m_rectHW) nW = m_rectHW;

        if(m_hit == 0)
            if(nW>m_step*3) nW = m_step*3;
        if(m_hit == 1)
            if(nW>m_step*2) nW = m_step*2;
        if(m_hit == 2)
            if(nW>m_step)   nW = m_step;

        m_rects[m_hit].setWidth(nW);
        update();
    }
    else
    {
        m_hit = -1;
        quint8 shape = 0;
        for(int i=0; i<4; i++)
        {
            if(m_rects[i].contains(event->pos()))
            {
                m_hit = i;
                shape = 1;

                if(m_values[i] && abs(m_rects[i].right() - event->pos().x()) <= 10)
                    shape = 2;

                break;
            }
        }

        setCursorShape(shape);
    }

    if(m_pressed && abs(move) >= 2)
    {
        if(m_hit != -1 && m_hit != 3 && m_values[m_hit])
            m_draging = true;
    }

    QFrame::mouseMoveEvent(event);
}

void FrameDragset::resizeEvent(QResizeEvent*event)
{
    //qDebug() << "resizeEvent:" << rect();
    int step = (rect().width() - m_rectHW - 10) / 3;
    m_step = step;
    for(int i=0; i<4; i++)
    {
        m_rects[i].setX(step*i + 5);
        m_rects[i].setY(2);
        //m_rects[i].setSize(QSize(m_rectHW,m_rectHW));
    }

    //showValue(0x55);

    QFrame::resizeEvent(event);
}