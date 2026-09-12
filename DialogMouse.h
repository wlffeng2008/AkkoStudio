#ifndef DIALOGMOUSE_H
#define DIALOGMOUSE_H

#include <QDialog>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QTableView>
#include <QHeaderView>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>

namespace Ui {
class DialogMouse;
}

class ImageDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    ImageDelegate(const QString&image,QObject *parent = nullptr) : QStyledItemDelegate(parent)
    {
        m_strImage = image;
        //installEventFilter(this);
    }

protected:
    QString m_strImage = ":/images/mouse/edit-0.png" ;

    QTableView *m_pView = nullptr ;

signals:
    void clicked(const QModelIndex &index);

protected:

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        //QStyledItemDelegate::paint(painter,option,index) ;
        painter->save() ;
        QRect rect = option.rect;
        rect.adjust(4,4,-4,-4);

        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawImage(rect,QImage(m_strImage));

        painter->restore() ;
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override
    {

        if(event->type() == QEvent::MouseButtonPress)
        {
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            auto mouseEv = static_cast<QMouseEvent*>(event);
            if (mouseEv->button() == Qt::LeftButton) // 只响应左键
            {
                emit clicked(index);
                //qDebug() << "QStyledItemDelegate" << index;
                QApplication::restoreOverrideCursor();
                QApplication::setOverrideCursor(Qt::ArrowCursor);
                return true;
            }
        }

        if(event->type() == QEvent::MouseMove)
        {
            QApplication::setOverrideCursor(Qt::PointingHandCursor);
        }
        else
        {
            QApplication::restoreOverrideCursor();
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        }
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    bool eventFilter(QObject *object, QEvent *event) override
    {
        qDebug() << event->type() ;
        if(event->type() == QEvent::MouseButtonPress)
        {
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            auto mouseEv = static_cast<QMouseEvent*>(event);
            if (mouseEv->button() == Qt::LeftButton) // 只响应左键
            {
                //emit clicked(index.row(),index.column());
                return true;
            }
        }
        else if(event->type() == QEvent::HoverEnter)
        {
            QApplication::setOverrideCursor(Qt::PointingHandCursor);
        }
        else if(event->type() == QEvent::HoverLeave)
        {
            QApplication::restoreOverrideCursor();
        }
        return QStyledItemDelegate::eventFilter(object,event);
    }
};



class DialogMouse : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMouse(QWidget *parent = nullptr);
    ~DialogMouse();

    void updateName(const QString&name);

protected:
    bool eventFilter(QObject *watched, QEvent *event) final;
    void changeEvent(QEvent *pEvt) final;
    void mousePressEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;


private:
    Ui::DialogMouse *ui;
    QPointF m_dragPosition;
    bool m_dragging = false;

    int m_nowAngle=0;

    QStandardItemModel *m_pModel = nullptr;
    QStandardItemModel *m_pMList = nullptr;

    ImageDelegate *m_MLDele1 = nullptr;
    ImageDelegate *m_MLDele2 = nullptr;
};

#endif // DIALOGMOUSE_H
