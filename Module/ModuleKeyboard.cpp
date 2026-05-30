#include "ModuleKeyboard.h"
#include "qdialog.h"
#include "ui_ModuleKeyboard.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

#include "keyboardbutton.h"
#include "DialogDeviceConnect.h"

static QList<ModuleKeyboard*>s_kbInstance;

ModuleKeyboard::ModuleKeyboard(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleKeyboard)
{
    ui->setupUi(this);

    setMinimumHeight(400);
    setMinimumWidth(960);

    for(int i=0; i<255; i++)
    {
        QString strName = QString::asprintf("pushButton_Hid%03d",i) ;
        KeyboardButton *btn = findChild<KeyboardButton*>(strName);
        if(!btn) continue;
        btn->setCheckable(false);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setCursor(Qt::PointingHandCursor);
        ui->buttonGroup->removeButton(btn);
        ui->buttonGroup->addButton(btn,i);
        if(i == 234 ||i == 233)
        {
            continue;
        }

        //btn->setMtFlag(rand()%2 ? Qt::green : Qt::blue);
    }

    ui->buttonGroup->setExclusive(false);

    connect(ui->buttonGroup,&QButtonGroup::idClicked, this, [=](int id){

        if(m_nSelectCount>1)
        {
            const QList<QAbstractButton*>btns = ui->buttonGroup->buttons();
            int nChecked = 0;
            for(QAbstractButton*btn:btns)
            {
                if(nChecked >= m_nSelectCount)
                {
                    btn->setChecked(false);
                    continue;
                }

                if(btn->isChecked())
                    nChecked++;
            }
        }

        QPushButton *btn = static_cast<QPushButton *>(ui->buttonGroup->button(id));
        emit onKeyClicked(btn->text(),btn->objectName().right(3).toUInt());
    });

    m_Menu = new CustomTooltip();
    m_Menu->setTextStyle("QLabel { color: red; }");

    connect(m_Menu,&CustomTooltip::onClicked,this,[=]{
        setButtonEnable(m_curBtn,!m_curBtn->isEnabled(),true);
    });

    s_kbInstance.push_back(this);
}

void ModuleKeyboard::setButtonEnable(QAbstractButton*btn, bool bEnable, bool bToDevice)
{
    KeyboardButton * tkb = static_cast<KeyboardButton *>(btn);
    if(bEnable)
    {
        btn->removeEventFilter(this);
        m_disables.removeAll(btn);
        btn->setEnabled(true);
    }
    else
    {
        if(!tkb->hasTip())
        {
            btn->setEnabled(false);
            btn->installEventFilter(this);
            m_disables.push_back(btn);
        }

        btn->setChecked(false);
        tkb->setTipText();
    }

    setKeyEnable(btn->objectName(),btn->isEnabled(),bToDevice,true);
}

ModuleKeyboard::~ModuleKeyboard()
{
    s_kbInstance.removeAll(this);
    delete ui;
}

void ModuleKeyboard::keepSpeacial()
{
    // return;
    KeyboardButton *btn1 = ui->pushButton_Hid234;
    KeyboardButton *btn2 = ui->pushButton_Hid233;
    if(m_bSetLightMode || m_bSetMtMode || m_bFixMode)
    {
        btn1->hide();
        btn2->hide();
    }
    else
    {
        btn1->show();
        btn2->show();
    }
}

void ModuleKeyboard::showEvent(QShowEvent *event)
{
    QTimer::singleShot(20,this,[=]{
        keepSpeacial();
    });
}

