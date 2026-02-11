#ifndef FRAMEDEVICESHOW_H
#define FRAMEDEVICESHOW_H

#include <QFrame>
#include <QScrollArea>

namespace Ui {
class FrameDeviceShow;
}

class FrameDeviceShow : public QFrame
{
    Q_OBJECT

public:
    explicit FrameDeviceShow(QWidget *parent = nullptr);
    ~FrameDeviceShow();

    static FrameDeviceShow *getFrameShow(int index,QWidget *parent);
    void setImage(const QString & image, int type=0);
    void setName(const QString & name, int type=0);
    void setPath(const QString & path){ m_path = path; };
    void setCreator(int creator){ m_creator = creator;};
    void setSelect(bool select=true);

    QScrollArea *m_sa = nullptr;
    void *m_device = nullptr;
    bool m_bConacted = false;
    QString m_path;
    QString m_image;
    int m_creator;

signals:
    void onClicked(void *device,const QString&path,const QString&image,int creator);

protected:
    bool event(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::FrameDeviceShow *ui;
};

#endif // FRAMEDEVICESHOW_H
