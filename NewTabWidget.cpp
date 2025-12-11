#include "NewTabWidget.h"

#include <QMouseEvent>
#include <QStyleOptionTab>
#include <QApplication>
#include <QPainter>
#include <QRect>

CustomTabBar::CustomTabBar(QWidget *parent)
    : QTabBar(parent), m_currentX(0), m_currentWidth(0), m_hoverIndex(-1)
{
    // 初始化颜色
    m_normalColor = QColor(240, 240, 240);
    m_hoverColor = QColor(220, 220, 220);
    m_selectedColor = QColor(255, 255, 255);

    // 创建动画
    m_animation = new QPropertyAnimation(this, "currentX", this);
    m_animation->setDuration(300);
    m_animation->setEasingCurve(QEasingCurve::OutQuart);

    QPropertyAnimation *widthAnim = new QPropertyAnimation(this, "currentWidth", this);
    widthAnim->setDuration(300);
    widthAnim->setEasingCurve(QEasingCurve::OutQuart);

    connect(this, &QTabBar::currentChanged, this, &CustomTabBar::onCurrentChanged);

    // 初始更新选中状态
    onCurrentChanged(currentIndex());

    setCursor(Qt::PointingHandCursor) ;
}

void CustomTabBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const int tabCount = count();

    // 绘制背景
    //painter.fillRect(rect(), QColor(230, 230, 230));

    // 绘制选中标签指示器
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(52, 152, 219));
    painter.drawRoundedRect(m_currentX, height() - 3, m_currentWidth, 3, 1.5, 1.5);

    for (int i = 0; i < tabCount; ++i)
    {
        bool isSelected = (currentIndex() == i);
        bool isHover = (m_hoverIndex == i);

        painter.setPen(Qt::NoPen);
        if (isSelected) {
            painter.setBrush(m_selectedColor);
        } else if (isHover) {
            painter.setBrush(m_hoverColor);
        } else {
            painter.setBrush(m_normalColor);
        }

        QRect tRect = tabRect(i);
        //if(isHover)
        //painter.drawRect(tRect);
        QColor setColor= !isSelected ? Qt::black : 0x6329B6 ;
        if(isHover && !isSelected)
            setColor = Qt::blue ;

        painter.setPen(setColor);
        QFont font = static_cast<QWidget*>(this->parent())->font();
        //font.setBold(true) ;
        //font.setBold(isSelected);
        font.setWeight(isSelected?QFont::ExtraBold : QFont::Normal) ;
        painter.setFont(font);

        QString text = tabText(i);
        QRect textRect = tRect.adjusted(0, 2, 0, -5);
        painter.drawText(textRect, Qt::AlignHCenter|Qt::AlignTop, text);

        if(isSelected)
        {
            QFontMetrics fm(painter.font());
            int textWidth = fm.boundingRect(text).width();
            QRect lineRect = tRect;
            lineRect.setTop(lineRect.bottom()-6) ;
            lineRect.setHeight(3) ;
            int nAdj = (tRect.width()-textWidth+30)/2 ;
            //if(tRect.width()<80)
                nAdj = 20 ;
            lineRect.adjust(nAdj,0,-nAdj,0) ;

            painter.setPen(setColor);
            painter.setBrush(setColor);
            painter.drawRoundedRect(lineRect,1.5,1.5);
        }
    }
}

void CustomTabBar::resizeEvent(QResizeEvent *event)
{
    QTabBar::resizeEvent(event);
    onCurrentChanged(currentIndex());
}

// QSize CustomTabBar::tabSizeHint(int index) const
// {
//     return QSize(120, 40); // 固定标签大小
// }

void CustomTabBar::tabLayoutChange()
{
    QTabBar::tabLayoutChange();
    onCurrentChanged(currentIndex()); // 布局变化时更新位置
}

void CustomTabBar::onCurrentChanged(int index)
{
    if (index >= 0 && index < count())
    {
        QRect rect = tabRect(index);

        m_animation->setStartValue(m_currentX);
        m_animation->setEndValue(rect.x());

        QPropertyAnimation *widthAnim = findChild<QPropertyAnimation*>("currentWidth");
        if (widthAnim) {
            widthAnim->setStartValue(m_currentWidth);
            widthAnim->setEndValue(rect.width());
            widthAnim->start();
        }

        m_animation->start();
    }
}

// 重写鼠标事件实现悬停效果
bool CustomTabBar::event(QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        int index = tabAt(mouseEvent->pos());
        if (index != m_hoverIndex)
        {
            m_hoverIndex = index;
            update();
        }
        return true;
    }
    else if (event->type() == QEvent::Leave)
    {
        m_hoverIndex = -1;
        update();
        return true;
    }
    return QTabBar::event(event);
}

NewTabWidget::NewTabWidget(QWidget *parent):QTabWidget(parent)
{
   setTabBar(new CustomTabBar(this));
}
