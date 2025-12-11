#ifndef DIALOGVKPICKER_H
#define DIALOGVKPICKER_H

#include <QDialog>

namespace Ui {
class DialogVKPicker;
}

class DialogVKPicker : public QDialog
{
    Q_OBJECT

public:
    explicit DialogVKPicker(QWidget *parent = nullptr);
    ~DialogVKPicker();
    quint8 m_selIds[3]={0};

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched,QEvent *event) override;

private:
    Ui::DialogVKPicker *ui;

};

#endif // DIALOGVKPICKER_H
