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
protected:
    void keyPressEvent(QKeyEvent *event) override;
private:
    Ui::ModuleAddMacroSquare *ui;
    quint8 m_hid = 0;
};

#endif // MODULEADDMACROSQUARE_H
