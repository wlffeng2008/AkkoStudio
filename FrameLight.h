#ifndef FRAMELIGHT_H
#define FRAMELIGHT_H
#include <DialogColorPicker.h>

#include <QFrame>

#include <QStandardItemModel>
#include <QPainter>
#include <QStyledItemDelegate>


extern QImage g_kbImg;


QImage decodeImage(const QString &imageData);

QByteArray encodeImage( const QImage &image);


class PictureDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    PictureDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {
        };

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyledItemDelegate::paint(painter,option,index) ;

        painter->save();

        QStandardItemModel *pModel = (QStandardItemModel *)index.model() ;
        QStandardItem *item = pModel->item(index.row(),index.column()) ;
        QString strText = item->text();

        QRect rect = option.rect ;
        rect.adjust(4,4,-4,-4) ;

        QPen pen(Qt::darkGray) ;
        painter->setPen(pen) ;
        QString strImg = item->data().toString();
        QImage img = decodeImage(strImg);
        painter->drawImage(rect,img);

        painter->restore() ;
    }
};


namespace Ui {
class FrameLight;
}

class FrameLight : public QFrame
{
    Q_OBJECT

public:
    explicit FrameLight(QWidget *parent = nullptr);
    ~FrameLight();

protected:
    bool eventFilter(QObject *watched,QEvent *event) override;

private:
    Ui::FrameLight *ui;

    QByteArray m_picture;
    QColor m_color;
    DialogColorPicker *m_pPick = nullptr;

    QStandardItemModel *m_pModel = nullptr;
    void saveLoadWork(bool save=true);
    int m_nSelect = -1;
    QString m_strWork;
    void addRecord(const QString&image,const QString&name,const QString&data);
};

#endif // FRAMELIGHT_H
