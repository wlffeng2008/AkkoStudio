#ifndef MODULELANGMENU_H
#define MODULELANGMENU_H

#include <QDialog>

#include <QButtonGroup>

namespace Ui {
class ModuleLangMenu;
}

class ModuleLangMenu : public QDialog
{
    Q_OBJECT

public:
    explicit ModuleLangMenu(QWidget *parent = nullptr);
    ~ModuleLangMenu();

    void setLanguage(int langId) ;

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

signals:
    void onLangChanged(int langId,const QString&lang) ;

private:
    Ui::ModuleLangMenu *ui;
    QButtonGroup *m_pBtnGrp = nullptr ;
};

#endif // MODULELANGMENU_H
