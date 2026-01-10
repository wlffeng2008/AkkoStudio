#include "modulemacromanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QDebug>

ModuleMacroManager::ModuleMacroManager(QObject *parent)
    : QObject{parent}
{
    m_strPath = QApplication::applicationDirPath() + "\\macro\\" ;
    QDir A(m_strPath);
    if(!A.exists())
        A.mkdir(m_strPath);

    saveLoadHeader(false);
}

MacroProject *ModuleMacroManager::addMacroProject()
{
    MacroProject *item = new MacroProject();
    item->mode = 0;
    item->repeat=1;

    for(int i=0; i<50; i++)
    {
        if(!getMarcoProject(i))
        {
            item->id=i;
            item->name = QString(tr("新建宏")+QString("%1").arg(i));
            break;
        }
    }
    m_macros.push_back(item);
    return item;
}

MacroProject *ModuleMacroManager::getMarcoProject(quint8 macroId)
{
    for(int i=0; i<m_macros.size(); i++)
    {
        if(m_macros[i]->id == macroId)
            return m_macros[i];
    }
    return nullptr;
}

void ModuleMacroManager::delMacroProject(MacroProject *item)
{
    m_macros.remove(m_macros.indexOf(item));
    saveLoadHeader();
}

void ModuleMacroManager::saveLoadHeader(bool save)
{
    QFile jF(m_strPath+"macroheader.txt");
    if(save)
    {
        QJsonObject Head;
        Head["version"]="20260109";
        Head["count"]=m_macros.size();
        QJsonArray Macro;
        for(int i=0; i<m_macros.size(); i++)
        {
            MacroProject *item = m_macros[i];
            QJsonObject It;
            It["id"]=item->id;
            It["mode"]=item->mode;
            It["name"]=item->name;
            It["repeat"]=item->repeat;
            Macro.push_back(It);
        }
        Head["macro"]=Macro;

        QJsonDocument jDoc(Head);
        QByteArray data = jDoc.toJson();
        if (jF.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            jF.write(data);
        }
    }
    else
    {
        if (jF.open(QIODevice::ReadOnly))
        {
            QByteArray data = jF.readAll();
            jF.close();
            QJsonDocument jDoc = QJsonDocument::fromJson(data);
            QJsonObject Head = jDoc.object() ;
            QJsonArray Macro = Head["macro"].toArray();
            int count = Macro.count();
            for(int i=0; i<count; i++)
            {
                QJsonObject It = Macro[i].toObject();
                MacroProject *item = new MacroProject();
                item->id     = It["id"].toInt();
                item->mode   = It["mode"].toInt();
                item->name   = It["name"].toString();
                item->repeat = It["repeat"].toInt();
                m_macros.push_back(item);
            }
            jF.close();
        }

        if(m_macros.size()==0)
        {
            for(int i=0; i<8; i++)
            {
                addMacroProject();
            }
            saveLoadHeader(true);
        }
    }
}

MacroEvent *ModuleMacroManager::addMacroEvent(MacroProject *item,quint8 type,quint16 value,bool down,const QString&text)
{
    MacroEvent *event = new MacroEvent();
    event->type=type;
    event->value=value;
    event->down=down;
    event->text=text;
    item->events.push_back(event);
    return nullptr;
}

void ModuleMacroManager::delMacroEvent(MacroProject *item,MacroEvent *event)
{
    int index = item->events.indexOf(event);
    if(index != -1)
        item->events.remove(index,2);
    saveLoadEvent(item);
}

void ModuleMacroManager::saveLoadEvent(MacroProject *item,bool save)
{
    int id = item->id;
    QFile jF(m_strPath+QString("macroevent%1.txt").arg(id));
    if(save)
    {
        QJsonArray jData;
        int count = item->events.count();
        for(int i=0; i<count; i++)
        {
            MacroEvent *event = item->events[i];
            QJsonObject jE;
            jE["type"]=event->type;
            jE["value"]=event->value;
            jE["down"]=event->down;
            jE["text"]=event->text;
            jData.push_back(jE);
        }
        qDebug() << "saveLoadEvent" <<  jData.size();
        QJsonObject jS ;
        jS["data"]=jData;
        QJsonDocument jDoc(jS);
        QByteArray data = jDoc.toJson();
        if (jF.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            jF.write(data);
        }
    }
    else
    {
        item->events.clear();
        if (jF.open(QIODevice::ReadOnly))
        {
            QByteArray data = jF.readAll();
            jF.close();

            QJsonDocument jDoc = QJsonDocument::fromJson(data);
            QJsonObject jData = jDoc.object() ;
            QJsonArray jArr = jData["data"].toArray();
            int count = jArr.count();

            for(int i=0; i<count; i++)
            {
                QJsonObject jE = jArr[i].toObject();
                MacroEvent *event = new MacroEvent();

                event->type  = jE["type"].toInt();
                event->value = jE["value"].toInt();
                event->down  = jE["down"].toBool();
                event->text  = jE["text"].toString();
                item->events.push_back(event);
            }
        }
    }
}
