#include "FrameDeviceShow.h"
#include "ui_FrameDeviceShow.h"

#include "hidapi.h"

#include <QScrollBar>
#include <QWheelEvent>
#include <QTimer>
#include <QThread>
#include <QApplication>
#include <QMessageBox>

static FrameDeviceShow *s_active = nullptr;

FrameDeviceShow *FrameDeviceShow::getFrameShow(int index, QWidget *parent)
{
    static FrameDeviceShow *pGroup[256] = {0};
    FrameDeviceShow *pFrame = pGroup[index];
    if (!pFrame)
    {
        pFrame = new FrameDeviceShow(parent);
        pGroup[index] = pFrame;
    }

    return pFrame;
}

FrameDeviceShow::FrameDeviceShow(QWidget *parent)
    : QFrame(parent), ui(new Ui::FrameDeviceShow)
{
    ui->setupUi(this);

    if (!s_active) s_active = this;

    setSelect(false);

    QTimer *pTMBatt = new QTimer(this);
    connect(pTMBatt,&QTimer::timeout,this,[=]{
        updateBattery();
    });

    pTMBatt->start(10000);
}

FrameDeviceShow::~FrameDeviceShow()
{
    delete ui;
}

void FrameDeviceShow::updateBattery()
{
    quint32 batt = 100;
    if(m_cnnType != 0)
    {
        int nlen = 0;
        if(m_creator == 0)
        {
            char buf[1024] = {0};

            hid_device *pDev = hid_open_path(m_path2.toStdString().c_str());
            if(!pDev) return;
            hid_set_nonblocking(pDev,1);

            QByteArray tmp(120, 0);

            int ntry = 0;
            while(ntry++ < 5)
            {
                tmp[1] = 0xf7;
                tmp[2] = 0x00;
                tmp[8] = 0xFF - tmp[1] - tmp[2];
                hid_send_feature_report(pDev, (quint8 *)tmp.data(), 65);
                QThread::msleep(15);
                nlen = hid_get_feature_report(pDev, (quint8 *)buf, 65);
                if(buf[6] == 1)
                    break;
            }

            // tmp[1] = 0x82;
            // tmp[8] = 0xFF - tmp[1];
            // hid_send_feature_report(pDev, (quint8 *)tmp.data(), 65);
            // QThread::msleep(20);
            // nlen = hid_get_feature_report(pDev, (quint8 *)buf, 65);

            if(nlen > 0) batt = buf[2];
            hid_close(pDev);
        }
        else
        {
            hid_device *pDev = hid_open_path(m_path2.toStdString().c_str());            
            if(!pDev) return;

            QString strCmd("04 00 00 1A 06 00 00 00");
            QByteArray cmd = QByteArray::fromHex(strCmd.toLatin1());
            hid_write(pDev,(quint8*)cmd.data(),cmd.size());
            QThread::msleep(5);
            quint8 buf[128] = {0};
            nlen = hid_read_timeout(pDev,buf,16,500);
            if(nlen > 0) batt = buf[8];
            hid_close(pDev);
        }
    }

    static QStringList imgPowers = {"batt-low.png","batt-25.png","batt-50.png","batt-75.png","batt-full.png"};
    quint8 level = 0;
    if(batt>20) level=1;
    if(batt>40) level=2;
    if(batt>60) level=3;
    if(batt>80) level=4;

    QString strBatt = QString(":/images/dev/") + imgPowers[level];
    QString strTip  = QString(tr("剩余电量")) +  QString(": %1%").arg(batt%101);

    ui->labelPower->setPixmap(QPixmap(strBatt));
    ui->labelPower->setToolTip(strTip);

    QString qss = R"(
        QToolTip {
            background-color: #F0F8FF;
            color: #333333;
            font-size: 12px;
            border-radius: 4px;
            border: 1px solid #CCCCCC;
            padding: 4px 4px;
        }
    )";
    if(m_cnnType == 0) strBatt.clear();
    ui->labelPower->setStyleSheet(qss);

    emit onReport(m_devInfo,strBatt,m_typeImage,strTip,qss);
}

