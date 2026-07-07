#include "FrameMacro.h"
#include "ModuleGenKeymapping.h"
#include "modulemacromanager.h"
#include "ui_FrameMacro.h"

#include "MacroItem.h"
#include "MacroSquare.h"
#include "ModuleGeneralMasker.h"
#include "ModuleAddMacroSquare.h"

#include <QLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>

static QList<MacroSquare*>s_MSquares;

FrameMacro::FrameMacro(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameMacro)
{
    ui->setupUi(this);
    srand(time(nullptr));

    m_pMM = new ModuleMacroManager(this);

    {
        QLayout *pLayout = ui->scrollAreaWidgetContents1->layout();
        pLayout->setSpacing(10) ;
        pLayout->setContentsMargins(5,5,5,5);
        pLayout->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

        m_loading=true;
        for(int i=0; i<50; i++)
        {
            addMacroBar((QObject*)m_pMM->getMarcoProject(i));
        }
        m_loading=false;
    }

    {
        connect(ui->pushButtonAddMacro,&QPushButton::clicked,this,[=](){
            addMacroBar((QObject*)m_pMM->addMacroProject());
        });

        connect(ui->pushButtonDelete,&QPushButton::clicked,this,[=]{
            removeView();
            saveEvents();
        });

        connect(ui->pushButtonClear,&QPushButton::clicked,this,[=]{
            int nCount = s_MSquares.count();
            for(int i=nCount-1; i>=0; i--)
            {
                MacroSquare *btn = s_MSquares[i];
                btn->hide();
            }
            s_MSquares.clear();
            m_prj->events.clear();
            m_delay=nullptr;
            saveEvents();
            updateView();
        });

        connect(ui->pushButtonSave,&QPushButton::clicked,this,[=]{ saveEvents(); });

        connect(ui->pushButtonInsert,&QPushButton::clicked,this,[=]{
            static ModuleAddMacroSquare *pEvt = new ModuleAddMacroSquare(this);
            ModuleGeneralMasker gMask(pEvt,ui->frameRight);
            pEvt->show();
            pEvt->update();
            gMask.setStyleSheet("QDialog { background-color: rgba(200, 200, 200, 0.9); border: none; border-radius: 32px; }");
            auto res = gMask.exec();
            if(res == QDialog::Accepted)
            {
                //m_loading = true;
                m_recording = true;

                if(pEvt->type() == 0)
                {
                    quint8 hid = pEvt->bKey();
                    if(hid)
                    {
                        addMacroSquare(::getKeyValue(hid),0,hid,true);
                        addMacroSquare(::getKeyValue(hid),0,hid,false);
                    }
                }
                else if(pEvt->type() == 1)
                {
                    addMacroSquare(tr("鼠标"),1,pEvt->mKey(),true);
                    addMacroSquare(tr("鼠标"),1,pEvt->mKey(),false);
                }
                else
                {
                    quint16 value = ((pEvt->xPos()<<8) | pEvt->yPos());
                    addMacroSquare(tr("位置"),2,value,false);
                }

                m_loading = false;
                m_recording = false;
                m_delay = nullptr;
                m_insert = nullptr;

                QTimer::singleShot(20,this,[=]{
                    saveEvents();
                });
            }
        });

        connect(ui->pushButtonRecord,&QPushButton::clicked,this,[=]{
            m_delay=nullptr;
            m_recording = ui->pushButtonRecord->isChecked();
            ui->pushButtonRecord->setText(m_recording ? tr("停止录制") : tr("开始录制"));
            if(!m_recording)
                saveEvents();
            setFocus();
        });

        connect(ui->pushButtonSetDelay,&QPushButton::clicked,this,[=]{
            quint16 delay = ui->spinBoxDelay->value();
            for(MacroSquare *item:std::as_const(s_MSquares))
            {
                item->setDelay(delay);
            }

            saveEvents();
        });
    }

    setFocusPolicy(Qt::StrongFocus);
}

FrameMacro::~FrameMacro()
{
    delete ui;
}

