#include "DialogFNPicker.h"
#include "ui_DialogFNPicker.h"

#include <QApplication>
#include <QStyleOption>
#include <QScreen>
#include <QStyleOption>
#include <QPainter>
#include <QEvent>

static QPoint getGlobalPos(QWidget *widget) {
    QPoint pos = widget->pos();
    QWidget *w = widget->parentWidget();
    if(w && !(w->windowFlags() & Qt::Popup))
        pos = w->mapToGlobal(pos) ;
    return pos;
}

DialogFNPicker::DialogFNPicker(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogFNPicker)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet("QDialog { background-color: rgba(230, 230, 230, 0.9);  border: none; border-radius: 24px;}");

    QRect geoMetry = QApplication::primaryScreen()->geometry();
    if(parent)
    {
        geoMetry = parent->frameGeometry() ;//parent->mapToGlobal(parent->pos());
        QPoint globalPos = getGlobalPos(parent);
        geoMetry = QRect(globalPos.x(),globalPos.y(),geoMetry.width(),geoMetry.height()) ;
    }
    setGeometry(geoMetry);
    setFixedSize(geoMetry.width(), geoMetry.height());

    connect(ui->pushButtonCancel,&QPushButton::clicked,this,[=]{ this->reject(); });
    connect(ui->pushButtonOK,&QPushButton::clicked,this,[=]{ this->accept(); });

    m_kd.b0=0;
    m_kd.b1=0;
    m_kd.b2=0;
    m_kd.b3=0;

    ModuleMacroManager *pMM = ModuleMacroManager::instance();
    connect(ui->buttonGroup,&QButtonGroup::buttonClicked,this,[=](QAbstractButton *btn){

        m_macro = false;
        QString name = btn->objectName();
        quint8 index = name.right(2).toLatin1().toUInt();
        if(name.contains("_Micro"))
        {
            m_macro = true;
            m_kd.b0 = ui->spinBoxM->value();
            m_kd.b1 = 0;
            if(ui->radioButtonM2->isChecked()) m_kd.b1 = 1;
            if(ui->radioButtonM3->isChecked()) m_kd.b1 = 2;
            m_kd.b2 = index;
            MacroProject *prj = pMM->getMarcoProject(index);
            if(prj)
                ui->labelSelect1->setText(prj->name);
            else
                ui->labelSelect1->setText("尚未录制的宏");
        }
        else
        {
            if(name.contains("_F"))     m_kd = *(keyData *)getFnData(index-1);
            if(name.contains("_Mouse")) m_kd = *(keyData *)getMuData(index);

            ui->labelSelect1->setText(getKeyString(&m_kd));
        }
    });

    {
        QStringList Files={
             "fn.png",
             "houtui.png",
             "kuaijin.png",
             "zanting.png",
             "guanbiyinliang.png",
             "yinliangjia.png",
             "yinliangjian.png",
             "yinyue.png",
             "jisuanqi.png",
             "youjian.png",

             "frame.png",
             "sousuo.png",
             "shouye.png",
             "shuaxin.png",
             "jianpanliangdujia.png",
             "jianpanliangdu.png",
             "fangda.png",
             "suoxiao.png",
             "yuyin.png"
        };
        for(int i=0; i<19; i++)
        {
            QString strName = QString::asprintf("pushButton_F%02d",i+1) ;
            QPushButton *btn = findChild<QPushButton*>(strName) ;
            if(!btn) continue;

            btn->setCheckable(true);
            btn->setText("");

            QString strStyle=QString(R"(
                QPushButton {
                    icon: url(:/images/macro/fn0/%1);
                    icon-size: 24px 24px;
                    border-radius: 16px ;
                    border: 1px soild #ECECEC;
                    background: #FFFFFF;
                    padding: 2px; }

                QPushButton:checked,pressed {
                    icon: url(:/images/macro/fn1/%2);
                    background-color: #3F3F3F; }

                QPushButton:hover {
                    icon: url(:/images/macro/fn1/%3);
                    background-color: #8F8F8F; }
            )").arg(Files[i],Files[i],Files[i]);

            btn->setStyleSheet(strStyle);
            btn->setToolTip(getKeyString(getFnData(i)));
            btn->setFocusPolicy(Qt::NoFocus) ;
            btn->setCursor(Qt::PointingHandCursor) ;
        }

        for(int i=0; i<50; i++)
        {
            QPushButton *mBtn = new QPushButton(QString("宏")+QString("%1").arg(i+1),this);
            QString strName = QString::asprintf("pushButton_Micro%02d",i);
            mBtn->setObjectName(strName);

            MacroProject *prj = pMM->getMarcoProject(i);
            if(prj && prj->events.size() >= 2)
                mBtn->setText(prj->name);

            ui->buttonGroup->addButton(mBtn);
            ui->gridLayout2->addWidget(mBtn,i/12,i%12);
        }
    }

}

DialogFNPicker::~DialogFNPicker()
{
    delete ui;
}

void DialogFNPicker::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QDialog::paintEvent(event);
}
