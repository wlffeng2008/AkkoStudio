#ifndef MODULEADDMACROSQUARE_H
#define MODULEADDMACROSQUARE_H

#include <QFrame>

namespace Ui {
class ModuleAddMacroSquare;
}

class ModuleAddMacroSquare : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleAddMacroSquare(QWidget *parent = nullptr);
    ~ModuleAddMacroSquare();

    quint8 type();
    quint8 mKey();
    quint8 bKey();
    quint8 xPos();
    quint8 yPos();

private:
    Ui::ModuleAddMacroSquare *ui;
};

#endif // MODULEADDMACROSQUARE_H
