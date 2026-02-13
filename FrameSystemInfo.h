#ifndef FRAMESYSTEMINFO_H
#define FRAMESYSTEMINFO_H

#include <QFrame>

namespace Ui {
class FrameSystemInfo;
}

class FrameSystemInfo : public QFrame
{
    Q_OBJECT

public:
    explicit FrameSystemInfo(QWidget *parent = nullptr);
    ~FrameSystemInfo();

private:
    Ui::FrameSystemInfo *ui;
};

#endif // FRAMESYSTEMINFO_H
