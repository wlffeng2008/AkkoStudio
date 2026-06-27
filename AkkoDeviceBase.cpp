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


quint64 getIntVal(const QString&val)
{
    quint64 value = 0;
    QString tmp(val.trimmed().toUpper());
    if(tmp.contains("0X"))
    {
        value=tmp.toLatin1().toULongLong(nullptr,16);
    }
    else
    {
        value = tmp.toULongLong();
    }
    return value;
}


quint32 getExtrasDevice(quint32 VID, quint32 PID, quint64 MOD,QString & deviceName, quint16 & deviceType, quint16&deviceCnnt)
{
    QString strFile = QApplication::applicationDirPath() + "/config/CustomDeviceList.json";

    QFile JF(strFile);
    if(JF.open(QIODevice::ReadOnly))
    {
        QTextStream in(&JF);
        while(!in.atEnd())
        {
            QString strLine = in.readLine().trimmed();
            if(strLine == "////CustomDeviceList////")
            {
                while(!in.atEnd())
                {
                    strLine = in.readLine();
                    strLine.replace("{","");
                    strLine.replace("}","");
                    strLine.replace("\"","");
                    if(strLine.isEmpty())
                        continue;

                    QStringList values = strLine.trimmed().split(',') ;
                    if(values.count()<6)
                        continue;

                    quint16 assignId  = getIntVal(values[0].trimmed());
                    quint16 deviceVID = getIntVal(values[1].trimmed());
                    quint16 devicePID = getIntVal(values[2].trimmed());
                    quint64 deviceMOD = getIntVal(values[3].trimmed());
                    quint16 deviceTYP = getIntVal(values[4].trimmed());
                    quint16 deviceUSB = getIntVal(values[5].trimmed());
                    QString strDevName= values[6].trimmed();

                    if(deviceVID == VID && devicePID == PID && deviceMOD == MOD)
                    {
                        //qDebug() << "Found:" << Qt::hex << VID << PID << MOD << assignId ;
                        deviceType = deviceTYP;
                        deviceCnnt = deviceUSB;
                        deviceName = strDevName;
                        return assignId;
                    }
                }
            }
        }
        JF.close();
    }
    return 0;
}


QString getDisplayName(quint32 driverId, quint16 & deviceType, int brand)
{
    if(brand == 0xFF)
    {
        QString strFile = QApplication::applicationDirPath() + "/config/CustomDeviceList.json";

        QFile JF(strFile);
        if(JF.open(QIODevice::ReadOnly))
        {
            QTextStream in(&JF);
            while(!in.atEnd())
            {
                QString strLine = in.readLine().trimmed();
                if(strLine == "////CustomDeviceList////")
                {
                    while(!in.atEnd())
                    {
                        strLine = in.readLine();
                        strLine.replace("{","");
                        strLine.replace("}","");
                        strLine.replace("\"","");
                        if(strLine.isEmpty())
                            continue;

                        QStringList values = strLine.trimmed().split(',') ;
                        if(values.count()<6)
                            continue;

                        quint16 assignId  = getIntVal(values[0].trimmed());
                        quint16 deviceVID = getIntVal(values[1].trimmed());
                        quint16 devicePID = getIntVal(values[2].trimmed());
                        quint16 deviceMOD = getIntVal(values[3].trimmed());
                        quint16 deviceTYP = getIntVal(values[4].trimmed());
                        quint16 deviceUSB = getIntVal(values[5].trimmed());
                        QString deviceName= values[6].trimmed();

                        if(assignId == driverId)
                        {
                            deviceType = deviceTYP;
                            return deviceName;
                        }
                    }
                }
            }
            JF.close();
        }

        deviceType = 0xFF;
        return QString();
    }

    QString strFile = QApplication::applicationDirPath() + (brand == 0 ? "/config/AkkoHubDevices.json" : "/config/MGKHubDevices.json");
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