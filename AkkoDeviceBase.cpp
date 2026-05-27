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


static QList<AkkoDeviceInfo> s_AkkoDeviceTable = {

    {  11, 0x38EE, 0x0007, 1, "3087RF"},
    {  11, 0x38EE, 0x0008, 1, "3087RF"},
    {   4, 0x38EE, 0x000B, 1, "Pulse01 Pro"},
    {   4, 0x38EE, 0x000C, 1, "Pulse01 Pro"},
    {  12, 0x38EE, 0x000B, 1, "泰坦 Pro"},
    {  12, 0x38EE, 0x000C, 1, "泰坦 Pro"},
    {   5, 0x320F, 0x5151, 0, "3108RF"},  // 2.4G
    {   5, 0x320F, 0x5152, 0, "3108RF"},  // 有线
    {   6, 0x38EE, 0x000B, 1, "AG ONE"},
    {   6, 0x38EE, 0x000C, 1, "AG ONE"},
    {  15, 0x38EE, 0x000B, 1, "灵动V9 Master"},
    {  15, 0x38EE, 0x000C, 1, "灵动V9 Master"},
    {  16, 0x38EE, 0x000D, 1, "泰坦N9 Master"},
    {  16, 0x38EE, 0x000F, 1, "泰坦N9 Master"},
    {  17, 0x38EE, 0x0010, 1, "虫巢"},
    {  17, 0x38EE, 0x0011, 1, "虫巢"},
    {  18, 0x38EE, 0x0010, 1, "灵动V9 Pro"},
    {  18, 0x38EE, 0x0011, 1, "灵动V9 Pro"},
    {  19, 0x38EE, 0x000D, 1, "空影TAN8 Master"},
    {  19, 0x38EE, 0x000F, 1, "空影TAN8 Master"},

    {  20, 0x38EE, 0x000D, 1, "疾风TAN7 Master"},
    {  20, 0x38EE, 0x000F, 1, "疾风TAN7 Master"},
    {  21, 0x38EE, 0x000D, 1, "疾风TAN7 Ultra"},
    {  21, 0x38EE, 0x000F, 1, "疾风TAN7 Ultra"},
    {  22, 0x38EE, 0x0023, 1, "疾风TAN7 Ultra"},
    {  22, 0x38EE, 0x0024, 1, "疾风TAN7 Ultra"},
    {  23, 0x38EE, 0x000B, 1, "泰坦N9 Pro"},
    {  23, 0x38EE, 0x000C, 1, "泰坦N9 Pro"},
    {  24, 0x38EE, 0x000B, 1, "灵动V9 Pro"},
    {  24, 0x38EE, 0x000C, 1, "灵动V9 Pro"},
    {  25, 0x38EE, 0x000B, 1, "MX01"},
    {  25, 0x38EE, 0x000C, 1, "MX01"},

    {   7, 0x38EE, 0x0010, 1, "灵动V9 Max"},
    {   7, 0x38EE, 0x0011, 1, "灵动V9 Max"},
    {   8, 0x38EE, 0x0010, 1, "泰坦N9 Max"},
    {   8, 0x38EE, 0x0011, 1, "泰坦N9 Max"},
    {   9, 0x38EE, 0x000D, 1, "灵动V9 Ultra"},
    {   9, 0x38EE, 0x000F, 1, "灵动V9 Ultra"},
    {  10, 0x38EE, 0x000D, 1, "泰坦N9 Ultra"},
    {  10, 0x38EE, 0x000F, 1, "泰坦N9 Ultra"},
    {  13, 0x38EE, 0x0023, 1, "灵动V9 Max"},
    {  13, 0x38EE, 0x0024, 1, "灵动V9 Max"},

    {  0xFF52, 0x38EE, 0x0016, 0, "FUN60"},
    {  0xFF59, 0x38EE, 0x0016, 0, "TAC75 HE"},
    {  0xFB29, 0x3554, 0xFB29, 1, "巢Nest"},
    {  0xFFFF, 0x38EE, 0x0000, 1, "Unkown"}

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