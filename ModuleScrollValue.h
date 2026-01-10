#ifndef MODULESCROLLVALUE_H
#define MODULESCROLLVALUE_H

#include <QFrame>
#include <QEvent>

namespace Ui {
class ModuleScrollValue;
}

class ModuleScrollValue : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleScrollValue(QWidget *parent = nullptr);
    ~ModuleScrollValue();
    void setValueList(QStringList&valus);

    void setIndex(quint32 index);
    quint32 getIndex();

signals:
    void onIndexChanged(int index);

protected:
    bool event(QEvent*event) override;
    bool eventFilter(QObject*watched, QEvent*event) override;

private:
    Ui::ModuleScrollValue *ui;
    QStringList m_Values ;
    qint64 m_nIndex = 0;
    qint64 m_nPick = 0;
    void rollValues(bool notify=true);
};

#endif // MODULESCROLLVALUE_H
