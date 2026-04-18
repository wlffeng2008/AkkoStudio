#ifndef DIALOGFNPICKER_H
#define DIALOGFNPICKER_H

#include <QDialog>
#include "ModuleGenKeymapping.h"

namespace Ui {
class DialogFNPicker;
}

class DialogFNPicker : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFNPicker(QWidget *parent = nullptr);
    ~DialogFNPicker();

    keyData m_kd = {0};
    bool m_macro = false;

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched,QEvent *event) override;

private:
    Ui::DialogFNPicker *ui;
};

#endif // DIALOGFNPICKER_H
