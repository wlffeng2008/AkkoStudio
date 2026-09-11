#ifndef DIALOGMOUSE_H
#define DIALOGMOUSE_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class DialogMouse;
}

class DialogMouse : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMouse(QWidget *parent = nullptr);
    ~DialogMouse();

    void updateName(const QString&name);

protected:
    bool eventFilter(QObject *watched, QEvent *event) final;
    void changeEvent(QEvent *pEvt) final;
    void mousePressEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;


private:
    Ui::DialogMouse *ui;
    QPointF m_dragPosition;
    bool m_dragging = false;

    int m_nowAngle=0;

    QStandardItemModel *m_pModel = nullptr;
};

#endif // DIALOGMOUSE_H
