#ifndef KEYBOARDBUTTON_H
#define KEYBOARDBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QFrame>


class keySetTooltip : public QWidget
{
    Q_OBJECT
public:
    explicit keySetTooltip(QWidget *parent = nullptr);
    void setText1(const QString&text);
    void setText2(const QString&text);

protected:
    void paintEvent(QPaintEvent *event) override ;

private:
    QLabel *label1 = nullptr;
    QLabel *label2 = nullptr;
    QFrame *midLine = nullptr;
    bool m_bAutohide= true;
};


class KeyboardButton : public QPushButton
{
    Q_OBJECT

public:
    KeyboardButton(QWidget *parent);
    void setMtFlag(const QString&flag);
    void showMtFlag(bool show=true);
    void setColor(QColor color);

    void setTipText(const QString&strText1="",const QString&strText2="");
    bool hasTip();

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *event) override;

private:
    QString m_mtFlag;
    bool m_showMtFlag=false;
    bool m_firstShow=true;
    bool m_colorMode=false;
    QColor m_color;

    keySetTooltip *m_tip = nullptr;
};

#endif // KEYBOARDBUTTON_H
