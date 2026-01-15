#ifndef COLORSQUARE_H
#define COLORSQUARE_H

#include <QFrame>

class ColorSquare : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(qreal hue READ hue WRITE setHue DESIGNABLE false )
    Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation DESIGNABLE false )
    Q_PROPERTY(qreal value READ value WRITE setValue DESIGNABLE false )

private:
    qreal huem=0, sat=0, val=0;

    QImage colorSquare;
    char    colorChar;
    qreal clickX, clickY;

    enum CheckedColor
    {
        H,S,V,R,G,B
    }checkedColor;

    enum MouseStatus
    {
        Nothing,
        DragSquare
    }mouseStatus;

public:
    explicit ColorSquare(QWidget *parent = 0);
    QColor color() const;
    qreal hue() const {return huem;}
    qreal saturation() const {return sat;}
    qreal value() const {return val;}

public slots:
    void setColor(QColor c);
    void setHue(qreal h);
    void setSaturation(qreal s);
    void setValue(qreal v);

    void setCheckedColor(char checked);

signals:
    void colorSelected(QColor);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:
    bool m_notify = false ;
    void RenderRectangle();
};

#endif // COLORSQUARE_H