void FrameMacro::addMacroSquare(const QString&text, quint8 type, quint16 value, bool down)
{
    if(!m_loading)
    {
        if(!m_recording) return;
        if(!m_canAdd)    return;
    }

    {
        quint16 final = value;
        if(type == 1)
        {
            if(final == Qt::LeftButton)   final = 240;
            if(final == Qt::RightButton)  final = 241;
            if(final == Qt::MiddleButton) final = 242;
        }

        QString strText = text;
        if(text.contains(' '))
            strText = text.left(2);
        if(final == 240) strText=tr("左键");
        if(final == 241) strText=tr("右键");
        if(final == 242) strText=tr("中键");

        int addAt = s_MSquares.count();
        if(m_insert)
            addAt = s_MSquares.indexOf(m_insert);
        MacroSquare *macro = MacroSquare::getSquare(s_MSquares.count(),this);
        macro->setData(strText.trimmed(), type, final, down);
        macro->setFixedSize(62,62);

        s_MSquares.insert(addAt+0,macro);

        if(!macro->m_bConnected)
        {
            macro->m_bConnected = true;
            connect(macro,&MacroSquare::onAction,[=](MacroSquare *from, quint8 action){
                if(action == 0)
                {
                    m_insert = from;
                    ui->pushButtonInsert->click();
                }

                if(action == 2)
                {
                    int index = s_MSquares.indexOf(from);
                    if(index >= 0)
                    {
                        s_MSquares[index+0]->hide();
                        s_MSquares[index+1]->hide();
                        s_MSquares.removeAt(index);
                        s_MSquares.removeAt(index);
                        updateView();
                        saveEvents();
                    }
                }
            });
        }

        MacroSquare *delay = MacroSquare::getSquare(s_MSquares.count(),this);
        delay->setData(tr("延迟"), 3, 50, false);
        delay->setFixedSize(56,56);
        s_MSquares.insert(addAt+1,delay);
        m_delay = delay;
    }

    if(!m_loading)
        updateView();
}

void FrameMacro::addMacroBar(QObject *item)
{
    MacroProject *prj=(MacroProject *)item;
    if(!prj) return;
    QLayout *pLayout = ui->scrollAreaWidgetContents1->layout();
    MacroItem *MItem = new MacroItem(this);
    MItem->setFixedSize(220,44);
    MItem->setMacroName(prj->name);
    MItem->setRelData(item);
    pLayout->addWidget(MItem);
    MItem->show();

    connect(MItem,&MacroItem::onOperation,this,[=](int action,QWidget *widget){
        MacroItem *item=(MacroItem *)widget;
        item->setActive();
        m_prj = (MacroProject *)item->getRelData();

        if(action == 1)
        {
            deleteMacro(item);
        }

        if(action == 2)
        {
            m_prj->name = item->getMarcoName();
            m_pMM->saveLoadHeader();
        }

        if(action == 0)
        {
            ui->labelTitle1->setText(item->getMarcoName());
            loadEvents();
        }
    });

    if(!m_prj)
    {
        m_prj = prj;
        ui->labelTitle1->setText(prj->name);
        MItem->setActive();
        loadEvents();
    }

    QTimer *pTMUpdate = new QTimer(this);
    pTMUpdate->stop();
    pTMUpdate->start(50);
    connect(pTMUpdate,&QTimer::timeout,this,[=]{
        //MItem->setActive();
        ui->scrollAreaWidgetContents1->adjustSize();
        ui->scrollAreaWidgetContents1->update();
        QScrollBar *vScrollBar = ui->scrollArea1->verticalScrollBar();
        vScrollBar->setValue(vScrollBar->maximum());
    });
}

void FrameMacro::deleteMacro(QWidget *item)
{
    MacroProject *prj = (MacroProject *)((MacroItem *)item)->getRelData();
    if(!prj) return;
    qDebug() << "Remove: " << prj->name;
    item->hide();
    ui->scrollAreaWidgetContents1->layout()->removeWidget(item);

    m_pMM->delMacroProject(prj);
    m_prj = nullptr;
    update();
    updateView();
}

void FrameMacro::updateView()
{
    MacroSquare::LostFocus();

    QGridLayout *pLayout = ui->gridLayout;
    pLayout->setSpacing(4);
    pLayout->setContentsMargins(10,10,0,0);
    pLayout->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    while(pLayout->count())
    {
        MacroSquare *item = (MacroSquare *)pLayout->takeAt(0)->widget();
        if(!item) break;
        pLayout->removeWidget(item);
        item->hide();
    }

    int nCount = s_MSquares.count();
    for(int i=0; i<nCount; i++)
    {
        MacroSquare *btn = s_MSquares[i];
        pLayout->addWidget(btn,i/12,i%12);
        btn->setSelected(false);
        btn->show();
    }
    pLayout->invalidate();
    update();

    QTimer::singleShot(10,this,[=]{
        ui->scrollAreaWidgetContents2->adjustSize();
        ui->scrollAreaWidgetContents2->update();
        QScrollBar *vScrollBar = ui->scrollArea2->verticalScrollBar();
        vScrollBar->setValue(vScrollBar->maximum());
    });

    if(!m_loading)
    {
        saveEvents();
        QByteArray data = ModuleMacroManager::packMacroPack(m_prj);
        int size = data.size();
        if(size >= 2) size += 2;
        ui->labelTitle4->setText(tr("占用") + QString::asprintf(": %d / 256",size) + tr("字节"));
    }
}

