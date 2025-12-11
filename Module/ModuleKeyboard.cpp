#include "ModuleKeyboard.h"
#include "qdialog.h"
#include "ui_ModuleKeyboard.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>


static QList<ModuleKeyboard*>s_kbInstance ;


static QString strStyle(R"(
        QPushButton {
                border: 1px solid #EAEAEA;
                color: black;
                background-color: #FBFBFB;
                border-radius: 14px;
                padding: 2px 2px;
                outline: none;
                min-width:42px;
                max-width:542px;
                min-height:42px;
            }

        QPushButton:hover { background-color: #EAEAEA; border: 1px solid #EAEAEA;}
        QPushButton:pressed { background-color: #3F3F3F; }
        QPushButton:checked { background-color: #3F3F3F; color: white; }
        QPushButton:disabled { background-color: #EAEAEA; color: #8C8C8C; }
        )");

ModuleKeyboard::ModuleKeyboard(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleKeyboard)
{
    ui->setupUi(this);

    setMinimumHeight(400) ;
    setMinimumWidth(960) ;

    QTimer::singleShot(100,this,[=]{
        if(parent)
        {
            //parent->setMinimumSize(960,400) ;
            //parent->layout()->setAlignment(Qt::AlignCenter);
            //qDebug() << "parent->parentWidget()->layout()->setAlignment(Qt::AlignCenter)";
        }
    });

    for(int i=0; i<255; i++)
    {
        QString strName = QString::asprintf("pushButton_Hid%03d",i) ;
        KeyboardButton *btn = findChild<KeyboardButton*>(strName) ;
        if(!btn) continue;
        btn->setStyleSheet(strStyle);
        btn->setCheckable(true) ;
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setCursor(Qt::PointingHandCursor) ;
        ui->buttonGroup->removeButton(btn);
        ui->buttonGroup->addButton(btn,i) ;
        if(i == 0xE000)
        {
            m_spcBtn = btn ;
            btn->showMtFlag(false);
        }
        btn->setMtFlag(rand()%2 ? ":/images/mt0.png" : ":/images/mt1.png") ;
    }

    QTimer::singleShot(100,this,[=]{if(m_spcBtn) m_spcBtn->setStyleSheet(strStyle + "QPushButton{ border-radius:24px;}");}) ;

    ui->buttonGroup->setExclusive(false) ;

    connect(ui->buttonGroup,&QButtonGroup::idClicked,this,[=](int id){

        if(m_nSelectCount>1)
        {
            const QList<QAbstractButton*>btns = ui->buttonGroup->buttons();
            int nChecked = 0;
            for(QAbstractButton*btn:btns)
            {
                if(nChecked >= m_nSelectCount)
                {
                    btn->setChecked(false);
                    continue ;
                }

                if(btn->isChecked())
                    nChecked++ ;
            }
        }

        QPushButton *btn = static_cast<QPushButton *>(ui->buttonGroup->button(id)) ;
        emit onKeyClicked(btn->text(),btn->objectName().right(3).toUInt()) ;
        // qDebug() << "Clicked:" << id << btn->objectName() ;
    });

    m_Menu = new CustomTooltip() ;
    m_Menu->setTextStyle("QLabel { color: red; }") ;

    connect(m_Menu,&CustomTooltip::onClicked,this,[=]{
        if(m_curBtn->isEnabled())
        {
            m_curBtn->setEnabled(false);
            m_curBtn->installEventFilter(this);
            m_disables.push_back(m_curBtn);
        }
        else
        {
            m_curBtn->removeEventFilter(this);
            m_disables.removeAll(m_curBtn);
            m_curBtn->setEnabled(true);
        }
        setKeyEnable(m_curBtn->objectName(),m_curBtn->isEnabled(),true);
    }) ;

    s_kbInstance.push_back(this);
}

ModuleKeyboard::~ModuleKeyboard()
{
    s_kbInstance.removeAll(this);
    delete ui;
}

void ModuleKeyboard::setSelectCount(int count)
{
    m_nSelectCount = count;
    ui->buttonGroup->setExclusive(false);
}

void ModuleKeyboard::setSingleMode(bool set)
{
    ui->buttonGroup->setExclusive(set);
}

void ModuleKeyboard::keyPressEvent(QKeyEvent *event)
{
    //qDebug() << "ModuleKeyboard::keyPressEvent" << event->nativeVirtualKey();
    QFrame::keyPressEvent(event);
}

void ModuleKeyboard::showFlag(bool show)
{
    ui->frameFlag->setVisible(show);
}

void ModuleKeyboard::setKeyEnable(const QString&objname,bool bEnable,bool bSetToAll)
{
    if(m_bFixMode) return;

    QPushButton *btn = findChild<QPushButton*>(objname);
    if(btn) btn->setEnabled(bEnable);

    if(!bSetToAll) return;

    for(ModuleKeyboard*pkb:s_kbInstance)
        pkb->setKeyEnable(objname,bEnable,false);
}

void ModuleKeyboard::setkeyHited(int id)
{
    int nCode = id ;
    if(nCode == 0) nCode = 0x81;

    QString strName = QString::asprintf("pushButton_Hid%03d",nCode);
    QPushButton *btn = findChild<QPushButton*>(strName);
    if(btn)
    {
        btn->setStyleSheet(strStyle + R"(
            QPushButton:disabled { background-color: #FF9052; color: white; }
        )") ;
    }
}

void ModuleKeyboard::setKeyFixMode()
{
    ui->frameFlag->hide();

    QTimer::singleShot(10,this,[=]{
        m_bFixMode = true;
        const QList<QAbstractButton*>btns = ui->buttonGroup->buttons();
        for(QAbstractButton*btn:btns)
        {
            btn->setEnabled(false);
            btn->setStyleSheet(strStyle + "QPushButton:disabled { background-color: white; color: black; }" +
                               (m_spcBtn == btn ? "QPushButton{ border-radius:24px;}":""));
        }
        update();
    });
}

void ModuleKeyboard::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    if(m_draging && !m_bFixMode)
    {
        QPainter painter(this);
        QRect rect(m_clkPt,m_nowPt);

        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QBrush(Qt::blue),2,Qt::DashLine));
        painter.drawRect(rect);
    }
}

bool ModuleKeyboard::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        m_draging=false;
        QMouseEvent *pME = static_cast<QMouseEvent *>(event);
        if(pME->button() == Qt::RightButton && m_Menu)
        {
            QPoint clkPt = mapFromGlobal(cursor().pos());
            const QList<QAbstractButton*>btns = ui->buttonGroup->buttons();
            for(QAbstractButton*btn:btns)
            {
                if(btn->geometry().contains(clkPt))
                {
                    m_curBtn = btn;
                    m_Menu->setText(tr("禁用该按键")) ;
                    QPoint pos = btn->mapToGlobal(QPoint(btn->width()+5,(btn->height() - m_Menu->height())/2));
                    m_Menu->move(pos);
                    m_Menu->show();
                    break;
                }
            }
        }
        update();
    }
    return QFrame::event(event) ;
}

