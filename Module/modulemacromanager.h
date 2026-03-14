#ifndef MODULEMACROMANAGER_H
#define MODULEMACROMANAGER_H

#include <QObject>

typedef struct
{
    quint8 type;
    bool down;
    quint16 value;
    QString text;
}MacroEvent;

typedef struct
{
    quint8 id;
    quint8 repeat;
    quint8 mode;
    QString name;
    QList<MacroEvent *>events;
}MacroProject;

class ModuleMacroManager : public QObject
{
    Q_OBJECT
public:
    explicit ModuleMacroManager(QObject *parent = nullptr);

    MacroProject *addMacroProject();
    MacroProject *getMarcoProject(quint8 macroId);
    QString getMarcoName(quint8 macroId);
    void delMacroProject(MacroProject *prj);
    void saveLoadHeader(bool save=true);

    MacroEvent *addMacroEvent(MacroProject *prj,quint8 type,quint16 value,bool down,const QString&text);
    void delMacroEvent(MacroProject *prj,MacroEvent *event);
    void saveLoadEvent(MacroProject *prj,bool save=true);

    static QByteArray packMacroPack(MacroProject *prj);
    static void unackMacroPack(MacroProject *prj,QByteArray&data);
    static ModuleMacroManager* instance();

signals:
    void nothing();
private:
    QList<MacroProject *>m_macros;
    QString m_strPath ;
};

#endif // MODULEMACROMANAGER_H
