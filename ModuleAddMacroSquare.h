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
    explicit ModuleAddMacroSquare(bool enablePostion,QWidget *parent = nullptr);
    ~ModuleAddMacroSquare();

    quint8 type();
    quint8 mKey();
    quint8 bKey();
    quint16 xPos();
    quint16 yPos();
    quint16 kNativeVK();
    quint16 kRawKey();

signals:
    void insert();

protected:
    void keyPressEvent(QKeyEvent *event) override;
private:
    Ui::ModuleAddMacroSquare *ui;
    quint8 m_hid = 0;
    quint16 m_nativeVK=0;
    quint16 m_rawKey=0;
};

#endif // MODULEADDMACROSQUARE_H
