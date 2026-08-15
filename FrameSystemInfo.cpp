#include "FrameSystemInfo.h"
#include "ui_FrameSystemInfo.h"
#include "ModuleGeneralMasker.h"
#include <QSettings>
#include <QApplication>
#include <QDateTime>
#include <Windows.h>

QString g_strVer("1.7.5");

QString currentVersion()
{
    return g_strVer;
}

inline QDateTime getCompileTime()
{
    const QString tsStr = QString::fromLatin1(__TIMESTAMP__);
    QLocale enUs(QLocale::English, QLocale::UnitedStates);

    QDateTime         dt = enUs.toDateTime(tsStr, "ddd MMM dd HH:mm:ss yyyy");
    if(!dt.isValid()) dt = enUs.toDateTime(tsStr, "ddd MMM  d HH:mm:ss yyyy");

    return dt;
}

inline QString getCompileTimeText(const QString &fmt = "yyyy-MM-dd HH:mm:ss")
{
    QDateTime dt = getCompileTime();
    return dt.isValid() ? dt.toString(fmt) : __TIMESTAMP__;
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

}

void FrameSystemInfo::changeEvent(QEvent *pEvt)
{
    if(pEvt->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        ui->labelVersion->setText(tr("当前版本") + QString(":  ") + g_strVer);
        ui->labelBuild->setText(tr("编译时间")   + QString(":  ") + getCompileTimeText());
    }
    QFrame::changeEvent(pEvt);
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

