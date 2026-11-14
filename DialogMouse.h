#ifndef DIALOGMOUSE_H
#define DIALOGMOUSE_H

#include <QDialog>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QSpinBox>
#include <QPainter>
#include <QTableView>
#include <QHeaderView>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>
#include <QElapsedTimer>
#include <QJsonObject>


#include "infortechDevice.h"
#ifdef _DEBUG
    #pragma comment(lib, "infortechSdkd.lib")
#else
    #pragma comment(lib, "infortechSdk.lib")
#endif

namespace Ui {
class DialogMouse;
}

class ImageDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    ImageDelegate(const QString&image,QTableView *pView,int forColumn,QObject *parent = nullptr) : QStyledItemDelegate(parent)
    {
        m_strImage = image;
        _nForColumn = forColumn;
        pView->setItemDelegateForColumn(forColumn,this);
        setTableView(pView);
    }

    void setTableView(QTableView *pView)
    {
        m_pView = pView;
        m_pView->viewport()->setMouseTracking(true);
        m_pView->viewport()->installEventFilter(this);
    }

protected:
    QString m_strImage = ":/images/mouse/edit-0.png" ;
    bool  _setCursor = false;
    int _nForColumn = -1;
    QTableView *m_pView = nullptr;

signals:
    void clicked(const QModelIndex &index);

protected:

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        //QStyledItemDelegate::paint(painter,option,index) ;

        painter->save() ;
        QRect rect = option.rect;
        rect.adjust(6,6,-6,-6);

        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawImage(rect,QImage(m_strImage));

        painter->restore();
    }

    bool eventFilter(QObject *object, QEvent *event) override
    {
        QModelIndex index = m_pView->indexAt(m_pView->viewport()->mapFromGlobal(QCursor::pos()));
        if(event->type() == QEvent::MouseMove)
        {
            bool bToSet = false;
            //qDebug() << index.column() << _nForColumn;
            if(index.column() >= 1)
            {
                bToSet = true;
            }

            if(bToSet)
            {
                if(!_setCursor)
                {
                    _setCursor = true;
                    QApplication::setOverrideCursor(Qt::PointingHandCursor);
                }
            }
            else
            {
                _setCursor = false;
                QApplication::restoreOverrideCursor();
            }
        }
        else if(event->type() == QEvent::Leave || QEvent::WindowDeactivate == event->type())
        {
            _setCursor = false;
            QApplication::restoreOverrideCursor();
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            auto mouseEv = static_cast<QMouseEvent*>(event);
            if (mouseEv->button() == Qt::LeftButton && index.column() == _nForColumn) // 只响应左键
            {
                emit clicked(index);
                _setCursor = false;
                QApplication::restoreOverrideCursor();
                return true;
            }
        }
        return QStyledItemDelegate::eventFilter(object,event);
    }
};


typedef struct _macroItem
{
    int type  = 0; // keyboard mouse
    bool down  = true; // press release
    int defId = 0; //
    int delay = 0; // ms

    QJsonObject toObject(){
        QJsonObject jObj;
        jObj["type"] = type;
        jObj["down"] = down;
        jObj["defId"] = defId;
        jObj["delay"] = delay;
        return jObj;
    }
    void formObject(const QJsonObject&jObj){
        type = jObj["type"].toInt() ;
        down = jObj["down"].toBool() ;
        defId = jObj["defId"].toInt() ;
        delay = jObj["delay"].toInt() ;
    }

}macroItem;

void refreshMacroItem(QStandardItem *item);

class MarcoItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    MarcoItemDelegate(QTableView *pView,int forColumn,QObject *parent = nullptr) : QStyledItemDelegate(parent) {
        pView->setItemDelegateForColumn(forColumn,this);
        this->pView = pView;
    }

