#ifndef MODULERTSET_H
#define MODULERTSET_H

#include <QFrame>
#include <QTimer>

namespace Ui {
class ModuleRtSet;
}

class ModuleRtSet : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleRtSet(QWidget *parent = nullptr);
    ~ModuleRtSet();

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject*,QEvent*) override;

signals:
    void setGlobalRtValue(float value,int type=0);

private:
    Ui::ModuleRtSet *ui;
    void setValue(float value);
    float getValue();
    bool m_dragging = false;

    QTimer *m_pSetTM = nullptr;
};

#endif // MODULERTSET_H
