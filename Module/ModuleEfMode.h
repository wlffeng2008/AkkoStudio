#ifndef MODULEEFMODE_H
#define MODULEEFMODE_H

#include <QFrame>
#include <QButtonGroup>

namespace Ui {
class ModuleEfMode;
}


typedef struct{
    QString name ;
    quint8 mode ;
}EfMode;


class ModuleEfMode: public QFrame
{
    Q_OBJECT

public:
    explicit ModuleEfMode(QWidget *parent = nullptr);
    ~ModuleEfMode();
    void setEfMode(int mode);

protected:
    void changeEvent(QEvent *pEvt) override;
    void updateButtonText();
signals:
    void onModeChanged(int mode,quint8 opt=true);
private:
    Ui::ModuleEfMode *ui;
    QButtonGroup *pBtnGrp = nullptr;
    bool m_bOutSet=false;
    quint8 m_mode=0;

    QList<EfMode> EfModeList;
};

#endif // MODULEEFMODE_H