protected:
    QTableView *pView ;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        Q_UNUSED(option)
        Q_UNUSED(index)

        QStandardItemModel *pModel = (QStandardItemModel *)index.model();
        QStandardItem *item = pModel->item(index.row(),index.column());

        if(item->data().toULongLong() == 0) return nullptr;

        QSpinBox *pBox = new QSpinBox(parent);
        pBox->setAlignment(Qt::AlignCenter);
        pBox->setRange(1,5000);
        pBox->setStyleSheet("background-color:#2c2c2c;");
        return pBox;
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        //QStyledItemDelegate::paint(painter,option,index);

        painter->save();

        painter->setRenderHint(QPainter::Antialiasing);

        QStandardItemModel *pModel = (QStandardItemModel *)index.model();
        QStandardItem *item = pModel->item(index.row(),index.column());

        if(item && item->data().toULongLong() != 0)
        {
            QPen pen(Qt::gray);
            //if(!pView->selectionModel()->selectedIndexes().isEmpty() && pView->selectionModel()->selectedIndexes().at(0) == index)
            //    pen.setColor("#6329B6");

            if(option.state & QStyle::State_Selected)
                pen.setColor("#6329B6");

            QString strText = item->text();

            QRect rect = option.rect;
            rect.adjust(4,4,-4,-4);

            painter->setPen(pen);
            painter->drawRoundedRect(rect,6,6);

            painter->setFont(option.font);
            painter->drawText(rect,Qt::AlignCenter,strText);
        }

        painter->restore();
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override {
        QStandardItemModel *pModel = (QStandardItemModel *)index.model();
        macroItem *mItem =((macroItem*)pModel->itemFromIndex(index)->data().toULongLong());
        if(mItem)return;
        int value = mItem->delay;

        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(value);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override {
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();

        QStandardItemModel *pModel = (QStandardItemModel *)index.model();
        macroItem *mItem = ((macroItem*)pModel->itemFromIndex(index)->data().toULongLong());
        if(mItem)return;
        mItem->delay = value;
        refreshMacroItem(pModel->itemFromIndex(index));
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        //opt.rect.adjust(0,0,opt.rect.width()/2 * -1 + 20 ,0);
        QStyledItemDelegate::updateEditorGeometry(editor,opt,index);
    }
};


class DialogMouse : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMouse(QWidget *parent = nullptr);
    ~DialogMouse();

    void updateName(const QString&name);
    void updateImage(const QString&image);
    void startConnect();

    void changeName();

protected:
    bool eventFilter(QObject *watched, QEvent *event) final;
    void changeEvent(QEvent *pEvt) final;
    void mousePressEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;
    void mouseReleaseEvent(QMouseEvent *event) final;
    void hideEvent(QHideEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override;

signals:
    void genUpdate();
    void hideAction();
    void ConnectStateChange();
    void addNewItem(int defId,bool down=true);

private:
    Ui::DialogMouse *ui;
    QPointF m_dragPosition;
    bool m_dragging = false;
    QPixmap m_devImage;
    QString m_devName;

    int m_nowAngle= 0;
    int m_pressDf = 0;
    int m_battery = 0;
    bool m_charging=false;

    QStandardItemModel *m_pModel = nullptr;
    QStandardItemModel *m_pMList = nullptr;

    ImageDelegate *m_MLDele1 = nullptr;
    ImageDelegate *m_MLDele2 = nullptr;

    QString m_strPath;
    bool m_loading=false;
    bool m_recording=false;
    int m_currentId= -1;
    int m_insertAt = -1;
    int m_select = -1;
    macroItem*m_lastItem = nullptr;
    QList<macroItem*> macroGroup;
    void addMacroItem(int defId,int delay,int type,bool down=true);
    void addMacroHeader(int id,const QString&name);
    void saveLoadMacroHeader(bool save=true);
    void saveLoadMacroContent(int macroId,QList<macroItem*>&macroContent,bool save=true);

    void updateButtonInfo();

    void printDeviceInf(InfortechDef::DevInf *deviceInf);

    void printButtonInf(const InfortechDef::AllBtnCfg *btnInf);

     void printMouseCfg(const InfortechDef::MouseCfg* mouseCfg);

    void printMouseExtraInfo(const InfortechDef::ExtCfgInfo* extCfg);

    void printDGAmbientCfg(const InfortechDef::DGAmbientCfg* dgAmbientCfg);

    void printSensorCfg(const InfortechDef::SensorCfg* sensorCfg);

    void printAxisCfg(const InfortechDef::AxisCfg* axisCfg);

    void getAllCfg();

};

#endif // DIALOGMOUSE_H
