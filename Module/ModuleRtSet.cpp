#include "ModuleRtSet.h"
#include "ui_ModuleRtSet.h"

#include <QButtonGroup>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>


static QRect s_Rect ;
static int g_value = 500 ;
static int valueMin = 100 ;
static int valueMax = 3000 ;

ModuleRtSet::ModuleRtSet(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleRtSet)
{
    ui->setupUi(this);

    connect(ui->checkBoxRTMode,&QCheckBox::clicked,this,[&](bool checked){
        ui->pushButtonMinus->setEnabled(checked);
        ui->pushButtonPlug->setEnabled(checked);
        ui->lineEdit->setEnabled(checked);
        update();
    }) ;

    ui->checkBoxRTMode->setChecked(true);

    connect(ui->lineEdit,&QLineEdit::textEdited,this,[=](const QString &text){
        getValue();
        update();
        m_pSetTM->stop();
        m_pSetTM->start(300);
    });

    ui->pushButtonMinus->setAutoRepeat(true);
    ui->pushButtonMinus->setAutoRepeatInterval(100) ;
    connect(ui->pushButtonMinus,&QPushButton::pressed,this,[=]{
        setValue(g_value-5);
    });

    ui->pushButtonPlug->setAutoRepeat(true);
    ui->pushButtonPlug->setAutoRepeatInterval(100) ;
    connect(ui->pushButtonPlug,&QPushButton::pressed,this,[=]{
        setValue(g_value+5);
        update();
    }) ;

    installEventFilter(this);
    setMouseTracking(true);

    m_pSetTM = new QTimer(this);
    connect(m_pSetTM,&QTimer::timeout,this,[=]{
        m_pSetTM->stop();
        getValue();
        emit setGlobalRtValue(g_value);
    });
}

ModuleRtSet::~ModuleRtSet()
{
    delete ui;
}

void ModuleRtSet::setValue(float value)
{
    double tmp = value;

    if(tmp<valueMin) tmp=valueMin;
    if(tmp>valueMax) tmp=valueMax;

    ui->lineEdit->setText(QString::asprintf("%.3f mm",tmp/1000.0));

    g_value = tmp;
    m_pSetTM->stop();
    m_pSetTM->start(300);

    update();
}

float ModuleRtSet::getValue()
{
    QString text = ui->lineEdit->text();
    text = text.replace("mm","").trimmed();

    float tmp = text.toFloat() * 1000;

    if(tmp < valueMin)
    {
        tmp = valueMin;
        setValue(tmp);
    }
    if(tmp > valueMax)
    {
        tmp = valueMax;
        setValue(tmp);
    }

    g_value = tmp;

    return tmp;
}

bool ModuleRtSet::eventFilter(QObject*watched,QEvent*event)
{
    if(event->type() == QEvent::MouseButtonPress && ui->checkBoxRTMode->isChecked())
    {
        QMouseEvent *pEV = static_cast<QMouseEvent *>(event);
        QRect tmp = s_Rect.adjusted(-2,0,2,0);
        if(tmp.contains(pEV->pos()))
        {
            m_dragging = true;
            int value = (pEV->pos().x() - s_Rect.left()) * (valueMax-valueMin) / s_Rect.width() + valueMin;
            setValue(value/5*5);
        }
    }

    if(event->type() == QEvent::MouseButtonRelease)
    {
        m_dragging = false;
    }

    if(event->type() == QEvent::MouseMove)
    {
        if(m_dragging)
        {
            QMouseEvent *pEV = static_cast<QMouseEvent *>(event);
            QRect tmp = s_Rect.adjusted(-2,0,2,0);
            if(tmp.contains(pEV->pos()))
            {
                m_dragging = true;
                int value = (pEV->pos().x() - s_Rect.left()) * (valueMax-valueMin) / s_Rect.width() + valueMin;
                setValue(value/5*5);
            }
        }
    }

    return QFrame::eventFilter(watched,event);
}

void ModuleRtSet::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen Black(Qt::black,1);
    painter.setBrush(Qt::black);
    if(!ui->checkBoxRTMode->isChecked())
    {
        painter.setBrush(0xB7B7B7);
        Black.setColor(0xB7B7B7);
    }
    painter.setPen(Black) ;
    s_Rect = QRect(20,50,260,40);
    int nCount = 20;
    qreal xStep = s_Rect.width()*1.0/(nCount-1);
    for(int i=0; i<nCount; i++)
    {
        painter.drawLine(QPoint(s_Rect.left() + i*xStep,75),QPoint(s_Rect.left() + i*xStep,90));
    }

    int nTriLen = 12;
    float value = getValue();
    int nImgX = s_Rect.left() + s_Rect.width() * (value - valueMin) / (valueMax - valueMin) - nTriLen/2;

    painter.drawLine(QPoint(nImgX+nTriLen/2,66),QPoint(nImgX+nTriLen/2,90));
    painter.setRenderHint(QPainter::Antialiasing,true);

    QPolygonF triangle;
    triangle<<QPoint(nImgX,s_Rect.top());
    triangle<<QPoint(nImgX+nTriLen,s_Rect.top());
    triangle<<QPoint(nImgX+nTriLen/2,s_Rect.top() + nTriLen/2 * sqrt(3));
    painter.drawPolygon(triangle);

    event->accept();
}
