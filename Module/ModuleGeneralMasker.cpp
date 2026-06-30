#include "ModuleGeneralMasker.h"


#include <QVBoxLayout>
#include <QApplication>
#include <QStyleOption>
#include <QScreen>
#include <QPainter>
#include <QTimer>


static QPoint getGlobalPos(QWidget *widget) {
    QPoint pos = widget->pos();
    QWidget *w = widget->parentWidget();
    if(w && !(w->windowFlags() & Qt::Popup))
        pos = w->mapToGlobal(pos) ;
    // while (w)
    // {
    //     if()
    //         pos += w->pos();
    //     w = w->parentWidget();
    // }
    return pos;
}


ModuleGeneralMasker::ModuleGeneralMasker(QWidget *cotnent, QWidget *parent)
    : QDialog(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint |Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    //setStyleSheet("QDialog { background-color: rgba(180, 180, 180, 0.8);  border: none; border-radius: 24px;}");

    mainLayout = new QVBoxLayout(this);

    setContent(cotnent);

    QRect geoMetry = QApplication::primaryScreen()->geometry();
    if(parent)
    {
        geoMetry = parent->frameGeometry(); // parent->mapToGlobal(parent->pos());
        QPoint globalPos = getGlobalPos(parent);
        geoMetry = QRect(globalPos.x(),globalPos.y(),geoMetry.width(),geoMetry.height()).adjusted(1,1,-1,-1);
    }
    setGeometry(geoMetry);
    setFixedSize(geoMetry.size());
    this->raise();
}

void ModuleGeneralMasker::setContent(QWidget *cotnent)
{
    if(cotnent)
    {
        cotnent->setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
        cotnent->setParent(this);
        mainLayout->addWidget(cotnent, 0, Qt::AlignCenter);

        cotnent->show();
        cotnent->installEventFilter(this);
        m_watch = cotnent;
    }
}

bool ModuleGeneralMasker::eventFilter(QObject*watched,QEvent*event)
{
    if (event->type() == QEvent::Hide && watched == m_watch)
    {
        layout()->removeWidget(m_watch);
        m_watch->hide();
        m_watch->setParent(nullptr);
        m_watch->removeEventFilter(this);
        if(m_nFlag == 0) QDialog::reject();
        if(m_nFlag == 1) QDialog::accept();
    }

    return QDialog::eventFilter(watched,event);
}

void ModuleGeneralMasker::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    QDialog::paintEvent(event);
}

void ModuleGeneralMasker::mousePressEvent(QMouseEvent *event)
{
    emit onClicked();
    QDialog::mousePressEvent(event);
}
