#include "AkkoDeviceBase.h"
#include <QFile>
#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

AkkoDeviceBase::AkkoDeviceBase(QWidget *parent)
    : QWidget(parent)
{

}


void AkkoDeviceBase::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}


static quint32 getIntVal(const QString&val)
{
    quint32 value = 0;
    QString tmp(val.trimmed().toUpper());
    if(val.startsWith("0X"))
    {
        tmp.replace("0X","");
        value=tmp.toUInt(nullptr,16);
    }
    else
    {
        value=tmp.toUInt();
    }
    return value;
}


static QList<AkkoDeviceInfo> s_AkkoDeviceTable = {

    {     4, 0x38EE, 0x000B, 0, 1, 0, "Pulse01 Pro"},
    {     4, 0x38EE, 0x000C, 0, 1, 1, "Pulse01 Pro"},
    {     5, 0x320F, 0x5151, 0, 0, 0, "3108RF"},  // 2.4G
    {     5, 0x320F, 0x5152, 0, 0, 1, "3108RF"},  // 有线
    {     6, 0x38EE, 0x000B, 1, 1, 0, "AG ONE"},
    {     6, 0x38EE, 0x000C, 1, 1, 1, "AG ONE"},
    {     7, 0x38EE, 0x0010, 1, 1, 0, "灵动V9 Ultra"},
    {     7, 0x38EE, 0x0011, 1, 1, 1, "灵动V9 Ultra"},
    {     8, 0x38EE, 0x0010, 1, 1, 0, "泰坦N9 Ultra"},
    {     8, 0x38EE, 0x0011, 1, 1, 1, "泰坦N9 Ultra"},
    {     9, 0x38EE, 0x000D, 1, 1, 0, "灵动V9 Ultra"},
    {     9, 0x38EE, 0x000F, 1, 1, 1, "灵动V9 Ultra"},
    {    10, 0x38EE, 0x000D, 1, 1, 0, "泰坦N9 Ultra"},
    {    10, 0x38EE, 0x000F, 1, 1, 1, "泰坦N9 Ultra"},
    {    11, 0x38EE, 0x0007, 1, 1, 0, "3087RF"},
    {    11, 0x38EE, 0x0008, 1, 1, 1, "3087RF"},
    {    12, 0x38EE, 0x000B, 1, 1, 0, "泰坦N9 Pro"},
    {    12, 0x38EE, 0x000C, 1, 1, 1, "泰坦N9 Pro"},
    {    13, 0x38EE, 0x0023, 1, 1, 0, "灵动V9 Ultra"},
    {    13, 0x38EE, 0x0024, 1, 1, 1, "灵动V9 Ultra"},
    {    15, 0x38EE, 0x000B, 5, 1, 0, "灵动V9 Master"},
    {    15, 0x38EE, 0x000C, 5, 1, 1, "灵动V9 Master"},
    {    16, 0x38EE, 0x000D, 6, 1, 0, "泰坦N9 Master"},
    {    16, 0x38EE, 0x000F, 6, 1, 1, "泰坦N9 Master"},
    {    17, 0x38EE, 0x0010, 1, 1, 0, "虫巢"},
    {    17, 0x38EE, 0x0011, 1, 1, 1, "虫巢"},
    {    18, 0x38EE, 0x000B, 3, 1, 0, "灵动V9 Pro"},
    {    18, 0x38EE, 0x000C, 3, 1, 1, "灵动V9 Pro"},
    {    19, 0x38EE, 0x000D, 7, 1, 0, "空影TAN8 Master"},
    {    19, 0x38EE, 0x000F, 7, 1, 1, "空影TAN8 Master"},
    {    20, 0x38EE, 0x000D, 8, 1, 0, "疾风TAN7 Master"},
    {    20, 0x38EE, 0x000F, 8, 1, 1, "疾风TAN7 Master"},
    {    21, 0x38EE, 0x000D, 9, 1, 0, "疾风TAN7 Ultra"},
    {    21, 0x38EE, 0x000F, 9, 1, 1, "疾风TAN7 Ultra"},
    {    22, 0x38EE, 0x0023, 1, 1, 0, "疾风TAN7 Ultra"},
    {    22, 0x38EE, 0x0024, 1, 1, 1, "疾风TAN7 Ultra"},
    {    23, 0x38EE, 0x000B, 4, 1, 0, "泰坦N9 Pro"},
    {    23, 0x38EE, 0x000C, 4, 1, 1, "泰坦N9 Pro"},
    {    24, 0x38EE, 0x000B, 5, 1, 0, "灵动V9 Pro"},
    {    24, 0x38EE, 0x000C, 5, 1, 1, "灵动V9 Pro"},
    {    25, 0x38EE, 0x0012, 0, 1, 0, "MX01"},
    {    25, 0x38EE, 0x0013, 0, 1, 1, "MX01"},
    {    26, 0x38EE, 0x000B, 6, 1, 0, "疾风TAN7 Pro"},
    {    26, 0x38EE, 0x000C, 6, 1, 1, "疾风TAN7 Pro"},
    {    27, 0x38EE, 0x0023, 2, 1, 0, "空影TAN8 Ultra"},
    {    27, 0x38EE, 0x0024, 2, 1, 1, "空影TAN8 Ultra"},
    {0xFF52, 0x38EE, 0x0016, 0, 0, 0, "FUN60"},
    {0xFF59, 0x38EE, 0x0016, 0, 0, 0, "TAC75 HE"},
    {0xFB29, 0x3554, 0xFB29, 1, 1, 0, "巢Nest"},
    {0xFFFF, 0x38EE, 0x0000, 1, 1, 0, "Unkown"}

};

