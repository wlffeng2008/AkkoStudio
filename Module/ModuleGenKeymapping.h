#ifndef MODULEGENKEYMAPPING_H
#define MODULEGENKEYMAPPING_H

#include <QObject>
#include <QWidget>

struct keyData
{
    quint8 b0;
    quint8 b1;
    quint8 b2;
    quint8 b3;

};

struct keyItem
{
    keyItem(const keyData &data,const QString&text){ this->data=data; this->text=text; }
    keyData data;
    QString text="";
};

quint8 getIndex(quint8 hid);
quint8 getHid(quint8 index);

keyData* getMuData(quint8 index);
keyData* getFnData(quint8 index);
keyData* getMatData(quint8 index);

bool isKeyChanged(quint8 index,const keyData*kd);
bool isKeyDisabled(quint8 index);
bool isKeyEqual(const keyData*kdA, const keyData*kdB);

QString getKeyString(keyData *data,bool addPreffix=true);

QString getKeyValue(quint16 nCode);
quint16 getKeyHid(quint16 nCode);
quint16 getKeyNCode(quint16 hid);

QByteArray getDefaultMatrix();
void setDefaultMatrix(const QByteArray&data);

class ModuleGenKeymapping:public QWidget
{
    Q_OBJECT

public:
    explicit ModuleGenKeymapping(QWidget *parent=nullptr);
    ~ModuleGenKeymapping();

    QString getKeyString(const keyData *data,bool addPreffix=true);

    keyData* getMuData(quint8 index);
    keyData* getFnData(quint8 index);

private:
    void initList() ;
    QList<keyItem *>m_fnList;
    QList<keyItem *>m_muList;
};


#endif // MODULEGENKEYMAPPING_H
