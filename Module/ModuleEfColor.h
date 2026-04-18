#ifndef MODULEEFCOLOR_H
#define MODULEEFCOLOR_H

#include <QFrame>
#include <QStandardItemModel>

namespace Ui {
class ModuleEfColor;
}

#include <QEvent>
#include <QTableView>
#include <QPainter>
#include <QStyledItemDelegate>

class ColorDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    ColorDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {
    }
    void setImage(const QString&text){ m_strImage = text ; } ;
    void setTableView(QTableView *pView){ tableView = pView ;} ;

private:
    QString m_strImage ;
    QTableView *tableView = nullptr ;
    int m_nSelect = 0 ;

signals:
    void onClicked(int col);

protected:

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        //QStyledItemDelegate::paint(painter,option,index) ;

        painter->save() ;

        QStandardItemModel *pModel = (QStandardItemModel *)index.model() ;
        QStandardItem *item = pModel->item(index.row(),index.column()) ;
        QString strText = item->text() ;

        QRect rect = option.rect ;
        QFont font = item->data(Qt::FontRole).value<QFont>() ;
        if(3 == index.column())
        {
            static QImage img(m_strImage) ;
            painter->drawImage(rect,img);
            font.setPointSize(font.pointSize()*1.3) ;
        }
        painter->setFont(font) ;

        QBrush br = item->data(Qt::ForegroundRole).value<QBrush>();
        QPen pen(br.color()) ;
        painter->setPen(pen) ;
        painter->drawText(rect,Qt::AlignBottom|Qt::AlignHCenter,strText);

        painter->restore() ;
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override
    {
        if(event->type() == QEvent::MouseButtonRelease && tableView)
        {
            m_nSelect = index.column() ;
            tableView->update() ;
            emit onClicked(index.column()) ;
        }
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
};

class ModuleEfColor : public QFrame
{
    Q_OBJECT

public:
    explicit ModuleEfColor(QWidget *parent = nullptr);
    ~ModuleEfColor();

signals:
    void onSetColor(const QColor&color,int option=7);

protected:
    bool eventFilter(QObject* watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::ModuleEfColor *ui;
    QStandardItemModel *m_pModel = nullptr ;
};

#endif // MODULEEFCOLOR_H
