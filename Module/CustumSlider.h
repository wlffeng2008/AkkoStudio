#ifndef CUSTUMSLIDER_H
#define CUSTUMSLIDER_H

#include <QSlider>

class CustumSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(bool redrawMode READ getRedrawMode WRITE setRedrawMode)

public:
    CustumSlider(QWidget *parent=nullptr);
    void setClipStep(int step){m_clipStep=step;};
    int  clipStep(){ return m_clipStep; }

    void setValue(int value);
    void setValue(const QString&value);
    QString getValue();

    void setShowMark(bool show=true){m_showMark=show;}

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

    bool m_showMark=false;
    int  m_clipStep=1;
    void updateValue();
};

#endif // CUSTUMSLIDER_H