AkkoDeviceInfo *getDevice(quint32 driverId)
{
    int nCount = s_AkkoDeviceTable.count();
    for (int i = 0; i < nCount; i++)
    {
        if (s_AkkoDeviceTable[i].driverId == driverId)
            return &s_AkkoDeviceTable[i];
    }
    return nullptr;
}

QString getDisplayName(quint32 driverId, quint16 & deviceType, int brand)
{
    if(brand == 0xFF)
    {
        AkkoDeviceInfo *pDI = getDevice(driverId);
        if(pDI)
        {
            deviceType = pDI->type;
            return pDI->name;
        }
        deviceType = 0xFF;
        return QString();
    }

    QString strFile = QApplication::applicationDirPath() + (brand == 0 ? "/AkkoHubDevices.json" : "/MGKHubDevices.json");
    QFile JF(strFile);
    if(JF.open(QIODevice::ReadOnly))
    {
        QString strJson = JF.readAll();
        QJsonDocument jDoc = QJsonDocument::fromJson(strJson.toUtf8());
        if(jDoc.isArray())
        {
            QJsonArray jArray = jDoc.array();
            for(int i=0; i<jArray.count(); i++)
            {
                QJsonObject jOb = jArray[i].toObject();
                if(jOb.value("id").toInt() == driverId)
                {
                    return jOb.value("displayName").toString().trimmed();
                }
            }
        }
        else
        {
            QJsonArray jArray = jDoc.object().value("keyboard").toArray();
            for(int i=0; i<jArray.count(); i++)
            {
                QJsonObject jOb = jArray[i].toObject();
                if(jOb.value("id").toInt() == driverId)
                {
                    deviceType=0;
                    return jOb.value("displayName").toString().trimmed();
                }
            }

            jArray = jDoc.object().value("mouse").toArray();
            for(int i=0; i<jArray.count(); i++)
            {
                QJsonObject jOb = jArray[i].toObject();
                if(jOb.value("id").toInt() == driverId)
                {
                    deviceType=1;
                    return jOb.value("displayName").toString().trimmed();
                }
            }

            jArray = jDoc.object().value("hitBoxKeyboard").toArray();
            for(int i=0; i<jArray.count(); i++)
            {
                QJsonObject jOb = jArray[i].toObject();
                if(jOb.value("id").toInt() == driverId)
                {
                    deviceType=3;
                    return jOb.value("displayName").toString().trimmed();
                }
            }

            jArray = jDoc.object().value("hitBoxScreen").toArray();
            for(int i=0; i<jArray.count(); i++)
            {
                QJsonObject jOb = jArray[i].toObject();
                if(jOb.value("id").toInt() == driverId)
                {
                    deviceType=3;
                    return jOb.value("displayName").toString().trimmed();
                }
            }
        }
    }
    deviceType = 0xFF;
    return QString();
}