#ifndef MACROSQUARE_H
#define MACROSQUARE_H

#include <QFrame>

#include "SuperLabel.h"
namespace Ui {
class MacroSquare;
}

class MacroSquare : public QFrame
{
    Q_OBJECT

public:
    explicit MacroSquare(QWidget *parent = nullptr);
    ~MacroSquare();

    void setData(const QString&text, quint8 type,quint16 value, bool down=true);
    bool isSelected(){ return m_bSelected; }
    void setSelected(bool selected=true){ m_bSelected = selected; update(); }
    void closeItems();
    void setDelay(quint16 delay);

    quint8  m_type = 0; // key, mouse, XY, delay
    bool    m_down = true;
    quint16 m_value= 0;
    QString m_text ;
    bool m_bConnected=false;

    static void LostFocus();
    static MacroSquare*getSquare(int index,QWidget *parent);

protected:
    bool eventFilter(QObject*object,QEvent *event) override;
    void paintEvent(QPaintEvent*event) override;
    bool event(QEvent *event) override;

signals:
    void onAction(MacroSquare *from, quint8 action);

private:
    Ui::MacroSquare *ui;

    bool m_bSelected=false;
    int m_xpos = 0;
    int m_ypos = 0;

    CustomTooltip *m_tTip=nullptr;
    CustomTooltip *m_rTip=nullptr;
    CustomTooltip *m_bTip=nullptr;
};

#endif // MACROSQUARE_H
