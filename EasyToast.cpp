#include "EasyToast.h"
#include "ui_EasyToast.h"

#include <QStyle>
#include <QScreen>
#include <QApplication>

static EasyToast *toast()
{
    static EasyToast *pDlg = new EasyToast();
    pDlg->hide();
    return pDlg;
}

static void easyToast(const QString&text,int type,quint32 durtaion)
{
    toast()->active(text,type,durtaion);
}

EasyToast::EasyToast(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogToast)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Popup | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::Dialog);    // 必须设置无边框
    setAttribute(Qt::WA_TranslucentBackground); // 设置背景透明
    setStyleSheet("QFrame#frame{background-color: rgba(50, 50, 50, 250); border-radius: 10px; color:white;}");

    QSize icoSize(48,48);
    pmInformation=QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(icoSize);
    pmQuestion   =QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(icoSize);
    pmWarning    =QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(icoSize);
    pmCritical   =QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical).pixmap(icoSize);
    ui->labelIcon->setPixmap(pmInformation);

    connect(&m_TMShow,&QTimer::timeout,this,[=]{
        m_TMShow.stop();
        m_TMHide.stop();
        m_TMHide.start(60);
    });

    connect(&m_TMHide,&QTimer::timeout,this,[=]{
        m_opacity -= 0.1;
        setWindowOpacity(m_opacity);
        if(m_opacity <= 0.0)
        {
            m_TMHide.stop();
            this->hide();
        }
    });
}

EasyToast::~EasyToast()
{
    delete ui;
}

void EasyToast::information(const QString&text,int durtion)
{
    easyToast(text,0,durtion);
}

void EasyToast::warning(const QString&text,int durtion)
{
    easyToast(text,1,durtion);
}

void EasyToast::critical(const QString&text,int durtion)
{
    easyToast(text,2,durtion);
}

void EasyToast::question(const QString&text,int durtion)
{
    easyToast(text,3,durtion);
}

void EasyToast::close()
{
    toast()->hide();
}

void EasyToast::active(const QString&text, int type, int durtion)
{
    m_TMShow.stop();
    m_TMHide.stop();
    setWindowOpacity(1.0);
    m_opacity = 1.0;

    switch(type)
    {
    case 0: ui->labelIcon->setPixmap(pmInformation); break;
    case 1: ui->labelIcon->setPixmap(pmWarning);     break;
    case 2: ui->labelIcon->setPixmap(pmQuestion);    break;
    case 3: ui->labelIcon->setPixmap(pmCritical);    break;
    }
    ui->label->setStyleSheet("QLabel{color:white;font-size:24px;}");
    ui->label->setText(text);
    ui->label->adjustSize();
    ui->frame->adjustSize();
    this->adjustSize();
    // ui->label->setAlignment(Qt::AlignCenter);

    raise();

    if(durtion>0)
    {
        m_TMShow.start(durtion);
        this->exec();
    }
    else
    {
        this->show();
    }
}
