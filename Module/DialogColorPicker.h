#ifndef DIALOGCOLORPICKER_H
#define DIALOGCOLORPICKER_H

#include <QDialog>

namespace Ui {
class DialogColorPicker;
}

class DialogColorPicker : public QDialog
{
    Q_OBJECT

public:
    explicit DialogColorPicker(QWidget *parent = nullptr);
    ~DialogColorPicker();

    QColor m_color;
    QString m_HexColor;

    void showPostion(int x=-1, int y=-1);
    void showPoint(const QPoint &point);

signals:
    void onPickupColor(const QColor&color);

protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched,QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::DialogColorPicker *ui;
    bool m_bShowing=true;
    QDialog *m_picker=nullptr;
    QImage m_image;
};

#endif // DIALOGCOLORPICKER_H
