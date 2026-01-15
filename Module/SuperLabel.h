#ifndef SUPERLABEL_H
#define SUPERLABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QDebug>

class CustomTooltip : public QWidget
{
    Q_OBJECT
public:
    explicit CustomTooltip(QWidget *parent = nullptr);
    void setText(const QString&text);

    void setTextStyle(const QString& stryle) ;
    void setAutohide(bool bSet=true){m_bAutohide = bSet;}

    static void setDefTipStyle(const QString& stryle) ;
    static void setGroupTipStyle( QObject *parent, QString&style) ;

protected:
    void mousePressEvent(QMouseEvent *event) override ;
    void focusOutEvent(QFocusEvent *event) override ;
    void showEvent(QShowEvent *event) override ;
    void enterEvent(QEnterEvent *event) override ;
    void leaveEvent(QEvent *event) override ;

signals:
    void onClicked() ;

private:
    QLabel *content = nullptr;
    QTimer *m_timer = nullptr;
    bool m_bAutohide= true;
};

class SuperLabel : public QLabel
{
    Q_OBJECT
public:
    explicit SuperLabel(QWidget *parent = nullptr);

    void setImages(const QString&strGetfocus,const QString&strLostfocus);
    void setOwnSheet(const QString&strGetfocus,const QString&strLostfocus);
    void setFocus(bool foucs = true) ;
    bool getFocus(){ return m_bFoucs; }
    CustomTooltip *getToolTip(){ return tooltip; }
    static void setGroupTipStyle(QObject *parent, QString &style) ;

protected:
    bool event(QEvent *event) override;

private:
    CustomTooltip *tooltip;
    QTimer *timer;

    bool m_bFoucs = false ;
    bool m_bEmpty = false ;

    QString m_strGetfocus ;
    QString m_strLostfocus ;
    QString m_strSheetGetfocus ;
    QString m_strSheetLostfocus ;
};

#endif // SUPERLABEL_H
