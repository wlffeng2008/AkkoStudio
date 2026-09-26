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


class DialogMouse : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMouse(QWidget *parent = nullptr);
    ~DialogMouse();

    void updateName(const QString&name);
    void updateImage(const QString&image);
    void startConnect();

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

private:
    Ui::DialogMouse *ui;
    QPointF m_dragPosition;
    bool m_dragging = false;

    int m_nowAngle=0;

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
    int m_currentId=-1;
    int m_insertAt = -1;
    macroItem*m_lastItem = nullptr;
    QList<macroItem*> macroGroup;
    void addMacroItem(int defId,int delay,int type,bool down=true);
    void addMacroHeed(int id,const QString&name);
    void saveLoadMacroHeader(bool save=true);
    void saveLoadMacroContent(int macroId,QList<macroItem*>&macroContent,bool save=true);

    void updateButtonInfo();

    // 打印设备信息
    void printDeviceInf(InfortechDef::DevInf *deviceInf);

    // 打印按键配置
    void printButtonInf(const InfortechDef::AllBtnCfg *btnInf);

    // 打印鼠标配置
    void printMouseCfg(const InfortechDef::MouseCfg* mouseCfg);

    // 打印额外配置
    void printMouseExtraInfo(const InfortechDef::ExtCfgInfo* extCfg);

    // 打印传感器配置
    void printDGAmbientCfg(const InfortechDef::DGAmbientCfg* dgAmbientCfg);

    // 打印传感器配置
    void printSensorCfg(const InfortechDef::SensorCfg* sensorCfg);

    // 打印电感轴配置
    void printAxisCfg(const InfortechDef::AxisCfg* axisCfg);

    // 获取所有配置
    void getAllCfg();

};

#endif // DIALOGMOUSE_H
