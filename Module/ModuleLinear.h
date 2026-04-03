#ifndef MODULELINEAR_H
#define MODULELINEAR_H

#include <QFrame>

namespace Ui {
class ModuleLinear;
}

class ModuleLinear : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleLinear(QWidget *parent = nullptr);
    ~ModuleLinear();

    void setText(const QString&text1,const QString&text2);

signals:
    void onSetValue(float value,int type=0);

protected:
    bool eventFilter(QObject*watched, QEvent*event) override;

private:
    Ui::ModuleLinear *ui;
};

#endif // MODULELINEAR_H
