#ifndef FRAMEDRAGSET_H
#define FRAMEDRAGSET_H

#include <QFrame>

namespace Ui {
class FrameDragset;
}

class FrameDragset : public QFrame
{
    Q_OBJECT

public:
    explicit FrameDragset(QWidget *parent = nullptr);
    ~FrameDragset();

    void showValue(quint8 value);
    quint8 combineValue();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent*event) override;

private:
    Ui::FrameDragset *ui;

    quint8 m_combine = 0;
    int m_step = 0;
    int m_values[4]={0};
    QRect m_rects[4];
    int m_hit = 0;
    bool m_draging = false;
    bool m_pressed = false;
    int m_clickpadding=0;
    QPoint m_clickPt;

    void setCursorShape(quint8 shape=0);
};

#endif // FRAMEDRAGSET_H
