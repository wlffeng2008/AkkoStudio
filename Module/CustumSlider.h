#ifndef CUSTUMSLIDER_H
#define CUSTUMSLIDER_H

#include <QSlider>

class CustumSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(bool redrawMode READ getRedrawMode WRITE setRedrawMode)

public:
    CustumSlider(QWidget *parent=nullptr);

    void setRedrawMode(bool set=true){ m_set = set; };
    bool getRedrawMode(){ return m_set; } ;
protected:
    void paintEvent(QPaintEvent *ev) override;
    void mousePressEvent(QMouseEvent *event) override ;
    void mouseReleaseEvent(QMouseEvent *event) override ;
    void mouseMoveEvent(QMouseEvent *event) override ;
private:
    bool m_set = false ;
    bool m_draging = false ;
    void updateValue() ;
};

#endif // CUSTUMSLIDER_H
