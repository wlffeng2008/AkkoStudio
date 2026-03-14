#include "modulemacromanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QDebug>

static ModuleMacroManager *s_instance = nullptr;
ModuleMacroManager::ModuleMacroManager(QObject *parent)
    : QObject{parent}
{
    if(!s_instance) s_instance = this;

    m_strPath = QApplication::applicationDirPath() + "\\macro\\" ;
    QDir A(m_strPath);
    if(!A.exists())
        A.mkdir(m_strPath);

    saveLoadHeader(false);
}

ModuleMacroManager *ModuleMacroManager::instance()
{
    return s_instance;
}

MacroProject *ModuleMacroManager::addMacroProject()
{
    if(m_macros.count() >= 50)
        return nullptr;
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
    for(MacroProject *prj: std::as_const(m_macros))
    {
        if(prj->id == macroId)
            return prj;
    }
    return nullptr;
}

QString ModuleMacroManager::getMarcoName(quint8 macroId)
{
    QString name;
    MacroProject *prj = getMarcoProject(macroId);
    if(prj) name = prj->name;

    return name;
}

void ModuleMacroManager::delMacroProject(MacroProject *prj)
{
    m_macros.remove(m_macros.indexOf(prj));
    delete prj;
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
            jF.close();
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

                saveLoadEvent(item,false);
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

MacroEvent *ModuleMacroManager::addMacroEvent(MacroProject *prj,quint8 type,quint16 value,bool down,const QString&text)
{
    MacroEvent *event = new MacroEvent();
    event->type=type;
    event->value=value;
    event->down=down;
    event->text=text;
    prj->events.push_back(event);
    return event;
}

void ModuleMacroManager::delMacroEvent(MacroProject *prj,MacroEvent *event)
{
    int index = prj->events.indexOf(event);
    if(index != -1)
        prj->events.remove(index,2);
    saveLoadEvent(prj);
}

void ModuleMacroManager::saveLoadEvent(MacroProject *prj,bool save)
{
    int id = prj->id;
    QFile jF(m_strPath+QString("macroevent%1.txt").arg(id));
    if(save)
    {
        QJsonArray jData;
        int count = prj->events.count();
        for(int i=0; i<count; i++)
        {
            MacroEvent *event = prj->events[i];
            QJsonObject jE;
            jE["type"]=event->type;
            jE["value"]=event->value;
            jE["down"]=event->down;
            jE["text"]=event->text;
            jData.push_back(jE);
        }

        QJsonObject jS;
        jS["data"]=jData;
        QJsonDocument jDoc(jS);
        QByteArray data = jDoc.toJson();
        if (jF.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            jF.write(data);
            jF.close();
        }
    }
    else
    {
        prj->events.clear();
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
                prj->events.push_back(event);
            }
        }
    }
}


QByteArray ModuleMacroManager::packMacroPack(MacroProject *prj)
{
    QByteArray data;
    if(prj)
    {
        int count = prj->events.count();
        for(int i=0; i<count; i+= 2)
        {
            MacroEvent *evt0 = prj->events[i+0];
            MacroEvent *evt1 = prj->events[i+1];
            int len = 2;
            quint8 unit[8] = {0};
            switch(evt0->type)
            {
            case 0:
            case 1:
                unit[0] = evt0->value & 0xFF;
                if(evt1->value <= 0x7F)
                {
                    unit[1] = evt1->value + (evt0->down ? 0x80 : 0x00);
                }
                else
                {
                    unit[1] = (evt0->down ? 0x80 : 0x00);
                    unit[2] = (evt1->value & 0x00FF);
                    unit[3] = (evt1->value & 0xFF00) >> 8;
                    len = 4;
                }
                break;

            case 2:
                break;

            case 3:
                unit[0] = 0xF9;
                unit[1] = 0x00;

                len = 4;
                if(evt1->value <= 0x7F)
                {
                    unit[1] = (evt1->value & 0xFF);
                }
                unit[2] = (evt0->value & 0x00FF);
                unit[3] = (evt0->value & 0xFF00) >> 8;

                if(evt1->value > 0x7F)
                {
                    unit[4] = (evt1->value & 0x00FF);
                    unit[5] = (evt1->value & 0xFF00) >> 8;
                    len = 6;
                }
                break;
            }

            data.append((char *)unit,len);
        }
    }

    return data;
}

void ModuleMacroManager::unackMacroPack(MacroProject *prj,QByteArray&data)
{

}
