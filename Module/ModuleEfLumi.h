#ifndef MODULEEFLUMI_H
#define MODULEEFLUMI_H

#include <QFrame>

namespace Ui {
class ModuleEfLumi;
}

class ModuleEfLumi : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleEfLumi(QWidget *parent = nullptr);
    ~ModuleEfLumi();
    void setBright(quint8 bright);

signals:
    void onBrightChanged(int bright);

private:
    Ui::ModuleEfLumi *ui;
    bool m_bOutSet = false;
};

#endif // MODULEEFLUMI_H
