#ifndef MODULEDKSADJUST_H
#define MODULEDKSADJUST_H

#include <QFrame>

namespace Ui {
class ModuleDKSAdjust;
}

class ModuleDKSAdjust : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleDKSAdjust(QWidget *parent = nullptr);
    ~ModuleDKSAdjust();
    void setOriginValue(const QString&strValue);

signals:
    void onValueSave(const QString&text) ;

protected:
    void changeEvent(QEvent *pEvt) override;

private:
    Ui::ModuleDKSAdjust *ui;
};

#endif // MODULEDKSADJUST_H
