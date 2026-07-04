#ifndef FRAMESYSTEMINFO_H
#define FRAMESYSTEMINFO_H

#include <QFrame>

namespace Ui {
class FrameSystemInfo;
}

QString currentVersion();

class FrameSystemInfo : public QFrame
{
    Q_OBJECT

public:
    explicit FrameSystemInfo(QWidget *parent = nullptr);
    ~FrameSystemInfo();

private slots:
    void on_pushButtonWeb_clicked();

private:
    void changeEvent(QEvent *pEvt) final;

private:
    Ui::FrameSystemInfo *ui;
};

#endif // FRAMESYSTEMINFO_H
