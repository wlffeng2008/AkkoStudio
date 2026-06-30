#include "FrameLight.h"
#include "ui_FrameLight.h"
#include "DialogDeviceConnect.h"


#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "MainWindow.h"

static QImage g_kbImg;

QImage decodeImage(const QString &imageData)
{
    QByteArray byteArray = QByteArray::fromBase64(imageData.toLatin1());
    QImage image;
    image.loadFromData(byteArray, "PNG");  // 从 Base64 编码数据加载图片
    return image;
}

QByteArray encodeImage( const QImage &image )
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return byteArray.toBase64();
}


FrameLight::FrameLight(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FrameLight)
{
    ui->setupUi(this);

    m_strWork = getUserDataPath() + "/work";
    QDir CD(m_strWork);
    if(!CD.exists()) CD.mkdir(m_strWork);

    ui->frameKeyboard->showFlag(false);

    m_picture = QByteArray(500,0);
    m_color.setGreen(220);

    DialogDeviceConnect *pCnn = DialogDeviceConnect::instance();

    connect(ui->frameLEDMode,&ModuleEfMode::onModeChanged,this,[=](int mode,quint8 opt){
        if(mode == -1)
        {
            pCnn->setLEDOn(opt);
            return;
        }
        pCnn->setLEDMode(mode,opt);
    });

    connect(ui->frameLEDBright,&ModuleEfLumi::onBrightChanged,this,[=](int bright){
        pCnn->setLEDBright(bright);
    });
    connect(ui->frameLEDSpeed,&ModuleEfSpeed::onSpeedChanged,this,[=](int speed){
        pCnn->setLEDSpeed(speed);
    });
    connect(ui->frameLEDColor,&ModuleEfColor::onSetColor,this,[=](const QColor&color, int option){
        pCnn->setLEDColor(color,option);
    });

    connect(pCnn,&DialogDeviceConnect::onReadBack,[=](const QByteArray&data){
        quint8 *pPack = (quint8 *)data.data();
        quint8 cmd = pPack[0];
        if(cmd == CMD_GET_LEDPARAM)
        {
            if(ui->frameLEDMode)
                ui->frameLEDMode->setEfMode(pPack[1]);
            if(ui->frameLEDSpeed)
                ui->frameLEDSpeed->setSpeed(4 - pPack[2]);
            if(ui->frameLEDSpeed)
                ui->frameLEDBright->setBright(pPack[3]);
        }
    });

    ui->frameKeyboard->setLightMode();
    ui->stackedWidgetB->setCurrentIndex(1);
    ui->frameKeyboard->setColor(0xFF,Qt::black);
    ui->labelColor->installEventFilter(this);
    srand(time(nullptr));

    connect(ui->pushButtonReset,&QPushButton::clicked,this,[=]{
        m_picture = QByteArray(500,0);
        ui->frameKeyboard->setColor(0xFF,Qt::black);
    });

    connect(pCnn,&DialogDeviceConnect::onGetPictrue,this,[=](const QByteArray&data){
        m_picture = data;
        QColor color;
        for(quint8 index = 0; index<128; index++)
        {
            quint8 hid = getHid(index);

            color.setRed(  (quint8)data[index*3 + 0]);
            color.setGreen((quint8)data[index*3 + 1]);
            color.setBlue( (quint8)data[index*3 + 2]);
            ui->frameKeyboard->setColor(hid,color);
        }
    });

    connect(ui->buttonGroupWrite,&QButtonGroup::idClicked,this,[=](int id){
        quint8 index = abs(id) - 2;
        pCnn->setPicture(index,m_picture);
    });

    connect(ui->buttonGroupRead,&QButtonGroup::idClicked,this,[=](int id){
        quint8 index = abs(id) - 2;
        pCnn->getPicture(index);
    });

    connect(ui->frameKeyboard,&ModuleKeyboard::onKeyClicked,this,[=](const QString&text,quint8 hid){
        QColor color = m_color;

        if(ui->checkBoxRam->isChecked())
        {
            color.setRed(  rand()%0x100);
            color.setGreen(rand()%0x100);
            color.setBlue( rand()%0x100);
        }

        quint8 index = getIndex(hid);

        m_picture[index*3 + 0] = color.red();
        m_picture[index*3 + 1] = color.green();
        m_picture[index*3 + 2] = color.blue();

        ui->frameKeyboard->setColor(hid,color);
    });

    m_pPick = new DialogColorPicker(this);
    connect(m_pPick,&DialogColorPicker::onPickupColor,this,[=](const QColor&color){
        m_color = color;
        QString strSheet=QString::asprintf("background-color: rgb(%d, %d, %d);border: none; border-radius:16px;",color.red(),color.green(),color.blue());
        ui->labelColor->setStyleSheet(strSheet);
    });

    g_kbImg = ui->frameKeyboard->grab().toImage();

    m_pModel = new QStandardItemModel(this);
    m_pModel->setHorizontalHeaderLabels(QString("预览图,作品名称,管理").split(','));
    ui->tableView->setModel(m_pModel);
    QHeaderView *pHeader = ui->tableView->horizontalHeader();
    pHeader->setSectionResizeMode(QHeaderView::Stretch);
    pHeader->setSectionResizeMode(0,QHeaderView::Fixed);
    pHeader->resizeSection(0,200);
    pHeader->setSectionResizeMode(2,QHeaderView::Fixed);
    pHeader->resizeSection(2,60);

    ui->tableView->setFocusPolicy(Qt::NoFocus);
    PictureDelegate *pDele0= new PictureDelegate(this);
    ui->tableView->setItemDelegateForColumn(0,pDele0);

    saveLoadWork(false);

    connect(ui->pushButtonSave,&QPushButton::clicked,this,[=]{
        g_kbImg = ui->frameKeyboard->grab().toImage();
        ui->tableView->update();

        addRecord(encodeImage(g_kbImg),ui->lineEditName->text().trimmed(),m_picture.toHex());

        saveLoadWork();
    });

    connect(ui->pushButtonUpdate,&QPushButton::clicked,this,[=]{
        g_kbImg = ui->frameKeyboard->grab().toImage();
        ui->tableView->update();

        if(m_nSelect != -1)
        {
            QStandardItem *item0 = m_pModel->item(m_nSelect);
            item0->setData(encodeImage(g_kbImg),Qt::UserRole+1);
            item0->setData(m_picture.toHex(), Qt::UserRole+2);

            QStandardItem *item1 = m_pModel->item(m_nSelect,1);
            item1->setText(ui->lineEditName->text().trimmed());

            saveLoadWork();
        }
    });

    ui->pushButtonUpdate->hide();
    connect(ui->tableView,&QTableView::clicked,this,[=](const QModelIndex &index){
        m_nSelect = index.row();
        if(index.column() == 2)
        {
            if(QMessageBox::question(this,"提示","确认要删除此作品吗？") == QMessageBox::Yes)
            {
                m_pModel->removeRow(m_nSelect);
                ui->pushButtonUpdate->hide();
                ui->lineEditName->setText(QString("我的作品%1").arg(m_pModel->rowCount()));
                m_nSelect = -1;
                saveLoadWork();
            }
        }
        else
        {
            ui->pushButtonUpdate->show();
            QStandardItem *item0 = m_pModel->item(index.row());
            ui->lineEditName->setText(m_pModel->item(index.row(),1)->text());
            QByteArray data = QByteArray::fromHex(item0->data(Qt::UserRole+2).toString().toLatin1());
            m_picture = data;
            QColor color;
            for(quint8 index = 0; index<128; index++)
            {
                quint8 hid = getHid(index);

                color.setRed(  (quint8)data[index*3 + 0]);
                color.setGreen((quint8)data[index*3 + 1]);
                color.setBlue( (quint8)data[index*3 + 2]);
                ui->frameKeyboard->setColor(hid,color);
            }
        }
    });

    connect(ui->tableView, &QTableView::entered, this, [=](const QModelIndex &index){
        if (index.column() == 2)
            ui->tableView->setCursor(Qt::PointingHandCursor);
        else
            ui->tableView->setCursor(Qt::ArrowCursor);
    });
}

