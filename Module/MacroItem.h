#ifndef MACROITEM_H
#define MACROITEM_H

#include <QFrame>

namespace Ui {
class MacroItem;
}

class MacroItem : public QFrame
{
    Q_OBJECT

public:
    explicit MacroItem(QWidget *parent = nullptr);
    ~MacroItem();
    void setMacroName(const QString&strName);
    void setRelData(QObject *data){ m_data = data; };
    QObject *getRelData(){ return m_data; };
    QString getMarcoName();
    void setActive();
protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched,QEvent *event) override;

signals:
    void onOperation(int operation,QWidget *from);

private:
    Ui::MacroItem *ui;
    bool m_bInEditing=false;
    QObject *m_data = nullptr;
};

#endif // MACROITEM_H
