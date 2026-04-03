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
    void setPath(const QString & path1, const QString & path2){ m_path1 = path1; m_path2 = path2;};
    void setCreator(int creator){ m_creator = creator;};
    void setSelect(bool select=true);

    void updateBattery();

    QScrollArea *m_sa = nullptr;
    void *m_device = nullptr;
    bool m_bConacted = false;
    quint8 m_cnnType =0;
    QString m_path1;
    QString m_path2;
    QString m_image;
    QString m_typeImage;
    int m_creator;

signals:
    void onClicked(void *device,const QString&path1,const QString&path2,const QString&image,int creator);
    void onReport(void *device,const QString&battImg,const QString&typeImg,const QString&tip,const QString&qss);

protected:
    bool event(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::FrameDeviceShow *ui;
};

#endif // FRAMEDEVICESHOW_H
