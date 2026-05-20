#include "DialogVKPicker.h"
#include "ui_DialogVKPicker.h"

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

DialogVKPicker::DialogVKPicker(QWidget *parent)
    : QDialog(nullptr)
    , ui(new Ui::DialogVKPicker)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet("QDialog { background-color: rgba(240, 240, 240, 0.85); border: none; border-radius: 24px; font-weight: 600;}");

    QRect geoMetry = QApplication::primaryScreen()->geometry();
    if(parent)
    {
        geoMetry = parent->frameGeometry(); //parent->mapToGlobal(parent->pos());
        QPoint globalPos = getGlobalPos(parent);
        geoMetry = QRect(globalPos.x(),globalPos.y(),geoMetry.width(),geoMetry.height());
    }
    setGeometry(geoMetry);
    setFixedSize(geoMetry.width(), geoMetry.height());

    connect(ui->pushButtonCancel,&QPushButton::clicked,this,[=]{ this->reject(); });
    connect(ui->pushButtonOK,&QPushButton::clicked,this,[=]{ this->accept(); });

    ui->labelSelect1->installEventFilter(this);
    ui->labelSelect2->installEventFilter(this);
    ui->labelSelect3->installEventFilter(this);

    m_kd.b0=0;
    m_kd.b1=0;
    m_kd.b2=0;
    m_kd.b3=0;

    connect(ui->buttonGroup,&QButtonGroup::buttonClicked,this,[=](QAbstractButton *btn){

        QString text = btn->text();
        quint8 hid = btn->objectName().right(3).toUInt();
        if(m_kd.b1 == 0)
        {
            m_kd.b1 = hid;
            ui->labelSelect1->setText(text);
            return;
        }

        if(m_kd.b2 == 0)
        {
            m_kd.b2 = hid;
            ui->labelSelect2->setText(text);
            return;
        }

        if(m_kd.b3 == 0)
        {
            m_kd.b3=hid;
            ui->labelSelect3->setText(text);
            return;
        }
    });
}

DialogVKPicker::~DialogVKPicker()
{
    delete ui;
}

bool DialogVKPicker::eventFilter(QObject *watched,QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        if(watched == ui->labelSelect1)
        {
            m_kd.b1 = 0;
            ui->labelSelect1->setText("--");
        }

        if(watched == ui->labelSelect2)
        {
            m_kd.b2 = 0;
            ui->labelSelect2->setText("--");
        }

        if(watched == ui->labelSelect3)
        {
            m_kd.b3 = 0;
            ui->labelSelect3->setText("--");
        }
    }

    return QDialog::eventFilter(watched,event);
}


void DialogVKPicker::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QDialog::paintEvent(event);
}
