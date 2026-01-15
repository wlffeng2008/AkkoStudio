#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QLabel>
#include <QObject>
#include <QWidget>

class ColorLabel : public QLabel
{
    Q_OBJECT
public:
    ColorLabel(QWidget *parent=nullptr);
    void setColor(const QColor&color){ m_color = color; update(); } ;
    static ColorLabel *Current(QWidget *parent) ;
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *) override;

private:
    QColor m_color = 0xD9D9D9 ;
};

#endif // COLORLABEL_H
