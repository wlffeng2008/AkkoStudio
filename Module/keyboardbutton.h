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
    void setMtFlag(const QColor&color);
    void showFlag(quint8 flag);
    void setColor(QColor color);
    void setDeathZone(float top,float bottom);
    void setKeyPress(float up,float down);
    void setKeyPressRt(float up,float down, bool bRtOn);

    void setTipText(const QString&strText1="", const QString&strText2="");
    bool hasTip();

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *event) override;

private:
    QColor m_mtColor=0x9B9B9B;
    quint8 m_showFlag=0;
    bool m_firstShow=true;
    bool m_colorMode=false;
    QColor m_color;

    QString m_strTop="0.30";
    QString m_strBtm="0.215";

    QString m_strUp="0.30";
    QString m_strDown="0.215";

    bool m_bRtOn = false;
    QString m_strRtUp="0.30";
    QString m_strRtDown="0.215";

    keySetTooltip *m_tip = nullptr;
};

#endif // KEYBOARDBUTTON_H
