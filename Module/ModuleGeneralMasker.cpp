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
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //setModal(true);

    setStyleSheet("QDialog { background-color: rgba(180, 180, 180, 0.8);  border: none; border-radius: 24px;}");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    if(cotnent)
    {
        cotnent->setParent(this) ;
        mainLayout->addWidget(cotnent, 1, Qt::AlignCenter);

        cotnent->show();
        cotnent->installEventFilter(this) ;
        m_watch = cotnent;
    }

    QRect geoMetry = QApplication::primaryScreen()->geometry();
    if(parent)
    {
        geoMetry = parent->frameGeometry() ;//parent->mapToGlobal(parent->pos());
        QPoint globalPos = getGlobalPos(parent);
        geoMetry = QRect(globalPos.x(),globalPos.y(),geoMetry.width(),geoMetry.height()) ;
    }
    setGeometry(geoMetry);
    setFixedSize(geoMetry.width(), geoMetry.height());
}

bool ModuleGeneralMasker::eventFilter(QObject*watched,QEvent*event)
{
    if (event->type() == QEvent::Hide && watched == m_watch)
    {
        layout()->removeWidget(m_watch) ;
        m_watch->setParent(nullptr) ;
        m_watch->removeEventFilter(this) ;
        this->close() ;
    }

    return QDialog::eventFilter(watched,event);
}

void ModuleGeneralMasker::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QDialog::paintEvent(event);
}
