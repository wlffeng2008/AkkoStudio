#ifndef MACROSQUARE_H
#define MACROSQUARE_H

#include <QWidget>
#include "SuperLabel.h"
#include <QSpinBox>

class MacroSquare : public QWidget
{
    Q_OBJECT
public:
    explicit MacroSquare(const QString&text, quint8 type,quint8 value, bool down=true, QWidget *parent = nullptr);
    bool isSelected(){ return m_bSelected; }
    void setSelected(bool selected=true){ m_bSelected = selected; update(); }
    void closeItems();
    void setDelay(quint16 delay);

    quint8  m_type = 0; // key, mouse, XY, delay
    bool    m_down=true;
    quint16 m_value=0;
    QString m_text;

    static void LostFocus();

protected:
    void paintEvent(QPaintEvent*event) override;
    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void onAction(MacroSquare *from, quint8 action);

private:
    bool m_bSelected = false;

    CustomTooltip *m_tTip=nullptr;
    CustomTooltip *m_rTip=nullptr;
    CustomTooltip *m_bTip=nullptr;
    QSpinBox *m_spin1=nullptr;
    QSpinBox *m_spin2=nullptr;
};

#endif // MACROSQUARE_H