void FrameDeviceShow::setImage(const QString &image,int type)
{
    m_image = image;
    QPixmap Img(image);
    int nSetW = 0;
    if (Img.isNull())
    {
        QString strDef = QApplication::applicationDirPath() + QString("/images/default%1.png").arg(type);
        Img = QPixmap(strDef);
        m_image = strDef;
    }

    ui->labelPower->setHidden(m_cnnType == 0);
    QTimer::singleShot(500,this,[=]{ updateBattery(); });

    static QStringList imgTypes = {"usb.png", "2.4g.png", "ble.png"};
    m_typeImage = QString(":/images/dev/") + imgTypes[m_cnnType];
    ui->labelType->setPixmap(QPixmap(m_typeImage));

    if (!Img.isNull())
    {
        int imgW = Img.width();
        int imgH = Img.height();

        int nW = this->width() - 40;
        int nH = this->height() - 160;

        nH = 210;

        qreal ir = imgW * 1.0 / imgH;
        if (ir > 1)
        {
            int nSH = nW / ir;
            if(nSH > nH)
            {
                nSH = nH;
                nW  = nH * ir;
            }
            ui->labelImage->setFixedSize(nW, nSH);
            nSetW = nW+80;
        }
        else
        {
            int nSW = nH * ir;
            if(nSW > nW)
            {
                nSW = nW;
                nH = nW / ir;
            }
            ui->labelImage->setFixedSize(nSW, nH);
            nSetW = nSW+80;
        }
    }

    if(nSetW > 0)
    {
        if(nSetW < 260) nSetW = 260;
        setFixedWidth(nSetW);
    }

    ui->labelImage->setScaledContents(true);
    ui->labelImage->setPixmap(Img);
}

void FrameDeviceShow::setName(const QString &name,int type)
{
    ui->labelDeviceName->setText(name);
    QString strImg =  QString("/images/%1.png").arg(name);
    strImg.replace(' ','-');
    setImage(QApplication::applicationDirPath() + strImg,type);
}

void FrameDeviceShow::setSelect(bool select)
{
    if (select)
        setStyleSheet("#FrameDeviceShow{background-color:#E0E0E0; border-radius:32px; border: 1px solid #B0B0B0;}");
    else
        setStyleSheet("#FrameDeviceShow{background-color:#F4F4F4; border-radius:32px; border: 1px solid transparent;}");

    update();
}

void simulateMouseClick(QWidget *targetWgt, QPoint pos, Qt::MouseButton button = Qt::LeftButton)
{
    QMouseEvent pressEvt(QEvent::MouseButtonPress, pos, button, button, Qt::NoModifier);
    QApplication::sendEvent(targetWgt, &pressEvt);

    QMouseEvent releaseEvt(QEvent::MouseButtonRelease, pos, button, button, Qt::NoModifier);
    QApplication::sendEvent(targetWgt, &releaseEvt);
}

bool FrameDeviceShow::event(QEvent *event)
{
    if (event->type() == QEvent::Wheel)
    {
        QWheelEvent *we = static_cast<QWheelEvent *>(event);
        QScrollBar *pSB = m_sa->horizontalScrollBar();
        pSB->setValue(pSB->value() + we->angleDelta().y());
        return true;
    }

    if (event->type() == QEvent::Enter)
    {
        setSelect(true);
    }

    if (event->type() == QEvent::Leave)
    {
        setSelect(false);
    }

    if (event->type() == QEvent::MouseButtonPress)
    {
        QTimer::singleShot(50,this,[=]{
            emit onClicked(m_devInfo,m_path1,m_path2,m_image,m_creator,m_cnnType);
            updateBattery();
        });
    }

    return QFrame::event(event);
}

void FrameDeviceShow::mousePressEvent(QMouseEvent *event)
{
    if (s_active)
        s_active->setSelect(false);

    s_active = this;
    setSelect();

    QFrame::mousePressEvent(event);
}