bool ModuleKeyboard::eventFilter(QObject *watched,QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *pME = static_cast<QMouseEvent *>(event) ;
        if(pME->button() == Qt::RightButton && m_Menu)
        {
            for(QAbstractButton*btn:m_disables)
            {
                if(btn == watched)
                {
                    m_curBtn = btn;
                    m_Menu->setText(tr("取消禁用")) ;
                    QPoint pos = btn->mapToGlobal(QPoint(btn->width()+5,(btn->height() - m_Menu->height())/2));
                    m_Menu->move(pos);
                    m_Menu->show();
                    break;
                }
            }
            update();
        }
    }
    return QFrame::eventFilter(watched,event);
}

void ModuleKeyboard::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && !m_bFixMode)
    {
        m_clkPt = event->pos();
        m_nowPt = event->pos();
        m_draging=true;
        update();
    }
    QFrame::mousePressEvent(event);
}

void ModuleKeyboard::mouseMoveEvent(QMouseEvent *event)
{
    if(m_draging && m_clkPt.x()>0)
    {
        m_nowPt = event->pos() ;
        QRect rect(m_clkPt,m_nowPt);

        const QList<QAbstractButton*>btns = ui->buttonGroup->buttons() ;
        for(QAbstractButton*btn:btns)
        {
            QRect btnRc = btn->rect();
            btn->setChecked(
                rect.contains( btn->mapToParent(btnRc.topLeft()    ) )||
                rect.contains( btn->mapToParent(btnRc.topRight()   ) )||
                rect.contains( btn->mapToParent(btnRc.bottomLeft() ) )||
                rect.contains( btn->mapToParent(btnRc.bottomRight()) )
                );
        }

        update();
    }

    QFrame::mouseMoveEvent(event);
}

void ModuleKeyboard::mouseReleaseEvent(QMouseEvent *event)
{
    m_draging=false;
    m_clkPt = event->pos();
    update();

    QFrame::mouseReleaseEvent(event);
}