void FrameMacro::removeView()
{
    int nCount = s_MSquares.count();
    for(int i = nCount-2; i >= 0; i -= 2)
    {
        MacroSquare *btn0 = s_MSquares[i+0];
        MacroSquare *btn1 = s_MSquares[i+1];
        if(btn0->isSelected() && btn0->m_type != 3)
        {
            btn0->hide();
            btn1->hide();
            s_MSquares.removeAt(i);
            s_MSquares.removeAt(i);
            m_prj->events.removeAt(i);
            m_prj->events.removeAt(i);
        }
    }
    updateView();
    m_delay = nullptr;
}

bool FrameMacro::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonDblClick)
    {
        m_loading = false;
        m_lastDelay = m_tcount.elapsed();
        if(m_delay) m_delay->setDelay(m_lastDelay);
        m_tcount.restart();

        QMouseEvent *pME = static_cast<QMouseEvent *>(event);
        quint8 btn = pME->button();
        addMacroSquare("",1,btn,true);
        addMacroSquare("",1,btn,false);
        addMacroSquare("",1,btn,true);
        addMacroSquare("",1,btn,false);
        updateView();
        m_canAdd = false;
        QTimer::singleShot(200,this,[=]{ m_canAdd=true; });
        return true;
    }

    if(event->type() == QEvent::MouseButtonPress)
    {
        m_loading = false;
        MacroSquare::LostFocus();
        m_lastDelay = m_tcount.elapsed();
        if(m_delay) m_delay->setDelay(m_lastDelay);
        m_tcount.restart();

        QMouseEvent *pME = static_cast<QMouseEvent *>(event);
        quint8 btn = pME->button();
        addMacroSquare("",1,btn,true);
    }

    if(event->type() == QEvent::MouseButtonRelease)
    {
        m_loading = false;

        m_lastDelay = m_tcount.elapsed();
        if(m_delay) m_delay->setDelay(m_lastDelay);
        m_tcount.restart();

        QMouseEvent *pME = static_cast<QMouseEvent *>(event);
        quint8 btn = pME->button();
        addMacroSquare("",1,btn,false);
    }

    return QFrame::event(event);
}

void FrameMacro::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat() && event->spontaneous())
    {
        m_lastDelay = m_tcount.elapsed();
        if(m_delay) m_delay->setDelay(m_lastDelay);
        m_tcount.restart();

        quint8 hid = getKeyHid(event->nativeScanCode());
        addMacroSquare(::getKeyValue(hid),0,hid,true);
    }
    QFrame::keyPressEvent(event);
}

void FrameMacro::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat() && event->spontaneous())
    {
        m_lastDelay = m_tcount.elapsed();
        if(m_delay) m_delay->setDelay(m_lastDelay);
        m_tcount.restart();

        quint8 hid = getKeyHid(event->nativeScanCode());
        addMacroSquare(::getKeyValue(hid),0,hid,false);
    }
    QFrame::keyReleaseEvent(event);
}

void FrameMacro::loadEvents()
{
    m_loading = true;

    if(m_prj)
    {
        int count = s_MSquares.count();
        for(int i=count-1; i>=0; i--)
        {
            MacroSquare *btn = s_MSquares[i];
            btn->hide();
        }

        s_MSquares.clear();

        count = m_prj->events.count();
        for(int i=0; i<count; i+=2)
        {
            MacroEvent *evt0 = m_prj->events[i+0];
            MacroEvent *evt1 = m_prj->events[i+1];
            addMacroSquare(evt0->text,evt0->type,evt0->value,evt0->down);
            m_delay->setDelay(evt1->value);
        }
    }

    m_loading = false;
    m_delay   = nullptr;

    updateView();
}

void FrameMacro::saveEvents()
{
    if(m_prj)
    {
        m_prj->events.clear();

        int count = s_MSquares.count();
        for(int i=0; i<count; i++)
        {
            MacroSquare *pSQ = s_MSquares[i];
            m_pMM->addMacroEvent(m_prj, pSQ->m_type, pSQ->m_value, pSQ->m_down, pSQ->m_text);
        }
        m_pMM->saveLoadEvent(m_prj);
    }

    m_loading = false;
}
