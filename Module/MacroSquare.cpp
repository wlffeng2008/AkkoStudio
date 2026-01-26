#include "MacroSquare.h"
#include "ui_MacroSquare.h"


#include "SuperLabel.h"

#include <QTimer>
#include <QKeyEvent>
#include <QPainter>

static MacroSquare *s_pCurItem = nullptr;
static QString s_strTipBtnStyle(R"(
    background-color: white;
    border: 1px solid #DEDEDE;
    border-radius: 4px;
    padding-left: 5px;
    padding-right: 5px;
    min-height:24px;
    max-height:24px;
    font-size: 14px;
    font-weight:400;
)");


static MacroSquare *s_Items[256]={nullptr};

MacroSquare *MacroSquare::getSquare(int index,QWidget *parent)
{
    MacroSquare *item = s_Items[index];
    if( !item) item = new MacroSquare(parent);
    s_Items[index] = item;
    return item;
}

MacroSquare::MacroSquare(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::MacroSquare)
{
    ui->setupUi(this);

    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);

    ui->spinBox1->hide();
    ui->spinBox2->hide();
    ui->label2->hide();

    hide();

    QTimer::singleShot(50,this,[=]{
        ui->label1->setAlignment(Qt::AlignCenter);
        ui->label2->setAlignment(Qt::AlignCenter);
        ui->label1->setFocusPolicy(Qt::StrongFocus);
        ui->label2->setFocusPolicy(Qt::StrongFocus);
        ui->spinBox1->setFocusPolicy(Qt::StrongFocus);
        ui->spinBox2->setFocusPolicy(Qt::StrongFocus);

        connect(ui->spinBox1,&QSpinBox::valueChanged,this,[=](int value){
            m_xpos = value;
            m_value = value;

            if(m_type == 2)
            {
                m_value = m_xpos << 8 | m_ypos;
            }
        });
        connect(ui->spinBox2,&QSpinBox::valueChanged,this,[=](int value){
            m_ypos = value;
            if(m_type == 2)
            {
                m_value = m_xpos << 8 | m_ypos;
            }
        });

        //setData("A",2,0x04,false);
    });

    m_tTip=new CustomTooltip(this);
    m_rTip=new CustomTooltip(this);
    m_bTip=new CustomTooltip(this);

    m_tTip->setStyleSheet(s_strTipBtnStyle);
    m_rTip->setStyleSheet(s_strTipBtnStyle);
    m_bTip->setStyleSheet(s_strTipBtnStyle);

    m_tTip->setText(tr("插入按键"));
    m_rTip->setText(tr("修改"));
    m_bTip->setText(tr("删除"));

    connect(m_tTip,&CustomTooltip::onClicked,this,[=]{ closeItems(); emit onAction(this,0); });
    connect(m_rTip,&CustomTooltip::onClicked,this,[=]{ closeItems(); emit onAction(this,1); });
    connect(m_bTip,&CustomTooltip::onClicked,this,[=]{ closeItems(); emit onAction(this,2); });
    m_tTip->setFocusPolicy(Qt::StrongFocus);
    m_tTip->installEventFilter(this);

    installEventFilter(this);
    ui->label1->installEventFilter(this);
    ui->label2->installEventFilter(this);
    ui->spinBox1->installEventFilter(this);
    ui->spinBox2->installEventFilter(this);
}

MacroSquare::~MacroSquare()
{
    delete ui;
}


void MacroSquare::setData(const QString&text, quint8 type,quint16 value, bool down)
{
    m_type = type;
    m_text = text;
    m_down = down;
    m_value= value;

    m_tTip->hide();
    m_rTip->hide();
    m_bTip->hide();
    ui->label2->hide();
    ui->spinBox1->hide();
    ui->spinBox2->hide();

    ui->label1->show();

    ui->label1->setAttribute(Qt::WA_Hover, false);
    ui->label2->setAttribute(Qt::WA_Hover, false);
    ui->label1->setStyleSheet("QLabel{color:black; background-color:transparent;}");
    ui->label2->setStyleSheet("QLabel{color:black; background-color:transparent;}");

    if(type == 3)
    {
        ui->spinBox1->setRange(5,0xFFFF);
        ui->label2->show();
        ui->label1->setAttribute(Qt::WA_Hover, true);
        ui->label1->setText(QString::asprintf("%d",value));
        ui->label2->setText("ms");
        ui->label1->setStyleSheet("QLabel:hover{color:black; background-color:#D0D0D0;border-radius:8px;}");
    }
    else if(type == 2)
    {
        m_xpos =  value&0x00FF    ;
        m_ypos = (value&0xFF00) >> 8;
        ui->spinBox1->setRange(0,127);
        ui->spinBox2->setRange(0,127);
        ui->label2->show();
        ui->label1->setAttribute(Qt::WA_Hover, true);
        ui->label2->setAttribute(Qt::WA_Hover, true);
        ui->label1->setText(QString::asprintf("X: %d", m_xpos));
        ui->label2->setText(QString::asprintf("Y: %d", m_ypos));

        ui->label1->setStyleSheet("QLabel:hover{color:black; background-color:#D0D0D0;border-radius:8px;}");
        ui->label2->setStyleSheet("QLabel:hover{color:black; background-color:#D0D0D0;border-radius:8px;}");
    }
    else
    {
        if(!down)
            ui->label1->setStyleSheet("QLabel{color:white; background-color:transparent;}");

        ui->label1->setText(text);

        m_tTip->setAutohide(false);
        m_rTip->setAutohide(false);
        m_bTip->setAutohide(false);
    }

    show();
    update();
}

