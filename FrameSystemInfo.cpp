#include "FrameSystemInfo.h"
#include "ui_FrameSystemInfo.h"
#include "ModuleGeneralMasker.h"
#include <QSettings>
#include <QApplication>
#include <QDateTime>
#include <Windows.h>

QString g_strVer("1.6.7") ;

inline QDateTime getCompileTime()
{
    QLocale enUs(QLocale::English, QLocale::UnitedStates);

    QDateTime dt = enUs.toDateTime(__TIMESTAMP__, "ddd MMM dd HH:mm:ss yyyy");
    qDebug() << dt;
    return dt;
}

inline QString getCompileTimeText(const QString &fmt = "yyyy-MM-dd HH:mm:ss")
{
    return getCompileTime().toString(fmt);
}


FrameSystemInfo::FrameSystemInfo(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameSystemInfo)
{
    ui->setupUi(this);

    static QSettings regSet0("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    regSet0.remove("AkkoHub");

    static QSettings regSet("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    ui->checkBox->setChecked(!regSet.value("AkkoHub").toString().isEmpty());

    connect(ui->pushButtonOK,&QPushButton::clicked,this,[=]{
        ModuleGeneralMasker *pTop = static_cast<ModuleGeneralMasker *>(this->parent());
        pTop->setFlag(QDialog::Accepted);
        regSet.remove("AkkoHub");
        if(ui->checkBox->isChecked())
        {
            QString strFile = QString("\"%1\"").arg(QApplication::applicationFilePath().replace("/","\\"));
            regSet.setValue("AkkoHub",strFile);
        }

        hide();
    });

    connect(ui->pushButtonClose,&QPushButton::clicked,this,[=]{
        ModuleGeneralMasker *pTop = static_cast<ModuleGeneralMasker *>(this->parent());
        pTop->setFlag(QDialog::Rejected);
        hide();
    });

    ui->labelVersion->setText(ui->labelVersion->text().trimmed() + QString(":  ") + g_strVer);
    ui->labelBuild->setText(ui->labelBuild->text().trimmed()  +QString(":  ") + getCompileTimeText());

    //raise();
}

FrameSystemInfo::~FrameSystemInfo()
{
    delete ui;
}

void FrameSystemInfo::on_pushButtonWeb_clicked()
{
    QString strUrl("https://www.akkogear.com/download/");
    ::ShellExecute(NULL, L"open", (LPCWSTR)strUrl.toStdU16String().c_str(), NULL, NULL, SW_SHOWNORMAL);
}

