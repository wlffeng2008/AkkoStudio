#include "LinearFixing1.h"
#include "LinearFixing2.h"
#include "DialogDeviceConnect.h"

#include <QScreen>
#include <QPainter>
#include <QStyleOption>
#include <QApplication>

LinearFixing1::LinearFixing1(const QString& title,const QString& content, QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //setModal(true);

    setStyleSheet("QDialog{background-color: rgba(120, 120, 120, 0.8);  border: 1px solid transparent;border-radius:20px;}"); // 黑色半透明

    QWidget* pContentWidget = new QWidget(this);
    pContentWidget->setObjectName("pContentWidget");
    pContentWidget->setStyleSheet(R"(
        QWidget {
            background-color: white;
            border-radius: 32px;
            padding: 10px;
        }
    )");

    QLabel *pLabTitle = new QLabel(tr("行程校准操作指南"), pContentWidget);
    pLabTitle->setStyleSheet("font-size: 18px; font-weight:600px; color: #333; text-align: center;");
    pLabTitle->setWordWrap(true);
    pLabTitle->setAlignment(Qt::AlignCenter);

    QLabel *pLabText = new QLabel(tr("请依次将每个按键轻按到底并停留至少1秒钟，直到按键变为橙色（表示该按键校准成功），请勿遗漏，可重复按按键。无线下不支持按键校准个数提示。校准过程中请勿拔出数据线。"), pContentWidget);
    pLabText->setStyleSheet("font-size: 13px; color: #333; text-align: left; min-height:80px; margin: 5px 20px;");
    pLabText->setWordWrap(true);
    pLabText->setAlignment(Qt::AlignLeft);

    QPushButton* confirmBtn = new QPushButton(tr("开始校准"), pContentWidget);
    confirmBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #6329B6;
            color: white;
            width:102px;
            height:32px;
            padding: 2px 2px;
            border: none;
            border-radius: 12px;
            font-size: 14px;
        }
        QPushButton:hover { background-color: #0E42F2; }
        QPushButton:pressed { background-color: #0A34A1; }
    )");

    DialogDeviceConnect *pCnn = DialogDeviceConnect::instance();

    connect(confirmBtn, &QPushButton::clicked, this, [=]{
        pCnn->StartCalibration();

        LinearFixing2 T2("","",this);
        T2.setGeometry(frameGeometry());
        T2.exec();

        pCnn->StopCalibration();

        QDialog::reject();
    });

    QPushButton* cancelBtn = new QPushButton(tr("取消"), pContentWidget);
    cancelBtn->setStyleSheet(R"(
        QPushButton {
            background-color: gray;
            color: white;
            width:102px;
            height:32px;
            padding: 2px 2px;
            border: none;
            border-radius: 12px;
            font-size: 14px;
        }
        QPushButton:hover { background-color: #C2C2C2; }
        QPushButton:pressed { background-color: #0A34A1; }
    )");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    cancelBtn->setFocusPolicy(Qt::NoFocus) ;
    cancelBtn->setCursor(Qt::PointingHandCursor) ;
    confirmBtn->setFocusPolicy(Qt::NoFocus) ;
    confirmBtn->setCursor(Qt::PointingHandCursor) ;

    // 内容布局
    QVBoxLayout* contentLayout = new QVBoxLayout(pContentWidget);
    contentLayout->addWidget(pLabTitle);
    contentLayout->addWidget(pLabText,1);
    contentLayout->setSpacing(5);

    QHBoxLayout* btnLayout = new QHBoxLayout(nullptr) ;
    btnLayout->setObjectName("btnLayout");
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(confirmBtn);
    btnLayout->setAlignment(Qt::AlignHCenter);
    btnLayout->setSpacing(18) ;
    btnLayout->setContentsMargins(10,10,10,10) ;
    contentLayout->addItem(btnLayout) ;

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName("mainLayout");
    mainLayout->addWidget(pContentWidget, 1, Qt::AlignCenter);

    pContentWidget->setFixedSize(426,223);
    pContentWidget->adjustSize();

    QRect geoMetry = QApplication::primaryScreen()->geometry();
    qDebug() << parent;
    if(parent) geoMetry = parent->frameGeometry() ;
    setGeometry(geoMetry);
    setFixedSize(geoMetry.width(), geoMetry.height());
}

void LinearFixing1::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QDialog::paintEvent(event);
}