void MacroSquare::closeItems()
{
    if(m_tTip)
    {
        m_tTip->hide();
        m_rTip->hide();
        m_bTip->hide();
    }
}

void MacroSquare::LostFocus()
{
    if(s_pCurItem) s_pCurItem->closeItems() ;
}

void MacroSquare::setDelay(quint16 delay)
{
    if(m_type == 3)
    {
        m_value = delay;
        ui->label1->setText(QString("%1").arg(delay));
        update();
    }
}

void MacroSquare::paintEvent(QPaintEvent*event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = this->rect().adjusted(3,3,-3,-3);

    if(m_type == 3)
    {
        painter.setPen(QPen(QBrush(0xFF9052),1));
        painter.setBrush(Qt::NoBrush);
        int nCY = rect.center().y()+2;
        painter.drawLine(QPoint(rect.left(),nCY),QPoint(rect.right(),nCY));
        painter.drawLine(QPoint(rect.right()-8,nCY+3),QPoint(rect.right(),nCY));
        painter.drawLine(QPoint(rect.right()-8,nCY-3),QPoint(rect.right(),nCY));
    }
    else
    {
        QColor penColor = 0x4B4B4B;

        painter.setBrush(Qt::white);
        if(!m_down && (m_type == 0 || m_type == 1))
        {
            painter.setBrush(0x4B4B4B);
        }

        QPen pen(penColor,0.5) ;
        pen.setCapStyle(Qt::FlatCap);
        painter.setPen(pen);
        painter.drawRoundedRect(rect,16,16);
    }

    if(m_bSelected)
    {
        painter.setRenderHint(QPainter::Antialiasing,false);
        painter.setPen(QPen(s_pCurItem == this ? Qt::green : Qt::blue, 0.5, Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(this->rect().adjusted(1,1,-1,-1));
    }

    if(s_pCurItem != this) closeItems();

    event->accept();
}

bool MacroSquare::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        if(ui->label1 == object && (m_type == 2 ||m_type == 3))
        {
            closeItems();
            ui->spinBox1->show();
            ui->label1->hide();
            ui->spinBox1->setValue(m_xpos);

            return true;
        }

        if(ui->label2 == object && m_type == 2)
        {
            closeItems();
            ui->spinBox2->show();
            ui->label2->hide();
            ui->spinBox2->setValue(m_ypos);
            return true;
        }
    }

    if(event->type() == QEvent::FocusOut)
    {
        if(ui->spinBox1 == object)
        {
            ui->spinBox1->hide();
            ui->label1->show();
            ui->label1->setText(QString("%1").arg(m_value));
            if(m_type == 2)
                ui->label1->setText(QString("X: %1").arg(m_xpos));
        }

        if(ui->spinBox2 == object)
        {
            ui->spinBox2->hide();
            ui->label2->show();
            ui->label2->setText(QString("Y: %1").arg(m_ypos));
        }

        if(m_tTip == object)
            closeItems();
    }

    return QFrame::eventFilter(object,event);
}

bool MacroSquare::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        MacroSquare *old = s_pCurItem;
        s_pCurItem = this;
        if(old) old->update();
        m_bSelected = !m_bSelected;
        update();
\
        closeItems();
    }

    if(event->type() == QEvent::MouseButtonRelease)
    {
        if(m_type != 3)
        {
            QPoint pos = mapToGlobal(QPoint(width()+1,(height() - m_rTip->height())/2));
            m_rTip->move(pos);

            pos = mapToGlobal(QPoint(size().width()/2 - m_tTip->width()/2,( -1 - m_tTip->height())));
            m_tTip->move(pos);

            pos = mapToGlobal(QPoint(size().width()/2 - m_bTip->width()/2,(height() + 1)));
            m_bTip->move(pos);

            //m_rTip->show();
            m_tTip->show();
            m_bTip->show();
            m_tTip->setFocus();
        }
    }

    if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *pKE = static_cast<QKeyEvent*>(event);
        if(pKE->key() == Qt::Key_Enter || pKE->key() == Qt::Key_Return)
        {
            ui->spinBox1->hide();
            ui->spinBox2->hide();
        }
    }

    return QFrame::event(event);
}