FrameLight::~FrameLight()
{
    delete ui;
}

bool FrameLight::eventFilter(QObject *watched,QEvent *event)
{
    if(watched == ui->labelColor && event->type() == QEvent::MouseButtonRelease)
    {
        m_pPick->showPostion();
    }
    return QFrame::eventFilter(watched,event);
}

void FrameLight::addRecord(const QString&image, const QString&name, const QString &data)
{
    QStandardItem *item0 = new QStandardItem("");
    item0->setData(image,Qt::UserRole+1);
    item0->setData(data, Qt::UserRole+2);
    QStandardItem *item1 = new QStandardItem(name);
    QStandardItem *item2 = new QStandardItem(tr("删除"));
    item2->setTextAlignment(Qt::AlignCenter);
    m_pModel->appendRow({item0,item1,item2});
    ui->tableView->setRowHeight(m_pModel->rowCount()-1, 80);
}

void FrameLight::saveLoadWork(bool save)
{
    QString strFile = m_strWork + "/ledwork.json";
    QFile WkFile(strFile);
    if(save)
    {
        if(WkFile.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            int count = m_pModel->rowCount();
            QJsonArray jWork;
            for(int i=0; i<count; i++)
            {
                QStandardItem *item0 = m_pModel->item(i,0);
                QStandardItem *item1 = m_pModel->item(i,1);

                QString image = item0->data(Qt::UserRole+1).toString();
                QString data  = item0->data(Qt::UserRole+2).toString();
                QString name  = item1->text();

                QJsonObject jData;
                jData["image"] = image;
                jData["name"] = name;
                jData["data"] = data;
                jWork.push_back(jData);
            }
            QJsonDocument jDoc(jWork);
            WkFile.write(jDoc.toJson());
            WkFile.close();
        }
    }
    else
    {
        if(WkFile.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            QString strBuf = WkFile.readAll();
            WkFile.close();

            QJsonDocument jDoc = QJsonDocument::fromJson(strBuf.toUtf8());
            if(jDoc.isArray())
            {
                QJsonArray jWork = jDoc.array();
                int count = jWork.count();
                for(int i=0; i<count; i++)
                {
                    QJsonObject jData = jWork[i].toObject();

                    QString image = jData["image"].toString();
                    QString name = jData["name"].toString();
                    QString data = jData["data"].toString();

                    addRecord(image,name,data);
                }
            }
        }
    }

}