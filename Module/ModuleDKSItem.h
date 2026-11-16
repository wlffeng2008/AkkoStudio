#ifndef MODULEDKSITEM_H
#define MODULEDKSITEM_H

#include <QTimer>
#include <QFrame>

namespace Ui {
class ModuleDKSItem;
}

class ModuleDKSItem : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleDKSItem(QWidget *parent = nullptr);
    ~ModuleDKSItem();

    void setText(const QString&text);
    void setData(quint8 data);
    quint32 getData();

signals:
    void onButtonClicked();

protected:
    bool eventFilter(QObject*watched, QEvent*event) override;

    void changeEvent(QEvent *pEvt) override;

private:
    Ui::ModuleDKSItem *ui;

};

#endif // MODULEDKSITEM_H
