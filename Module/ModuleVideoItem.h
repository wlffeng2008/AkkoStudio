#ifndef MODULEVIDEOITEM_H
#define MODULEVIDEOITEM_H

#include <QFrame>

namespace Ui {
class ModuleVideoItem;
}

class ModuleVideoItem : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleVideoItem(QWidget *parent = nullptr);
    ~ModuleVideoItem();

protected:
    void changeEvent(QEvent *pEvt) override;

private:
    Ui::ModuleVideoItem *ui;
};

#endif // MODULEVIDEOITEM_H
