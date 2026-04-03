#ifndef MODULERTSET_H
#define MODULERTSET_H

#include <QFrame>

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
    void onSetValue(float value,int type=0);

private:
    Ui::ModuleRtSet *ui;
    void setValue(float value);
    float getValue();
    bool m_dragging = false;
};

#endif // MODULERTSET_H