void ModuleKeyboard::setLightMode()
{
    m_bSetLightMode=true;

    const QList<QAbstractButton*>btns = ui->buttonGroup->buttons();
    for(QAbstractButton*btn:btns)
    {
        KeyboardButton *pKb = (KeyboardButton *)btn;
        pKb->setTipText();
        pKb->setCheckable(true);
    }
    keepSpeacial();
    ui->pushButton_Hid250->setEnabled(true);
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

void ModuleKeyboard::showMtFlag(bool show)
{
    m_bSetMtMode=true;
    const QList<QAbstractButton*>btns = ui->buttonGroup->buttons();
    for(QAbstractButton*btn:btns)
    {
        btn->setCheckable(true);
        (static_cast<KeyboardButton *>(btn))->setTipText();
        (static_cast<KeyboardButton *>(btn))->showMtFlag(show);
    }
    showFlag(!show);
    keepSpeacial();
}

void ModuleKeyboard::setColor(quint8 hid, const QColor &color)
{
    if(hid == 0xFF)
    {
        const QList<QAbstractButton*>btns = ui->buttonGroup->buttons();
        for(QAbstractButton*btn:btns)
        {
            //btn->setCheckable(true);
            (static_cast<KeyboardButton *>(btn))->setColor(color);
        }
        return;
    }
    QString strName = QString::asprintf("pushButton_Hid%03d",hid);
    QPushButton *btn = findChild<QPushButton*>(strName);
    if(btn)
    {
        static_cast<KeyboardButton *>(btn)->setColor(color);
    }
}

void ModuleKeyboard::setMtColor(quint8 hid, const QColor &color)
{
    QString strName = QString::asprintf("pushButton_Hid%03d",hid);
    QPushButton *btn = findChild<QPushButton*>(strName);
    if(btn)
    {
        static_cast<KeyboardButton *>(btn)->setMtFlag(color);
    }
}

void ModuleKeyboard::setKeyTip(quint8 hid, const QString&strTip1, const QString&strTip2, bool bSetToAll)
{
    QString strName = QString::asprintf("pushButton_Hid%03d",hid);
    setKeyTip(strName,strTip1,strTip2,bSetToAll);
}

void ModuleKeyboard::setKeyTip(const QString&objname,const QString&strTip1,const QString&strTip2,bool bSetToAll)
{
    if(m_bFixMode     ) return;
    if(m_bSetLightMode) return;
    if(m_bSetMtMode   ) return;

    QPushButton *btn = findChild<QPushButton*>(objname);
    if(btn)
    {
        if(strTip1 == "DISABLED")
        {
            btn->setDisabled(true);
            setButtonEnable(btn,false,false);
        }
        else
        {
            btn->setDisabled(false);
            static_cast<KeyboardButton *>(btn)->setTipText(strTip1, strTip2);
        }
    }

    if(!bSetToAll) return;

    for(ModuleKeyboard*pkb:s_kbInstance)
        pkb->setKeyTip(objname, strTip1, strTip2, false);
}

void ModuleKeyboard::setKeyEnable(const QString&objname, bool bEnable, bool bToDevice, bool bSetToAll)
{
    if(m_bFixMode     ) return;
    if(m_bSetLightMode) return;
    if(m_bSetMtMode   ) return;

    QPushButton *btn = findChild<QPushButton*>(objname);
    if(btn) btn->setEnabled(bEnable);

    if(!bSetToAll) return;
    quint8 hid = objname.right(3).toInt();
    DialogDeviceConnect *pCnn = DialogDeviceConnect::instance();
    quint8 type = pCnn->getKeyType(hid);
    if(bToDevice)
    {
        pCnn->restKey(hid);
        emit onKeyChanged(hid,type,bEnable);
        if(type == 7)
        {
            quint8 index=::getIndex(hid);
            quint8 snapKid=pCnn->getSnapkey(index);
            pCnn->send65Cmd(0x07,snapKid,0,true);
            QString strName = QString::asprintf("pushButton_Hid%03d",snapKid);
            KeyboardButton *btn = findChild<KeyboardButton*>(strName);
            btn->setTipText();
        }
        pCnn->enableKey(hid,bEnable);
    }

    for(ModuleKeyboard*pkb:s_kbInstance)
        pkb->setKeyEnable(objname,bEnable,false,false);
}

void ModuleKeyboard::setkeyHited(int id)
{
    int nCode = id ;
    if(nCode == 0) nCode = 0x81;

    QString strName = QString::asprintf("pushButton_Hid%03d",nCode);
    QPushButton *btn = findChild<QPushButton*>(strName);
    if(btn)
    {
        btn->setStyleSheet(R"(
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
            //btn->setStyleSheet("QPushButton:disabled { background-color: white; color: black; }" );
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
                    m_Menu->setText(static_cast<KeyboardButton *>(btn)->hasTip()?tr("清除设置"):tr("禁用按键")) ;
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
