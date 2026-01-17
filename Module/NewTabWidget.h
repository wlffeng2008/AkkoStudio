#ifndef NEWTABWIDGET_H
#define NEWTABWIDGET_H

#include <QTabWidget>
#include <QTabBar>
#include <QPropertyAnimation>

class CustomTabBar : public QTabBar
{
    Q_OBJECT
    Q_PROPERTY(int currentX READ currentX WRITE setCurrentX)
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)

public:
    explicit CustomTabBar(QWidget *parent = nullptr);

    int currentX() const { return m_currentX; }
    int currentWidth() const { return m_currentWidth; }
    void setCurrentX(int x) { m_currentX = x; update(); }
    void setCurrentWidth(int w) { m_currentWidth = w; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    //QSize tabSizeHint(int index) const override;
    void tabLayoutChange() override;
    bool event(QEvent *event)override;

private slots:
    void onCurrentChanged(int index);

private:
    int m_currentX;
    int m_currentWidth;
    QPropertyAnimation *m_animation;
    QColor m_normalColor;
    QColor m_hoverColor;
    QColor m_selectedColor;
    int m_hoverIndex;
};


class NewTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    NewTabWidget(QWidget *parent = nullptr);
};

#endif // NEWTABWIDGET_H
