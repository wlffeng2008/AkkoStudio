#ifndef MODULEGENKEYMAPPING_H
#define MODULEGENKEYMAPPING_H

#include <QObject>
#include <QWidget>

typedef struct
{
    quint8 b0;
    quint8 b1;
    quint8 b2;
    quint8 b3;

}keyData;

struct keyItem
{
    keyItem(const keyData &data,const QString&text){ this->data=data; this->text=text; }
    keyData data;
    QString text="";
};


quint8 getIndex(quint8 hid);
quint8 getHid(quint8 index);

keyData* getKeyData(quint8 index);
keyData* getFnData(quint8 index);

QString getKeyString(keyData *data);

QString getKeyValue(quint16 nCode);
quint16 getKeyHid(quint16 nCode);
quint16 getKeyNCode(quint16 hid);

class ModuleGenKeymapping:public QWidget
{
    Q_OBJECT

public:
    explicit ModuleGenKeymapping(QWidget *parent=nullptr);
    ~ModuleGenKeymapping();

    QList<keyItem *>m_fnList ;
    QList<keyItem *>m_muList ;
};


#endif // MODULEGENKEYMAPPING_H
