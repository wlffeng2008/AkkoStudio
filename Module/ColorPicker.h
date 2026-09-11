#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>
#include <QPointF>
#include <QColor>

class ColorPicker : public QWidget
{
    Q_OBJECT
public:
    explicit ColorPicker(QWidget *parent = nullptr);
    QColor selectedColor() const;

signals:
    void colorChanged(const QColor &c);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    // 色板区域
    QRect m_svRect;
    // 明度条区域
    QRect m_vRect;

    QPointF m_cursorPos; // 色板上圆圈位置
    double m_hue = 0.0;
    double m_sat = 1.0;
    double m_val = 1.0;

    bool m_bDraggingSV = false;
    bool m_bDraggingV = false;

    void updateColorFromPos(const QPoint &pos);
    void drawSVPanel(QPainter *p);
    void drawVSlider(QPainter *p);
};

#endif // COLORPICKER_H
