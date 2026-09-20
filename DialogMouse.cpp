#include "DialogMouse.h"
#include "qforeach.h"
#include "ui_DialogMouse.h"
#include "colorpicker.h"
#include "MyEasyApp.h"
#include "MainWindow.h"

#include <QLineEdit>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QTimer>
#include <QModelIndex>
#include <QFile>
#include <QJsonArray>
#include <QMessageBox>


void SetLineEditTipColor(QLineEdit *edit,const QColor&color){
    QImage image(edit->size(),QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect(0,0,image.rect().height(),image.rect().height());
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(rect.adjusted(2,2,-2,-2),8,8);

    QString strFile = QApplication::applicationDirPath() + "/images/" + edit->objectName() + ".png";
    image.save(strFile);

    QString strSheet = QString(R"(
    QLineEdit {
        padding-left: %1px;
        padding-top:4px;
        padding-bottom:4px;
        border:1px solid transparent;
        border-radius:4px;
        background-image: url(%2);
        background-repeat: no-repeat;
        background-color: transparent;
        color: white;
        font-size:14px;
    }

    QLineEdit:focus { border:1px solid #2196F3; }

)").arg(rect.height()).arg(strFile);

    edit->setStyleSheet(strSheet);
}

void SetButtonTipColor(QPushButton *button,const QColor&color){
    QImage image(button->size(),QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect(0,0,image.rect().height(),image.rect().height());
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundedRect(rect.adjusted(4,4,-6,-6),8,8);

    QString strFile = QApplication::applicationDirPath() + "/images/" + button->objectName() + ".png";
    image.save(strFile);

    QString strSheet = QString(R"(
    QPushButton {
        padding-left: %1px;

        border:1px solid transparent;
        border-radius:8px;
        background-image: url(%2);
        background-repeat: no-repeat;
        background-color: transparent;
        color: white;
        min-height:32px;
        font-size:16px;
        font-weight:500;
        text-align:left;
    }

    QPushButton:checked { border:1px solid %3; }

)").arg(rect.height() + 8).arg(strFile).arg(color.name());

    button->setStyleSheet(strSheet);
}

void SetButtonBackground(QPushButton *button,bool left=true,const QString&newText="",const QColor&colorChecked=QColor("#6329B6"),const QColor&colorBG=QColor("gray")){
    QImage image(button->size(),QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(button->font());
    QRect rect(0,0,image.rect().height(),image.rect().height());

    QString text = newText;
    if(text.isEmpty())
        text = button->text();
    QRect destRc0 = rect.adjusted(4,4,-4,-4);
    QRect textRc = image.rect().adjusted(rect.height()+10,1,-2,-2);

    if(!left)
    {
        int w = destRc0.width();
        int p = image.width() - w - 2;
        destRc0.setLeft(p);
        destRc0.setWidth(w);

        textRc = image.rect().adjusted(3,1,-rect.height()-10,-2);
    }
    QRect destRc1 = destRc0.adjusted(4,4,-4,-4);
    QString strFile0 = QApplication::applicationDirPath() + QString("/images/back0-") + button->objectName() + QString(".png");
    QString strFile1 = QApplication::applicationDirPath() + QString("/images/back1-") + button->objectName() + QString(".png");

    painter.setPen(Qt::NoPen);
    painter.setBrush(colorChecked);
    painter.drawRoundedRect(destRc1,destRc1.width()/2,destRc1.width()/2);
    painter.setPen(QPen(colorChecked,3));
    painter.setBrush(Qt::NoBrush);
    painter.drawArc(destRc0,0,360*16);

    painter.setPen(Qt::white);
    painter.setBrush(colorChecked);
    painter.drawRoundedRect(textRc,12,12);
    painter.drawText(textRc,text,QTextOption(Qt::AlignCenter));
    if(!image.save(strFile1))
    {
        qDebug()<< "Save File Failed: " << strFile1;
    }

    image.fill(Qt::transparent);
    painter.setPen(Qt::NoPen);
    painter.setBrush(colorBG);
    painter.drawRoundedRect(destRc1,destRc1.width()/2,destRc1.width()/2);
    painter.setPen(QPen(colorBG,3));
    painter.setBrush(Qt::NoBrush);
    painter.drawArc(destRc0,0,360*16);

    painter.setPen(Qt::white);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(textRc,12,12);
    painter.drawText(textRc,text,QTextOption(Qt::AlignCenter));

    if(!image.save(strFile0))
    {
        qDebug()<< "Save File Failed: " << strFile0;
    }

    QString strSheet = QString(R"(
    QPushButton {
        border:1px solid transparent;
        background-color: transparent;
        background-image: url(%1);
        background-repeat: no-repeat;
        min-height:32px;
        min-width:120px;
        color:transparent;
    }

    QPushButton:hover { background-image: url(%2); }
    QPushButton:checked { background-image: url(%3); }

)").arg(strFile0).arg(strFile1).arg(strFile1);

    button->setStyleSheet(strSheet);
}

// SDK 单例
static InfortechDevice* serviceDevice = nullptr;
static bool mouseChanged = false;

// 回调函数
auto deviceStateChange(const InfortechDef::DevMsg& msg)
{
    mouseChanged = true;

    switch (msg.type)
    {
    case InfortechDef::MsgType::DeviceDisconnect:
        std::cout << "DeviceDisconnect" << std::endl;
        break;

    // 鼠标配对
    case InfortechDef::MsgType::PairFailed:
        std::cout << "PairFailed" << std::endl;
        break;

    case InfortechDef::MsgType::PairTimeout:
        std::cout << "PairTimeout" << std::endl;
        break;

    case InfortechDef::MsgType::PairSucceed:
        std::cout << "PairSucceed" << std::endl;
        break;

    // 固件升级
    case InfortechDef::MsgType::fwUpgradeFailed:
        std::cout << "fwUpgradeFailed" << std::endl;
        break;

    case InfortechDef::MsgType::fwUpgradeProgress:
        std::cout << "fwUpgradeProgress: " << (int)msg.inf->getFwUpProgress()->progress << std::endl;
        break;

    case InfortechDef::MsgType::fwUpgradeSucceed:
        std::cout << "fwUpgradeSucceed" << std::endl;
        break;

    // 设备状态上报
    case InfortechDef::MsgType::DpiChange:
        std::cout << "DpiChange" << std::endl;
        break;

    case InfortechDef::MsgType::RateChange:
        std::cout << "RateChange" << std::endl;
        break;

    case InfortechDef::MsgType::ConfChange:
        std::cout << "ConfChange" << std::endl;
        break;

    case InfortechDef::MsgType::BatteryChange:
        std::cout << "BatteryChange" << std::endl;
        break;

    case InfortechDef::MsgType::ConnectStateChange:
        std::cout << "ConnectStateChange" << std::endl;
        break;

    case InfortechDef::MsgType::TriggerPosChange:
        std::cout << "TriggerPosChange" << std::endl;
        break;

    case InfortechDef::MsgType::CalibrateChange:
        std::cout << "CalibrateChange" << std::endl;
        break;

    default:
        break;
    }
};

QMap<int,QString> keyMapset=
{
    {1707,"左滚"},
    {1702,"右键"},
    {1710,"下滚"},
    {1705,"前进"},
    {1701,"左键"},
    {1709,"上滚"},
    {1704,"后退"},
    {1711,"DPI循环"},
    {1712,"DPI+"},
    {1708,"右滚"},
    {1703,"中键"},
    {1713,"DPI-"},
    {1731,"BLE循环"},
    {1732,"BLE+"},
    {1733,"BLE-"},
    {1706,"底部按钮"},
    {1901,"火力按钮"},
    {1801,"亮度+"},
    {1802,"亮度-"},
    {1809,"播放器"},
    {1810,"停止"},
    {1811,"暂停"},
    {1812,"上一首"},
    {1813,"下一首"},
    {1805,"静音"},
    {1803,"音量+"},
    {1804,"音量-"},
    {1808,"邮件"},
    {1814,"主页"},
    {1815,"搜索"},
    {1816,"刷新"},
    {1817,"收藏夹"},
    {1818,"网页停止"},
    {1819,"网页前进"},
    {1820,"网页后退"},
    {1806,"计算器"},
    {1807,"我的电脑"},
    {1201,"F1"},
    {1202,"F2"},
    {1203,"F3"},
    {1204,"F4"},
    {1205,"F5"},
    {1206,"F6"},
    {1207,"F7"},
    {1208,"F8"},
    {1209,"F9"},
    {1210,"F10"},
    {1211,"F11"},
    {1212,"F12"},
    {1101,"!1"},
    {1102,"@2"},
    {1103,"#3"},
    {1104,"$4"},
    {1105,"%5"},
    {1106,"^6"},
    {1107,"&&7"},
    {1108,"*8"},
    {1109,"(9"},
    {1110,")0"},
    {1401,"~`"},
    {1402,"-_"},
    {1403,"+="},
    {1404,"{["},
    {1405,"}]"},
    {1406,"|\\"},
    {1407,":;"},
    {1408,"\"'"},
    {1409,"<,"},
    {1410,">."},
    {1411,"?/"},
    {1301,"1"},
    {1302,"2"},
    {1303,"3"},
    {1304,"4"},
    {1305,"5"},
    {1306,"6"},
    {1307,"7"},
    {1308,"8"},
    {1309,"9"},
    {1310,"0"},
    {1311,"."},
    {1312,"Number Lock"},
    {1313,"/"},
    {1314,"*"},
    {1315,"-"},
    {1316,"+"},
    {1317,"Return"},
    {1001,"A"},
    {1002,"B"},
    {1003,"C"},
    {1004,"D"},
    {1005,"E"},
    {1006,"F"},
    {1007,"G"},
    {1008,"H"},
    {1009,"I"},
    {1010,"J"},
    {1011,"K"},
    {1012,"L"},
    {1013,"M"},
    {1014,"N"},
    {1015,"O"},
    {1016,"P"},
    {1017,"Q"},
    {1018,"R"},
    {1019,"S"},
    {1020,"T"},
    {1021,"U"},
    {1022,"V"},
    {1023,"W"},
    {1024,"X"},
    {1025,"Y"},
    {1026,"Z"},
    {1505,"Space"},
    {1502,"Esc"},
    {1504,"Tab"},
    {1503,"Back Space"},
    {1501,"Enter"},
    {1519,"Left Win"},
    {1523,"Right Win"},
    {1516,"Left Ctrl"},
    {1520,"Right Ctrl"},
    {1518,"Left Alt"},
    {1522,"Right Alt"},
    {1517,"Left Shift"},
    {1521,"Right Shift"},
    {1601,"Up"},
    {1603,"Left"},
    {1602,"Down"},
    {1604,"Right"},
    {1507,"Print Screen"},
    {1508,"Scroll Lock"},
    {1509,"Pause"},
    {1510,"Insert"},
    {1511,"Home"},
    {1514,"End"},
    {1512,"Page Up"},
    {1515,"Page Down"},
    {1513,"Delete"},
    {1506,"Caps Lock"},
};

QString getKeyNameByVK(uint16_t vkCode);
uint16_t getKeyDefineVK(uint16_t vk_code);

QString getKeyNameByDF(uint16_t dfCode)
{
    return keyMapset[dfCode];
}

const char *get_keyname_df(uint16_t df_code);


QMap<int,QString> keyChangeset=
{
    {1701,""},
    {1702,""},
    {1703,""},
    {1704,""},
    {1705,""},
    {1706,""}
};

void refreshMacroItem(QStandardItem *item)
{
    if(!item) return;
    macroItem *macro = reinterpret_cast<macroItem *>(item->data().toULongLong());
    QString text=QString("%1: %2\n延迟：%3ms").arg(keyMapset[macro->defId], macro->down?"按下":"松开").arg(macro->delay);
    item->setText(text);
}

void DialogMouse::addMacroItem(int defId,int delay,int type,bool down)
{
    int count = macroGroup.size();
    if(count >= 70) return;
    if(!m_recording) return;

    static bool lastFlag = 0 ;
    static int lastDefId= 0 ;
    if(down == lastFlag && lastDefId == defId)
        return;

    lastFlag = down;
    lastDefId= defId;

    static QStandardItem *lastItem = nullptr;
    if(m_lastItem)
    {
        m_lastItem->delay = delay;
        refreshMacroItem(lastItem);
    }

    m_lastItem = new macroItem();
    m_lastItem->defId = defId;
    m_lastItem->down = down;
    m_lastItem->type = type;

    int col = count%6;
    int row = count/6;

    QStandardItem *item = m_pModel->item(row,col);
    lastItem = item;

    if(item)
    {
        item->setData((quint64)m_lastItem);
        refreshMacroItem(item);
        macroGroup.push_back(m_lastItem);
    }

    ui->labelTitle4->setText(QString("%1 / 70").arg(macroGroup.count()));
}

void DialogMouse::addMacroHeed(int id,const QString&name)
{
    QList<QStandardItem*>test;
    for(int i=0; i<3; i++)
    {
        QStandardItem *item = new QStandardItem("");
        test.push_back(item);
    }
    int addId = id;
    if(id==0) addId = m_pMList->rowCount()+1;
    QString addName=QString("新宏%1").arg(addId);
    if(!name.isEmpty())addName=name;

    QFont font = this->font();
    test[0]->setText(addName);
    test[0]->setData(addId);
    test[0]->setFont(font);

    ui->comboBoxMacro->addItem(addName);
    int count = ui->comboBoxMacro->count();
    ui->comboBoxMacro->setItemData(count-1,addId);

    test[1]->setEditable(false);
    test[2]->setEditable(false);

    test[1]->setSelectable(false);
    test[2]->setSelectable(false);
    m_pMList->appendRow(test);
    ui->tableViewMList->setRowHeight(m_pMList->rowCount()-1,36);
}

void DialogMouse::saveLoadMacroHeader(bool save)
{
    m_loading = true;
    QFile jF(m_strPath+"/macroheader.txt");
    if(save)
    {
        if (jF.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            QJsonObject Head;
            Head["version"]="20260919";
            Head["count"]=m_pMList->rowCount();

            QJsonArray Macro;
            for(int i=0; i<m_pMList->rowCount(); i++)
            {
                QStandardItem *item = m_pMList->item(i,0);
                QJsonObject It;
                It["id"]=item->data().toInt();
                It["name"]=item->text();
                Macro.push_back(It);
            }
            Head["macro"]=Macro;

            QJsonDocument jDoc(Head);
            QByteArray data = jDoc.toJson();
            jF.write(data);
            jF.close();
        }
    }
    else
    {
        if (jF.open(QIODevice::ReadOnly))
        {
            QByteArray data = jF.readAll();
            jF.close();

            QJsonDocument jDoc = QJsonDocument::fromJson(data);
            QJsonObject Head = jDoc.object() ;
            QJsonArray Macro = Head["macro"].toArray();
            int count = Macro.count();
            QList<int> checker;
            for(int i=0; i<count; i++)
            {
                QJsonObject It = Macro[i].toObject();
                if(checker.contains(It["id"].toInt()))
                    continue;
                addMacroHeed(It["id"].toInt(),It["name"].toString());

                checker.push_back(It["id"].toInt());
            }

        }
    }
    m_loading = false;
}

void DialogMouse::saveLoadMacroContent(int macroId, QList<macroItem*>&macroContent, bool save)
{
    m_loading = true;
    QFile jF(m_strPath+QString::asprintf("/macroEvent-%02d.txt",macroId));
    if(save)
    {
        if (jF.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            QJsonObject jMEvt;
            jMEvt["version"]="20260919";
            jMEvt["count"]=macroContent.count();

            QJsonArray Macro;
            for(int i=0; i<macroContent.count(); i++)
            {
                macroItem *item = macroContent[i];
                QJsonObject It = item->toObject();
                Macro.push_back(It);
            }
            jMEvt["macro"]=Macro;

            QJsonDocument jDoc(jMEvt);
            QByteArray data = jDoc.toJson();
            jF.write(data);
            jF.close();
        }
    }
    else
    {
        macroContent.clear();
        if (jF.open(QIODevice::ReadOnly))
        {
            QByteArray data = jF.readAll();
            jF.close();

            QJsonDocument jDoc = QJsonDocument::fromJson(data);
            QJsonObject jMEvt = jDoc.object() ;
            QJsonArray Macro = jMEvt["macro"].toArray();
            int count = Macro.count();

            for(int i=0; i<count; i++)
            {
                QJsonObject It = Macro[i].toObject();
                macroItem *item=new macroItem();
                item->formObject(It);
                macroContent.push_back(item);
            }
        }
    }
    m_loading = false;
}

void DialogMouse::startConnect()
{
    serviceDevice = InfortechDevice::getInstance();
    serviceDevice->openLog(false);

    // 设置设备信息
    //serviceDevice->addMouseInf(0x1A86, 0x8501);
    //serviceDevice->addDongleInf(0x1A86, 0xD810);
    //serviceDevice->addMouseBootInf(0x1A86, 0x85F1);
    //serviceDevice->addDongleBootInf(0x1A86, 0xD8E0);

    std::vector<InfortechDef::DeviceItem> allDevices = {
        { 0x38EE, 0x0021, 0, 0, 0 },
        { 0x38EE, 0x0022, 0, 0, 0 },
        { 0x38EE, 0x0047, 0, 0, 0 },
        { 0x38EE, 0x0048, 0, 0, 0 }
    };
    serviceDevice->setSupportDeviceSet(allDevices);

    // 注册通知函数
    serviceDevice->regDevChangeCall(deviceStateChange);

    auto deviceSet = serviceDevice->getDevices();
    if(deviceSet.size())
    {
        serviceDevice->connectDevice(deviceSet[0]);
        serviceDevice->initDevice();

        getAllCfg();
    }

    QTimer *pMonitorTM = new QTimer(this);
    pMonitorTM->start(100);
    connect(pMonitorTM,&QTimer::timeout,this,[=]{
        if(mouseChanged)
        {
            mouseChanged=false;
            getAllCfg();
        }
    });
}

DialogMouse::DialogMouse(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogMouse)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);

    m_strPath = getUserDataPath("huaximouse");

    {
        startConnect();
    }

    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->buttonGroupMain,&QButtonGroup::idClicked,this,[=](int id){
        ui->labelSubTitle->setText(ui->buttonGroupMain->button(id)->text());
        ui->stackedWidget->setCurrentIndex(abs(id)-2);
    });

    connect(ui->pushButtonClose,&QPushButton::clicked,this,[=]{ this->hide(); });
    connect(ui->pushButtonBack,&QPushButton::clicked,this,[=]{
        if(ui->frameKeyShow->isVisible())
        {
            ui->frameKeyShow->hide();
            return;
        }
        this->hide();
    });

    {
        MyEasyApp *app = dynamic_cast<MyEasyApp *>(qApp);
        static QElapsedTimer ignTm;
        ignTm.start();
        connect(app,&MyEasyApp::transEvent,this,[=](QObject *receiver,QEvent *event)
        {
            int nDelay = ignTm.elapsed();
            int nLimit = 5;

            QKeyEvent   *kEvent = static_cast<QKeyEvent*>(event);
            QMouseEvent *mEvent = static_cast<QMouseEvent*>(event);
            QWheelEvent *wEvent = static_cast<QWheelEvent*>(event);
            QWidget *widget = static_cast<QWidget*>(receiver);

            if(event->type() == QEvent::MouseButtonPress)
            {
                if(ui->pushButtonRecord == widget) return;
                if(nDelay>nLimit)
                {
                    qDebug() << "Global::MouseButtonPress     ------ " ;
                    int btn = mEvent->button();
                    int defId = 1701;
                    if(btn == Qt::MouseButton::RightButton)  defId = 1702;
                    if(btn == Qt::MouseButton::MiddleButton) defId = 1703;

                    addMacroItem(defId,nDelay,0);
                }
                ignTm.restart();
            }

            if(event->type() == QEvent::MouseButtonRelease)
            {
                if(ui->pushButtonRecord == widget) return;
                if(nDelay>nLimit)
                {
                    qDebug() << "Global::MouseButtonRelease   +++++++ " << nDelay <<"ms";
                    int btn = mEvent->button();
                    int defId = 1701;
                    if(btn == Qt::MouseButton::RightButton) defId = 1702;
                    if(btn == Qt::MouseButton::MiddleButton) defId = 1703;

                    addMacroItem(defId,nDelay,0,false);
                }
                ignTm.restart();
            }

            if(event->type() == QEvent::MouseMove)
            {
                //if(nDelay>nLimit)
                //    qDebug() << "Global::MouseMove   ------ " << nDelay <<"ms";
                //ignTm.restart();
            }

            if(event->type() == QEvent::Wheel)
            {
                //if(nDelay>nLimit)
                //    qDebug() << "Global::Wheel   ------ " << nDelay <<"ms" << wEvent->angleDelta();
                //ignTm.restart();
            }

            if(event->type() == QEvent::KeyPress)
            {
                if(kEvent->isAutoRepeat())
                    return;
                int defId = getKeyDefineVK(kEvent->nativeVirtualKey());
                if(defId == 0) defId = getKeyDefineVK(kEvent->key());
                m_pressDf = defId;
                if(nDelay>nLimit)
                {
                    qDebug() << "Global::KeyPress   ------ " << kEvent->key() << kEvent->nativeVirtualKey() << kEvent->nativeScanCode() << defId << keyMapset[defId];
                    addMacroItem(defId,nDelay,1);
                }

                ignTm.restart();
            }

            if(event->type() == QEvent::KeyRelease)
            {
                if(kEvent->isAutoRepeat())
                    return;
                int defId = getKeyDefineVK(kEvent->nativeVirtualKey());
                if(defId == 0) defId = getKeyDefineVK(kEvent->key());
                if(nDelay>nLimit)
                {
                    qDebug() << "Global::KeyRelease +++++++ " << kEvent->key() << kEvent->nativeVirtualKey() << kEvent->nativeScanCode() <<  defId << keyMapset[defId];
                    addMacroItem(defId,nDelay,1,false);
                }
                ignTm.restart();
            }

            if(event->type() == QEvent::Enter)
            {
                // if(nDelay>nLimit)
                //     qDebug() << "Globel QEvent::Enter:" << widget->objectName();
                // ignTm.restart();
            }

            if(event->type() == QEvent::Leave)
            {
                // if(nDelay>nLimit)
                //     qDebug() << "Globel QEvent::Leave:" << widget->objectName();
                // ignTm.restart();
            }

        });
    }

    {
        ui->pushButtonMkey1706->hide();
        //ui->frameColorPack->hide();
        //ui->widgetColor->hide();

        ui->frameKeyShow->hide();
        ui->frameMouseHold->setFixedWidth(720);

        static int seletetKey = 0 ;
        connect(ui->buttonGroupKeySet,&QButtonGroup::idClicked,this,[=](int id){
            ui->frameKeyShow->show();

            QPushButton *pBtn = static_cast<QPushButton *>(ui->buttonGroupKeySet->button(id));
            seletetKey = pBtn->objectName().replace("pushButtonMkey","").toInt();
            //qDebug() << seletetKey << id;
        });

        ui->stackedWidgetPickKey->setCurrentIndex(0);
        connect(ui->buttonGroupKeyType,&QButtonGroup::idClicked,this,[=](int id){
            int index = abs(id)-2;
            ui->stackedWidgetPickKey->setCurrentIndex(index);
            QPushButton *pBtn = static_cast<QPushButton *>(ui->buttonGroupKeyType->button(id));
        });

        connect(ui->buttonGroupConfig,&QButtonGroup::idClicked,this,[=](int id){
            int index = abs(id)-2;
            serviceDevice->switchProfile(index);
            getAllCfg();
        });

        QTimer::singleShot(500,this,[=]{
            ui->radioButtonConfig0->click();
        });

        ui->buttonGroupShortcut->setExclusive(false);
        connect(ui->buttonGroupShortcut,&QButtonGroup::idClicked,this,[=](int id){
            QList<QAbstractButton*>btns = ui->buttonGroupShortcut->buttons();
            int count = 0;
            foreach(auto btn,btns)
            {
                if(btn->isChecked())
                    count++;
            }

            if(count==2)
            {
                foreach(auto btn,btns)
                {
                    if(!btn->isChecked())
                        btn->setEnabled(false);
                }
            }
            else
            {

                foreach(auto btn,btns)
                {
                    btn->setEnabled(true);
                }
            }
        });

        connect(ui->pushButtonSetCombine,&QPushButton::clicked,this,[=]{
            if(m_pressDf == 0) return;

            InfortechDef::ComboKeyInf shortcutKeyData;
            if(ui->checkBox_Ctrl->isChecked())  shortcutKeyData.sysKey.push_back(1516);
            if(ui->checkBox_Shift->isChecked()) shortcutKeyData.sysKey.push_back(1517);
            if(ui->checkBox_Alt->isChecked())   shortcutKeyData.sysKey.push_back(1518);
            if(ui->checkBox_Win->isChecked())   shortcutKeyData.sysKey.push_back(1519);
            if(shortcutKeyData.sysKey.size()==0)
                return;

            shortcutKeyData.customKey = m_pressDf;

            serviceDevice->setShortcutKeyInf(seletetKey,shortcutKeyData);
            m_pressDf = 0;
            ui->lineEditShortcut->clear();
            updateButtonInfo();
        });

        connect(ui->pushButtonSetFireKey,&QPushButton::clicked,this,[=]{
            InfortechDef::FireKeyInf fireKeyData;
            fireKeyData.clickNum = ui->spinBoxFireCount->value();
            fireKeyData.interval = ui->spinBoxFireInterval->value();
            fireKeyData.keepClick= ui->checkBoxKeepClick->isChecked();
            serviceDevice->setFireKeyInf(seletetKey,fireKeyData);
            updateButtonInfo();
        });

        connect(ui->pushButtonSetMacro,&QPushButton::clicked,this,[=]{
            int keyId = seletetKey;
            int index = ui->comboBoxMacro->currentIndex();
            if(index == -1) return;

            QList<macroItem*>mEvts;
            saveLoadMacroContent(ui->comboBoxMacro->itemData(index).toInt(),mEvts,false);
            if(mEvts.size() == 0) return;

            InfortechDef::MacroKeyInf macroKeyData = {};

            int loopType = 0;
            if(ui->radioButtonMLoop1->isChecked()) loopType = 1;
            if(ui->radioButtonMLoop2->isChecked()) loopType = 2;
            if(ui->radioButtonMLoop3->isChecked()) loopType = 3;
            if(ui->radioButtonMLoop4->isChecked()) loopType = 4;
            if(ui->radioButtonMLoop5->isChecked()) loopType = 5;

            macroKeyData.macroId = 2;						// 宏 ID
            macroKeyData.macroType = (InfortechDef::MacroType)loopType;
            macroKeyData.cycleNumber = ui->spinBoxMacroLoop->value();				// 宏循环次数

            // 最大支持 70 组数据
            // macroKeyData.data.push_back({ InfortechDef::Action::Press, 1101, 78 });
            // macroKeyData.data.push_back({ InfortechDef::Action::Release, 1101, 290 });
            // macroKeyData.data.push_back({ InfortechDef::Action::Press, 1001, 131 });
            // macroKeyData.data.push_back({ InfortechDef::Action::Release, 1001, 26 });
            // macroKeyData.data.push_back({ InfortechDef::Action::Press, 1101, 148 });
            // macroKeyData.data.push_back({ InfortechDef::Action::Release, 1516, 377 });
            // macroKeyData.data.push_back({ InfortechDef::Action::Press, 1516, 128 });
            // macroKeyData.data.push_back({ InfortechDef::Action::Release, 1701, 271 });
            // macroKeyData.data.push_back({ InfortechDef::Action::Press, 1701, 78 });
            // macroKeyData.data.push_back({ InfortechDef::Action::Release, 1002, 79 });
            // macroKeyData.data.push_back({ InfortechDef::Action::Release, 1002, 104 });

            for(int i=0; i<mEvts.size(); i++)
            {
                macroItem*item = mEvts[i];
                macroKeyData.data.push_back({ item->down?InfortechDef::Action::Press:InfortechDef::Action::Release, (uint16_t)item->defId, (uint16_t)item->delay });
            }

            serviceDevice->setMacroKeyInf(keyId, macroKeyData);
            updateButtonInfo();
        });

        connect(ui->pushButtonGotoMacro,&QPushButton::clicked,this,[=]{
            ui->frameKeyShow->hide();
            ui->pushButtonMain4->click();
        });

        connect(ui->pushButtonMKeyReset,&QPushButton::clicked,this,[=]{
            int nKeyId = seletetKey ;
            serviceDevice->setBaseKeyInf(nKeyId,nKeyId);
            updateButtonInfo();
        });

        connect(ui->buttonGroupSuper,&QButtonGroup::idClicked,this,[=](int id){
            Q_UNUSED(id)
            QPushButton *pBtn = static_cast<QPushButton *>(ui->buttonGroupSuper->button(id));

            int nKeyId = seletetKey ;
            int nDestID = pBtn->objectName().replace("pushButton_","").toInt();

            serviceDevice->setBaseKeyInf(nKeyId,nDestID);
            updateButtonInfo();
        });

        ui->frameConfig->hide();
        connect(ui->pushButtonMenu,&QPushButton::clicked,this,[=](bool checked){
            ui->frameConfig->setVisible(checked);
        });
    }

    {
        static QPushButton *btnsX[] = {
            ui->pushButtonX0,
            ui->pushButtonX1,
            ui->pushButtonX2,
            ui->pushButtonX3,
            ui->pushButtonX4,
            ui->pushButtonX5,
            ui->pushButtonX6,
            ui->pushButtonX7
        };

        static QPushButton *btnsY[] = {
            ui->pushButtonY0,
            ui->pushButtonY1,
            ui->pushButtonY2,
            ui->pushButtonY3,
            ui->pushButtonY4,
            ui->pushButtonY5,
            ui->pushButtonY6,
            ui->pushButtonY7
        };

        static QStringList dpiColors = {"#FF0000","#00FF00","#0000FF","#FF00FF","#00FFFF","#FFFF00","#FFFFFF","#FF69B4"};
        for(int i=0; i<8; i++)
        {
            SetButtonTipColor(btnsX[i],dpiColors[i]);
            SetButtonTipColor(btnsY[i],dpiColors[i]);
        }

        static int valueA[]={400,800,1200,1600,3200,6400,10000,30000};
        static int valueX[]={400,800,1200,1600,3200,6400,10000,30000};
        static int valueY[]={400,800,1200,1600,3200,6400,10000,30000};
        static int selectA = 0 ;
        static int selectX = 0 ;
        static int selectY = 0 ;
        static int selectDpiX = 0;
        static int selectDpiY = 0;

        connect(ui->horizontalSliderBrightness,&QSlider::valueChanged,this,[=](int value){
            serviceDevice->setRgbBrightness(value);
        });

        static QList<int> rgbSpeeds={1,2,3,4,5};

        connect(ui->buttonGroupRgbEffect,&QButtonGroup::idClicked,this,[=](int id){
            serviceDevice->setRgbEffect(abs(id)-2);
        });

        connect(ui->buttonGroupRgbSpeed,&QButtonGroup::idClicked,this,[=](int id){
            serviceDevice->setRgbSpeed(rgbSpeeds[abs(id)-2]);
        });

        connect(ui->spinBoxDpiColor,&QSpinBox::valueChanged,this,[=](int value){
            ui->widgetColor->colorChanged(dpiColors[value-1]);
        });

        {
            connect(ui->horizontalSliderBrightness,&QSlider::valueChanged,this,[=](int value){
                ui->spinBoxLumi->setValue(value);
            });

            connect(ui->spinBoxLumi,&QSpinBox::valueChanged,this,[=](int value){
                ui->horizontalSliderBrightness->setValue(value);
            });

            static QStringList preColors = {"#73F2FD","#E9381D","#71FB48","#FDFF4E","#EA36B0","#C6FD4B","#51A4F9","#D1307D","#6A8B28","#BC92BC"};
            QTimer::singleShot(500,this,[=]{
                QList<QAbstractButton*> btns = ui->buttonGroupClr->buttons();
                for(int i=0; i<10; i++)
                {
                    btns[i]->setStyleSheet(
                        QString(R"(
                        QPushButton{
                            border-radius:6px;
                            background-color: %1;
                            border: 2px solid transparent;
                            max-width:40px;
                            max-height:40px;
                        }
                        QPushButton:checked{border-color: white;}
                    )").arg(preColors[i]));

                    btns[i]->setFixedSize(40,40);
                }
            });

            static int preSelect = 0;
            static QPushButton *pColorBrn = nullptr; // ui->pushButtonC0;
            connect(ui->buttonGroupClr,&QButtonGroup::idClicked,this,[=](int id){
                pColorBrn = static_cast<QPushButton *>(ui->buttonGroupClr->button(id));
                preSelect = ui->buttonGroupClr->buttons().indexOf(ui->buttonGroupClr->button(id));
                qDebug() << preSelect << id;

                ui->widgetColor->colorChanged(preColors[preSelect]);
            });
            connect(ui->widgetColor,&ColorPicker::colorChanged,this,[=](const QColor&color){

                int dpiIndex = ui->spinBoxDpiColor->value()-1;
                dpiColors[dpiIndex] = color.name();
                preColors[preSelect] = color.name();

                ui->labelDpiColor->setStyleSheet(QString("QLabel{background-color:%1;border:none;border-radius:20px;}").arg(color.name()));
                ui->spinBoxDpiColor->setStyleSheet(QString("QSpinBox{background-color:%1;}").arg(color.name()));
                serviceDevice->setDpiColor(dpiIndex,color.name().toStdString().c_str());
                SetButtonTipColor(btnsX[selectDpiX],color.name());
                SetButtonTipColor(btnsY[selectDpiX],color.name());

                if(!pColorBrn) return;
                pColorBrn->setStyleSheet(QString(R"( QPushButton{ background-color: %1;} )").arg(color.name()));
                pColorBrn->setFixedSize(40,40);

            });
        }

        ui->horizontalSliderX->setClipStep(50);
        ui->horizontalSliderY->setClipStep(50);
        ui->lineEditValueX->setFixedHeight(32);
        ui->lineEditValueY->setFixedHeight(32);

        connect(this,&DialogMouse::genUpdate,this,[=]{
            auto mouseCfg = serviceDevice->getMouseCfg();

            int i = 0;
            for (const auto &item : mouseCfg->dpiData.data) {
                valueA[i] = item.xDpi;
                valueX[i] = item.xDpi;
                valueY[i] = item.yDpi;
                dpiColors[i] =  item.color.c_str();
                i++;
            }

            for(int i=0; i<8; i++)
            {
                btnsX[i]->setText(QString("%1").arg(ui->checkBoxDoubleSet->isChecked()?valueX[i]:valueA[i]));
                btnsY[i]->setText(QString("%1").arg(valueY[i]));

                SetButtonTipColor(btnsX[i],dpiColors[i]);
                SetButtonTipColor(btnsY[i],dpiColors[i]);
            }

            int index=(int)mouseCfg->dpiData.curDpiId;
            qDebug()<< "index: " << index << mouseCfg->rgbEffect << mouseCfg->rgbSpeed;

            if(index>7) index = 7;

            selectDpiX=index;
            selectDpiY=index;
            btnsX[index]->click();
            btnsY[index]->click();
            ui->horizontalSliderX->setValue(QString("%1").arg(valueX[index]));
            ui->horizontalSliderY->setValue(QString("%1").arg(valueY[index]));

            ui->horizontalSliderBrightness->setValue((int)mouseCfg->rgbBrightness);
            ui->buttonGroupRgbEffect->buttons().at((int)mouseCfg->rgbEffect)->setChecked(true);
            ui->buttonGroupRgbSpeed->buttons().at((int)mouseCfg->rgbSpeed-1)->setChecked(true);

            updateButtonInfo();
        });

        connect(ui->checkBoxDoubleSet,&QCheckBox::clicked,this,[=](bool checked){
            ui->label_X->setVisible(checked);
            ui->label_Y->setVisible(checked);
            ui->frameYDPI->setVisible(checked);

            for(int i=0; i<8; i++)
            {
                btnsX[i]->setText(QString("%1").arg(ui->checkBoxDoubleSet->isChecked()?valueX[i]:valueA[i]));
                btnsY[i]->setText(QString("%1").arg(valueY[i]));
            }
            ui->horizontalSliderX->setValue(ui->checkBoxDoubleSet->isChecked()?valueX[selectX]:valueA[selectA]);
            ui->horizontalSliderY->setValue(valueY[selectY]);

            btnsX[selectDpiX]->click();
            btnsY[selectDpiX]->click();
        });
        ui->checkBoxDoubleSet->click();

        connect(ui->lineEditValueX,&QLineEdit::editingFinished,this,[=]{
            ui->horizontalSliderX->setValue(ui->lineEditValueX->text().toInt());
        });

        connect(ui->lineEditValueY,&QLineEdit::editingFinished,this,[=]{
            ui->horizontalSliderY->setValue(ui->lineEditValueY->text().toInt());
        });

        connect(ui->buttonGroupX,&QButtonGroup::idClicked,this,[=](int id){
            selectA = abs(id)-2;
            selectX = abs(id)-2;

            QPushButton *pBtn = static_cast<QPushButton *>(ui->buttonGroupX->button(id));
            selectDpiX = pBtn->objectName().replace("pushButtonX","").toInt();

            serviceDevice->setDpiIndex(selectDpiX);
            btnsY[selectDpiX]->setChecked(true);
            ui->lineEditValueY->setText(QString("%1").arg(valueY[selectDpiX]));
            ui->horizontalSliderX->setValue(ui->checkBoxDoubleSet->isChecked()?valueX[selectX]:valueA[selectA]);
        });

        connect(ui->horizontalSliderX,&QSlider::valueChanged,this,[=](int value){
            ui->lineEditValueX->setText(QString("%1").arg(value));

            valueA[selectX] = value;
            valueX[selectX] = value;
            if(ui->checkBoxDoubleSet->isChecked())
            {
                serviceDevice->setDpiX(selectDpiX,value);
                serviceDevice->setDpiY(selectDpiY,valueY[selectDpiY]);
            }
            else
            {
                serviceDevice->setDpiXY(selectDpiX,value);
            }

            ui->spinBoxDpiColor->setValue(selectDpiX+1);
            serviceDevice->setDpiIndex(selectDpiX);
            btnsX[selectX]->setText(QString("%1").arg(value));
        });

        connect(ui->buttonGroupY,&QButtonGroup::idClicked,this,[=](int id){
            selectY = abs(id)-2;

            QPushButton *pBtn = static_cast<QPushButton *>(ui->buttonGroupY->button(id));
            selectDpiY = pBtn->objectName().replace("pushButtonY","").toInt();
            btnsX[selectDpiY]->setChecked(true);
            ui->lineEditValueX->setText(QString("%1").arg(valueX[selectDpiY]));
            ui->horizontalSliderY->setValue(valueY[selectY]);
        });

        connect(ui->horizontalSliderY,&QSlider::valueChanged,this,[=](int value){
            ui->lineEditValueY->setText(QString("%1").arg(value));

            valueY[selectY] = value;
            serviceDevice->setDpiY(selectDpiY,value);

            btnsY[selectY]->setText(QString("%1").arg(value));
        });

        QList<QAbstractButton *> btns  = ui->buttonGroupSuper->buttons();
        foreach (QAbstractButton* btn, btns) {
            btn->setCheckable(true);
            btn->setAutoExclusive(true);
            //qDebug().noquote() << QString("{%1,\"%2\"},").arg(btn->objectName().replace("pushButton_",""), btn->text());
        }

    }

    {
        static QList<int> reportRates={125,250,500,1000,2000,4000,8000};
        static QList<int> sleepTimes={10,30,60,120,180,900,1800};
        static QList<int> shakeDelays={1,2,4,8,15,20};
        static QList<int> slientHeights={700,1000,2000};

        connect(ui->horizontalSlider00,&QSlider::valueChanged,this,[=](int value){
            serviceDevice->setReportRate(reportRates[value]);
        });
        connect(ui->horizontalSlider01,&QSlider::valueChanged,this,[=](int value){
            serviceDevice->setShakeDelayTime(shakeDelays[value]);
        });

        connect(ui->horizontalSlider02,&QSlider::valueChanged,this,[=](int value){
            serviceDevice->setSleepTime(sleepTimes[value]);
        });

        connect(ui->verticalSlider00,&QSlider::valueChanged,this,[=](int value){
            serviceDevice->setSilenceHeight(slientHeights[value]);
        });

        connect(ui->checkBoxAngle,&QCheckBox::clicked,this,[=](bool checked){
            serviceDevice->setSensorSwitch(checked);
        });

        connect(ui->checkBoxLine,&QCheckBox::clicked,this,[=](bool checked){
            serviceDevice->setStraight(checked);
        });

        connect(ui->checkBox20000FPS,&QCheckBox::clicked,this,[=](bool checked){
            serviceDevice->setCompetitiveMode(checked);
        });

        connect(ui->checkBoxSyncMove,&QCheckBox::clicked,this,[=](bool checked){
            serviceDevice->setMotionSync(checked);
        });
        connect(ui->checkBoxWaveCtrl,&QCheckBox::clicked,this,[=](bool checked){
            serviceDevice->setRippleControl(checked);
        });

        connect(ui->pushButtonAngleM,&QPushButton::clicked,this,[=]{
            m_nowAngle--;
            if(m_nowAngle < -30) m_nowAngle = -30;
            update();
            serviceDevice->setSensorAngle(m_nowAngle);
        });

        connect(ui->pushButtonAngleP,&QPushButton::clicked,this,[=]{
            m_nowAngle++;
            if(m_nowAngle >  30) m_nowAngle = 30;
            update();
            serviceDevice->setSensorAngle(m_nowAngle);
        });

        ui->horizontalSlider00->setShowMark();
        ui->horizontalSlider01->setShowMark();
        ui->horizontalSlider02->setShowMark();

        connect(this,&DialogMouse::genUpdate,this,[=]{
            auto mouseCfg = serviceDevice->getMouseCfg();

            // std::cout
            //     << "isSupportMoveOffLed: " << (mouseCfg->isSupportMoveOffLed ? "true" : "false") << "\n"
            //     << "moveOffLed: " << (mouseCfg->moveOffLed ? "true" : "false") << "\n"
            //     << "isSupportDpiLight: " << (mouseCfg->isSupportDpiLight ? "true" : "false") << "\n"

            //     << "ambientEffect: " << (int)mouseCfg->ambientEffect << "\n"
            //     << "ambientBrightness: " << (int)mouseCfg->ambientBrightness << "\n"
            //     << "ambientSpeed: " << (int)mouseCfg->ambientSpeed << "\n"
            //     << "ambientCol or: " << mouseCfg->ambientColor << std::endl;
            ui->horizontalSlider00->setValue(reportRates.indexOf((int)mouseCfg->reportRate));
            ui->horizontalSlider01->setValue(shakeDelays.indexOf((int)mouseCfg->shakeDelayTime));
            ui->horizontalSlider02->setValue(sleepTimes.indexOf((int)mouseCfg->sleepTime));
            ui->verticalSlider00->setValue(slientHeights.indexOf((int)mouseCfg->silenceHeight));

            ui->checkBoxLine->setChecked(mouseCfg->straight);
            ui->checkBox20000FPS->setChecked(mouseCfg->competitiveMode);
            ui->checkBoxSyncMove->setChecked(mouseCfg->motinSync);
            ui->checkBoxWaveCtrl->setChecked(mouseCfg->rippleControl);
        });
    }

    ui->labelAngleShow->setStyleSheet("QLabel{background-color:transparent;}");
    ui->labelAngleShow->installEventFilter(this);
    ui->labelGoBack->installEventFilter(this);
    ui->lineEditShortcut->installEventFilter(this);
    ui->labelBattery->installEventFilter(this);
    ui->frameLeft->installEventFilter(this);
    ui->frameKeyShow->installEventFilter(this);

    {
        m_pModel = new QStandardItemModel(this);
        m_pModel->setHorizontalHeaderLabels(QString("名称,类型,名称,类型,名称,类型").split(','));
        ui->tableViewMContent->setModel(m_pModel);

        QHeaderView *pHeader = ui->tableViewMContent->horizontalHeader();
        pHeader->setSectionResizeMode(QHeaderView::Stretch);
        pHeader->hide();

        QFont font1 = ui->tableViewMContent->font();
        //font1.setBold(true);
        font1.setPixelSize(16);

        for(int m=0;m<12;m++)
        {
            QList<QStandardItem*>test;
            for(int i=0; i<6; i++)
            {
                QStandardItem *item = new QStandardItem(QString(""));
                item->setTextAlignment(Qt::AlignCenter);
                item->setEditable(false);
                item->setFont(font1);
                test.push_back(item);
            }
            m_pModel->appendRow(test);
            ui->tableViewMContent->setRowHeight(m,64);
        }
        connect(m_pModel,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item){
        });

        connect(ui->tableViewMContent,&QTableView::clicked,this,[=](const QModelIndex &index){
            //pClkItem = m_pModel->itemFromIndex(index);
        });
    }
        {

        //------------------------------------

        m_pMList = new QStandardItemModel(this);
        m_pMList->setHorizontalHeaderLabels(QString("名称,类型,名称").split(','));

        ui->tableViewMList->setModel(m_pMList);

        ui->tableViewMList->setShowGrid(false);
        ui->tableViewMList->verticalHeader()->hide();

        QHeaderView *pHeader = ui->tableViewMList->horizontalHeader();
        pHeader->setSectionResizeMode(QHeaderView::Stretch);
        pHeader->setSectionResizeMode(1,QHeaderView::Fixed);
        pHeader->setSectionResizeMode(2,QHeaderView::Fixed);
        pHeader->resizeSection(1,36);
        pHeader->resizeSection(2,36);
        pHeader->hide();

        m_MLDele1 = new ImageDelegate(":/images/mouse/edit-0.png",ui->tableViewMList,1,this);
        m_MLDele2 = new ImageDelegate(":/images/mouse/delete-0.png",ui->tableViewMList,2,this);
        QFont font = ui->tableViewMList->font();
        //font.setBold(true);
        font.setPixelSize(16);

        saveLoadMacroHeader(false);

        connect(ui->tableViewMList,&QTableView::clicked,this,[=](const QModelIndex &index){
            QStandardItem *item = m_pMList->itemFromIndex(index);

            m_currentId = item->data().toInt();
            QList<macroItem*>mEvts;
            saveLoadMacroContent(m_currentId,mEvts,false);

            ui->pushButtonClear->click();

            int count = mEvts.count();
            for(int i=0; i<count; i++)
            {
                QStandardItem *item = m_pModel->item(i/6,i%6);
                if(item) item->setData((quint64)mEvts[i]);
                refreshMacroItem(item);
            }
            ui->labelTitle4->setText(QString("%1 / 70").arg(count));
        });

        connect(m_pMList,&QStandardItemModel::itemChanged,this,[=](QStandardItem *item){
            if(m_loading) return;
            saveLoadMacroHeader();
        });

        connect(m_MLDele1,&ImageDelegate::clicked,this,[=](const QModelIndex&index){
            QModelIndex test =m_pMList->index(index.row(),0);
            ui->tableViewMList->edit(test);
        });

        connect(m_MLDele2,&ImageDelegate::clicked,this,[=](const QModelIndex&index){            
            QStandardItem *item = m_pMList->itemFromIndex(index);
            int id = item->data().toInt();
            int count = ui->comboBoxMacro->count();
            for(int i=0; i<count; i++)
            {
                if(ui->comboBoxMacro->itemData(i).toInt() == id)
                {
                    ui->comboBoxMacro->removeItem(i);
                    break;
                }
            }
            m_pMList->removeRows(index.row(),1);
            saveLoadMacroHeader();
            m_currentId = -1;
        });

        connect(ui->pushButtonAddMacro,&QPushButton::clicked,this,[=]{
            addMacroHeed(0,"");
            saveLoadMacroHeader();
        });

        connect(ui->pushButtonRecord,&QPushButton::clicked,this,[=]{
            if(m_currentId == -1)
            {
                QMessageBox::warning(this,"提示","首先选择或者新建一个宏才能进行录制，请重试！");
                return;
            }

            if(!m_recording)
            {
                ui->pushButtonClear->click();
            }

            QTimer::singleShot(200,this,[=]{
                m_recording = ui->pushButtonRecord->isChecked();
                saveLoadMacroContent(m_currentId,macroGroup,true);
            });

            setFocusPolicy(Qt::StrongFocus);
            this->setFocus();
        });

        connect(ui->pushButtonClear,&QPushButton::clicked,this,[=]{
            m_lastItem = nullptr;
            macroGroup.clear();
            ui->labelTitle4->setText("0 / 70");
            for(int m=0;m<12;m++)
            {
                for(int n=0; n<6; n++)
                {
                    m_pModel->item(m,n)->setText("");
                }
            }
        });

        connect(ui->tableViewMContent->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, [=](const QItemSelection &selected, const QItemSelection &deselected){
                    for(auto idx : selected.indexes())
                    {
                        QStandardItem* item = m_pModel->itemFromIndex(idx);
                    }
                });

        connect(ui->pushButtonDelete,&QPushButton::clicked,this,[=]{
            m_lastItem = nullptr;
            QStandardItem* item = nullptr;
            QModelIndexList selecteds = ui->tableViewMContent->selectionModel()->selectedIndexes();
            if(selecteds.count())
                item = m_pModel->itemFromIndex(selecteds.at(0)) ;
            int count = macroGroup.count();
            if(count>0 && item)
            {
                int index = item->row() * 6 + item->column();

                if(index<count)
                {
                    if(index%2 == 0)
                    {
                        macroGroup.removeAt(index);
                        macroGroup.removeAt(index);
                    }
                    else
                    {
                        macroGroup.removeAt(index-1);
                        macroGroup.removeAt(index-1);
                    }
                }
            }

            for(int m=0;m<12;m++)
            {
                for(int n=0; n<6; n++)
                {
                    m_pModel->item(m,n)->setText("");
                }
            }

            count = macroGroup.count();
            for(int i=0; i<count; i++)
            {
                QStandardItem *item = m_pModel->item(i/6,i%6);
                if(item) item->setData((quint64)macroGroup[i]);
                refreshMacroItem(item);
            }
            ui->labelTitle4->setText(QString("%1 / 70").arg(count));
            saveLoadMacroContent(m_currentId,macroGroup,true);
        });

        connect(ui->pushButtonSetDelay,&QPushButton::clicked,this,[=]{
            int count = macroGroup.count();
            for(int i=0; i<count; i++)
            {
                macroGroup[i]->delay = ui->spinBoxDelay->value();
                refreshMacroItem(m_pModel->item(i/6,i%6));
            }
        });
    }

    connect(ui->pushButtonResetDevice,&QPushButton::clicked,this,[=]{
        serviceDevice->restoreDefaultConfig();
    });

    QTimer::singleShot(1000,this,[=]{
        setFixedSize(1280,900);
        ui->label_12->setFixedHeight(20);
        ui->label_15->setFixedHeight(20);
        ui->label_13->setFixedHeight(20);
        ui->label_48->setFixedHeight(20);
        ui->labelGoBack->setFixedWidth(20);

        ui->lineEditValueX->setFixedHeight(32);
        ui->lineEditValueY->setFixedHeight(32);
    });
}

void DialogMouse::updateButtonInfo()
{
    auto btnInfo = serviceDevice->getButtonCfg();
    if (btnInfo == nullptr) {
        return;
    }

    for (const auto &item : *btnInfo) {
        QString strName=QString("pushButtonMkey%1").arg(item.keyId);
        QPushButton *btn = findChild<QPushButton*>(strName);
        QString strChangedText;
        switch (item.keyType)
        {
        case InfortechDef::KeyType::BaseKey:
        {
            auto baseKeyData = item.cfg->getBaseKeyInf();
            if (baseKeyData != nullptr) {
                if(item.keyId != baseKeyData->funKeyId)
                    strChangedText = QString("普通按键: ") + getKeyNameByDF(baseKeyData->funKeyId);
            }
            break;
        }
        case InfortechDef::KeyType::ComboKey:
        {
            auto comboKeyData = item.cfg->getComboKeyInf();
            if (comboKeyData != nullptr) {
                QString strSys = getKeyNameByDF(comboKeyData->sysKey[0]) + " + ";
                if (comboKeyData->sysKey.size() == 2)
                {
                    strSys+= getKeyNameByDF(comboKeyData->sysKey[1]) + " + ";
                }
                strSys+= getKeyNameByDF(comboKeyData->customKey);
                strChangedText = QString("组合键: ") + strSys;
            }

            break;
        }
        case InfortechDef::KeyType::FireKey:
        {
            auto fireKeyData = item.cfg->getFireKeyInf();
            if (fireKeyData != nullptr) {
                strChangedText = QString("火力键: ") + QString::asprintf("点击次数: %d, 点击间隔: %d, 保持点击: ",(int)fireKeyData->clickNum,(int)fireKeyData->interval) + QString(fireKeyData->keepClick?"是":"否");
            }

            break;
        }
        case InfortechDef::KeyType::MacroKey:
        {
            auto macroKeyData = item.cfg->getMacroKeyInf();
            if (macroKeyData != nullptr)
            {
                strChangedText = QString::asprintf("宏设置: ID: %d, 类型: %d, 数据量: %d\n",macroKeyData->macroId,macroKeyData->macroType,macroKeyData->cycleNumber);

                for (auto item : macroKeyData->data) {
                    strChangedText += QString("动作: ")  + (item.action == InfortechDef::Action::Press ? "按下" : "松开");
                    strChangedText += QString(",按键: ") + (getKeyNameByDF(item.keyId));
                    strChangedText += QString(",延迟: %1\n").arg(item.delayTime);
                }
            }
            break;
        }
        default:
            break;
        }

        keyChangeset[item.keyId] = strChangedText;

        bool left = false;
        if(item.keyId == 1702 || item.keyId == 1703 || item.keyId == 1706)
            left = true;

        KeyboardButton *pBtn = static_cast<KeyboardButton *>(btn);

        if(strChangedText.isEmpty())
        {
            pBtn->setTipText("","");
            SetButtonBackground(btn,left);
        }
        else
        {
            pBtn->setTipText(getKeyNameByDF(item.keyId),strChangedText);
            SetButtonBackground(btn,left,"",QColor("pink"),QColor("pink"));
        }
    }

}

DialogMouse::~DialogMouse()
{
    delete ui;
}

void DialogMouse::hideEvent(QHideEvent *event)
{
    emit hideAction();
    QDialog::hideEvent(event);
}

void DialogMouse::keyPressEvent(QKeyEvent *event)
{
    auto key = event->key();

    //qDebug() << "DialogMouse::keyPressEvent" << key << event->nativeVirtualKey() << event->nativeScanCode() << event->text();

    if(key == Qt::Key_Escape)
    {
        if(ui->frameLeft->isVisible())
        {
            ui->frameKeyShow->hide();
        }
        else
        {
            this->hide();
        }
        event->accept();
        return;
    }

    if(key == Qt::Key_Return || key == Qt::Key_Enter)
    {
        event->accept();
        return;
    }

    QDialog::keyPressEvent(event);
}

void DialogMouse::keyReleaseEvent(QKeyEvent *event)
{
    //qDebug() << "DialogMouse::keyReleaseEvent" << event->nativeVirtualKey() << event->nativeScanCode() << event->text();
    QDialog::keyReleaseEvent(event);
}


void DialogMouse::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "DialogMouse::mousePressEvent";
    if (event->button() == Qt::LeftButton)
    {
        if (event->pos().y() < 60)
        {
            m_dragPosition = event->globalPosition() - frameGeometry().topLeft();
            m_dragging = true;
        }
    }

    QDialog::mousePressEvent(event);
}

void DialogMouse::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_dragging)
    {
        QPointF MP = event->globalPosition() - m_dragPosition;
        move(MP.toPoint());
    }

    QDialog::mouseMoveEvent(event);
}

void DialogMouse::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "DialogMouse::mouseReleaseEvent";
    m_dragging = false;

    QDialog::mouseReleaseEvent(event);
}

bool DialogMouse::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride)
    {
        //event->accept();
    }
    if(event->type() == QEvent::MouseButtonDblClick)
    {
        // m_loading = false;
        // m_lastDelay = m_tcount.elapsed();
        // if(m_delay) m_delay->setDelay(m_lastDelay);
        m_tcount.restart();

        QMouseEvent *pME = static_cast<QMouseEvent *>(event);
        quint8 btn = pME->button();
        // addMacroSquare("",1,btn,true);
        // addMacroSquare("",1,btn,false);
        // addMacroSquare("",1,btn,true);
        // addMacroSquare("",1,btn,false);
        // updateView();

        return true;
    }

    if(event->type() == QEvent::MouseButtonPress)
    {
        //qDebug() << "event::MouseButtonPress";
        // m_loading = false;
        // MacroSquare::LostFocus();
        // m_lastDelay = m_tcount.elapsed();
        // if(m_delay) m_delay->setDelay(m_lastDelay);
        m_tcount.restart();

        QMouseEvent *pME = static_cast<QMouseEvent *>(event);
        quint8 btn = pME->button();
        //addMacroSquare("",1,btn,true);
    }

    if(event->type() == QEvent::MouseButtonRelease)
    {
        //qDebug() << "event::MouseButtonRelease" << m_tcount.elapsed();
        m_tcount.restart();
        // m_loading = false;

        // m_lastDelay = m_tcount.elapsed();
        // if(m_delay) m_delay->setDelay(m_lastDelay);

        QMouseEvent *pME = static_cast<QMouseEvent *>(event);
        quint8 btn = pME->button();
        // addMacroSquare("",1,btn,false);
    }
    if(event->type() == QEvent::Wheel)
    {
        //qDebug() << "event::Wheel";
        m_tcount.restart();
        // m_loading = false;

        // m_lastDelay = m_tcount.elapsed();
        // if(m_delay) m_delay->setDelay(m_lastDelay);

        QWheelEvent *pWE = static_cast<QWheelEvent *>(event);
        //quint8 btn = pME->button();
        // addMacroSquare("",1,btn,false);
    }

    if(event->type() == QEvent::KeyPress)
    {
        m_tcount.restart();

        QKeyEvent *pKE = static_cast<QKeyEvent *>(event);
        //qDebug() << "event::KeyPress   ------ " << pKE->key() << pKE->nativeVirtualKey() << pKE->nativeScanCode() << getKeyDefineVK(pKE->nativeVirtualKey());

    }

    if(event->type() == QEvent::KeyRelease)
    {
        //qDebug().nospace() << "event::KeyRelease ++++++  " << m_tcount.elapsed() << "ms";
        m_tcount.restart();
        // m_loading = false;

        // m_lastDelay = m_tcount.elapsed();
        // if(m_delay) m_delay->setDelay(m_lastDelay);

        QKeyEvent *pKE = static_cast<QKeyEvent *>(event);
        //quint8 btn = pME->button();
        // addMacroSquare("",1,btn,false);
    }
    return QDialog::event(event);
}


bool DialogMouse::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        m_tcount.restart();

        QKeyEvent *pKE = static_cast<QKeyEvent *>(event);
        qDebug() << "eventFilter::KeyPress" << pKE->key() << pKE->nativeVirtualKey() << getKeyDefineVK(pKE->nativeVirtualKey());

    }

    if(QEvent::Show == event->type())
    {
        if(watched == ui->frameKeyShow)
        {
            ui->frameLeft->hide();
            this->setFixedWidth(1440);
        }
    }

    if(QEvent::Hide == event->type())
    {
        if(watched == ui->frameKeyShow)
        {
            ui->frameLeft->show();
            this->setFixedWidth(1280);
        }
    }

    if(watched == ui->lineEditShortcut)
    {
        if(event->type() == QEvent::KeyPress)
        {
            auto ke = static_cast<QKeyEvent *>(event);
            event->accept();

            auto key = ke->nativeVirtualKey();
            int def  = getKeyDefineVK(key);
            m_pressDf= def;
            QString text(getKeyNameByDF(def));
            ui->lineEditShortcut->setText(text.toUpper());

            return true;
        }
    }

    if(watched == ui->labelGoBack)
    {
        if(event->type() == QEvent::MouseButtonRelease)
        {
            ui->frameKeyShow->hide();
        }
    }

    if(watched == ui->labelAngleShow)
    {
        QRect rect = ui->labelAngleShow->rect();
        int nW = rect.width();
        int nCX = nW/2;
        int ra0 = nCX-1;
        int ra1 = nCX-25;

        float start = 270;

        for(int i=start; i<450; i+=1)
        {
            QPoint p0(nCX+ra0*cos(i*3.1415936/180),nCX-ra0*sin(i*3.1415936/180));
            if(rect.contains(p0))
            {
                start = i;
                break;
            }
        }

        float full = 180 + (360 - start) * 2.0;
        float fStep = full * 1.0 / 60;

        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *pMEvt = static_cast<QMouseEvent *>(event);

            double dx = pMEvt->pos().x() - nCX;
            double dy = pMEvt->pos().y() - nCX;
            double rad = qAtan2(dy, dx);
            double deg = fmod(360 - qRadiansToDegrees(rad),360);

            m_nowAngle = 30 - fmod((360 + deg - start),360) / fStep;
            ui->labelAngleShow->update();            
            serviceDevice->setSensorAngle(m_nowAngle);
        }

        if(event->type() == QEvent::Paint)
        {
            QPainter paniter(ui->labelAngleShow);
            paniter.setRenderHint(QPainter::Antialiasing);
            paniter.fillRect(rect,Qt::transparent);

            paniter.setPen(QPen(QColor("#373737"),4));
            for(int i=0; i<=60; i++)
            {
                float curAngle = (start + i * fStep) * 3.1415936/180;
                QPoint p0(nCX+ra0*cos(curAngle),nCX-ra0*sin(curAngle));
                QPoint p1(nCX+ra1*cos(curAngle),nCX-ra1*sin(curAngle));
                if(!rect.contains(p0))
                    break;

                paniter.drawLine(p0,p1);
            }

            int nAngle = m_nowAngle;
            if(nAngle<-30) nAngle = -30;
            if(nAngle>30)  nAngle = 30;
            float curAngle = (start + (30 - nAngle) * fStep) * 3.1415936/180;
            paniter.setPen(QPen(Qt::white,4));
            QPoint p0(nCX+ra0*cos(curAngle),nCX-ra0*sin(curAngle));
            QPoint p1(nCX+(ra1-8)*cos(curAngle),nCX-(ra1-8)*sin(curAngle));
            paniter.drawLine(p0,p1);
            ui->labelAngleValue->setText(QString("%1°").arg(nAngle));
        }
    }

    if(watched == ui->labelBattery && event->type() == QEvent::Paint)
    {
        QRect rect = ui->labelBattery->rect();
        QImage image(rect.size(),QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);

        QRect batRc = rect.adjusted(6,8,-20,-8);

        QPainterPath path;
        QRect bodyRc = batRc.adjusted(0,0,-16,0);
        path.addRoundedRect(bodyRc,6,6);
        painter.setClipPath(path);

        int battery = m_battery;
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::darkGreen);
        painter.drawRect(bodyRc.adjusted(0,0,-bodyRc.width()/100.0 * (100-battery),0));
        painter.setClipping(false);

        painter.setPen(QPen(Qt::gray,2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(batRc.adjusted(0,0,-16,0),6,6);

        painter.setBrush(Qt::gray);
        painter.drawRoundedRect(batRc.adjusted(batRc.width()-16,6,-10,-6),4,4);
        painter.drawRect(batRc.adjusted(batRc.width()-16,6,-12,-6));
        //image.save("D:\\battery-008.png");

        painter.setFont(ui->labelBattery->font());
        painter.setPen(Qt::white);
        painter.drawText(rect.adjusted(4,0,-30,0),QString("%1%").arg(battery),QTextOption(Qt::AlignCenter));

        ui->labelBattery->setPixmap(QPixmap::fromImage(image));
    }

    return QDialog::eventFilter(watched, event);
}

void DialogMouse::updateName(const QString&name)
{
    ui->labelName->setText(name);
    ui->labelDeviceName->setText(name);
}


void DialogMouse::updateImage(const QString&image)
{
    QPixmap Img(image);
    if(Img.isNull()) return;

    ui->labelMouse->setPixmap(Img);
    ui->labelDeviceImage->setPixmap(Img);

    int nw = 260;
    float factor=nw*1.0/Img.width();
    int nh = Img.height()*factor;
    ui->labelMouse->setFixedSize(nw,nh);
}

void DialogMouse::changeEvent(QEvent *pEvt)
{
    if(pEvt->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        getAllCfg();
    }

    QDialog::changeEvent(pEvt);
}

//-----------------------------------------------------------------------------

// 打印设备信息
void DialogMouse::printDeviceInf(InfortechDef::DevInf *deviceInf) {
    if (deviceInf != nullptr)
    {
        std::cout << "\nSuccessfully obtained device information." << std::endl;
        std::cout
            << "mac_address: " << deviceInf->getMacAddressStr() << "\n"
            << "mouseBatteryCharging: " << (deviceInf->mouseBatteryCharging ? "true" : "false") << "\n"
            << "mouseBatteryFilled: " << (deviceInf->mouseBatteryFilled ? "true" : "false") << "\n"
            << "mouseBatterylevel: " << (int)deviceInf->mouseBatterylevel << "%" << "\n"
            << "mouseFirmwareVersion: " << deviceInf->getMouseFwVerStr() << "\n"
            << "receiverFirmwareVersion: " << deviceInf->getReceiverFwVerStr() << "\n"
            << "configId: " << (int)deviceInf->confId << std::endl;

        m_battery =deviceInf->mouseBatterylevel;
        m_charging=deviceInf->mouseBatteryCharging;
        update();
    }
}

// 打印按键配置
void DialogMouse::printButtonInf(const InfortechDef::AllBtnCfg *btnInf) {
    if (btnInf == nullptr) {
        std::cout << "The key configuration data is empty." << std::endl;
    }

    for (const auto &item : *btnInf) {
        switch (item.keyType) {
        case InfortechDef::KeyType::BaseKey:
        {
            auto baseKeyData = item.cfg->getBaseKeyInf();
            if (baseKeyData != nullptr) {
                std::cout << "btnId: " << item.keyId << ", type: BaseKey" << ", cfg: " << baseKeyData->funKeyId << std::endl;
            }
            break;
        }
        case InfortechDef::KeyType::ComboKey:
        {
            auto comboKeyData = item.cfg->getComboKeyInf();
            if (comboKeyData != nullptr) {
                if (comboKeyData->sysKey.size() == 2) {
                    std::cout << "btnId: " << item.keyId << ", type: ComboKey" << ", cfg: "
                              << ", sysKey: [" << comboKeyData->sysKey[0] << "," << comboKeyData->sysKey[1] << "]"
                              << ", customKey: " << comboKeyData->customKey << std::endl;
                }
                else if (comboKeyData->sysKey.size() == 1) {
                    std::cout << "btnId: " << item.keyId << ", type: ComboKey" << ", cfg: "
                              << ", sysKey: [" << comboKeyData->sysKey[0] << "]"
                              << ", customKey: " << comboKeyData->customKey << std::endl;
                }
            }
            break;
        }
        case InfortechDef::KeyType::FireKey:
        {
            auto fireKeyData = item.cfg->getFireKeyInf();
            if (fireKeyData != nullptr) {
                std::cout << "btnId: " << item.keyId << ", type: FireKey" << ", cfg: "
                          << "clickNum: " << (int)fireKeyData->clickNum
                          << ", interval: " << (int)fireKeyData->interval
                          << ", keepClick: " << (fireKeyData->keepClick ? "true" : "false") << std::endl;
            }
            break;
        }
        case InfortechDef::KeyType::MacroKey:
        {
            auto macroKeyData = item.cfg->getMacroKeyInf();
            if (macroKeyData != nullptr) {
                std::cout << "btnId: " << item.keyId << ", type: MacroKey" << ", cfg: "
                          << "macroId: " << (int)macroKeyData->macroId
                          << ", macroType: " << (int)macroKeyData->macroType
                          << ", macroCycleNumber: " << (int)macroKeyData->cycleNumber
                          << ", data: {" << std::endl;

                for (auto item : macroKeyData->data) {
                    std::cout << "    { action: " << (item.action == InfortechDef::Action::Press ? "Press" : "Release")
                    << ", keyId: " << item.keyId << ", delayTime: " << item.delayTime << " }" << std::endl;
                }
                std::cout << "}" << std::endl;
            }
            break;
        }
        default:
            break;
        }
    }
}

// 打印鼠标配置
void DialogMouse::printMouseCfg(const InfortechDef::MouseCfg* mouseCfg) {
    if (mouseCfg == nullptr) {
        std::cout << "The mouse configuration data is empty." << std::endl;
    }

    std::cout
        << "reportRate: " << (int)mouseCfg->reportRate << "\n"
        << "sleepTime: " << (int)mouseCfg->sleepTime << "\n"
        << "shakeDelayTime: " << (int)mouseCfg->shakeDelayTime << "\n"
        << "silenceHeight: " << (int)mouseCfg->silenceHeight << "\n"

        << "competitiveMode: " << (mouseCfg->competitiveMode ? "true" : "false") << "\n"
        << "motinSync: " << (mouseCfg->motinSync ? "true" : "false") << "\n"
        << "straight: " << (mouseCfg->straight ? "true" : "false") << "\n"
        << "rippleControl: " << (mouseCfg->rippleControl ? "true" : "false") << "\n"
        << "isSupportMoveOffLed: " << (mouseCfg->isSupportMoveOffLed ? "true" : "false") << "\n"
        << "moveOffLed: " << (mouseCfg->moveOffLed ? "true" : "false") << "\n"
        << "isSupportDpiLight: " << (mouseCfg->isSupportDpiLight ? "true" : "false") << "\n"

        << "rgbBrightness: " << (int)mouseCfg->rgbBrightness << "\n"
        << "rgbEffect: " << (int)mouseCfg->rgbEffect << "\n"
        << "rgbSpeed: " << (int)mouseCfg->rgbSpeed << "\n"

        << "ambientEffect: " << (int)mouseCfg->ambientEffect << "\n"
        << "ambientBrightness: " << (int)mouseCfg->ambientBrightness << "\n"
        << "ambientSpeed: " << (int)mouseCfg->ambientSpeed << "\n"
        << "ambientColor: " << mouseCfg->ambientColor << "\n"

        << "curDpiId: " << (int)mouseCfg->dpiData.curDpiId << "\n"
        << "dpiData: {" << std::endl;

    for (auto item : mouseCfg->dpiData.data) {
        std::cout << "    { xDpi: " << item.xDpi << ", yDpi: " << item.yDpi
                  << ", color: " << item.color << " }" << std::endl;
    }
    std::cout << "}" << std::endl;
}

// 打印额外配置
void DialogMouse::printMouseExtraInfo(const InfortechDef::ExtCfgInfo* extCfg) {
    if (extCfg == nullptr) {
        std::cout << "The mouse configuration data is empty." << std::endl;
    }

    std::cout
        << "mouseVid: " << (int)extCfg->mouseVid << "\n"
        << "mousePid: " << (int)extCfg->mousePid << "\n"
        << "sensorModel: " << (int)extCfg->sensorType << "\n"
        << "sensorDpiUiMax: " << (int)extCfg->maxDpiUi << "\n"
        << "sensorRptMax: " << (int)extCfg->maxRpt << std::endl;

    std::cout << "indAxisBtns: [ ";
    for (auto item : extCfg->indAxisBtns) {
        std::cout << item << ", ";
    }
    std::cout << "]" << std::endl;

    std::cout << "silenceHeightRange: [ ";
    for (auto item : extCfg->silenceHeightRange) {
        std::cout << item << ", ";
    }
    std::cout << "]" << std::endl;
}

// 打印传感器配置
void DialogMouse::printSensorCfg(const InfortechDef::SensorCfg* sensorCfg) {
    if (sensorCfg == nullptr) {
        std::cout << "The sensor configuretion data is empty." << std::endl;
    }

    std::cout
        << "angle: " << (int)sensorCfg->angle << "\n"
        << "angleSwitch: " << (sensorCfg->angleSwitch ? "true" : "false") << std::endl;
}

// 打印电感轴配置
void DialogMouse::printAxisCfg(const InfortechDef::AxisCfg* axisCfg) {
    if (axisCfg == nullptr) {
        std::cout << "The AxisCfg data is empty." << std::endl;
    }

    std::cout
        << "keyId: " << axisCfg->keyId << "\n"
        << "maxTrigger: " << (int)axisCfg->maxTrigger << "\n"
        << "triggerValue: " << (int)axisCfg->triggerValue << "\n"
        << "rapidSwitch: " << axisCfg->rapidSwitch << "\n"
        << "maxRapid: " << (int)axisCfg->maxRapid << "\n"
        << "rapidValue: " << (int)axisCfg->rapidValue << "\n"
        << "maxFeedback: " << (int)axisCfg->maxFeedback << "\n"
        << "feedbackValue: " << (int)axisCfg->feedbackValue << "\n"
        << "feedbackSwitch: " << axisCfg->feedbackSwitch << std::endl;
}

// 打印传感器配置
void DialogMouse::printDGAmbientCfg(const InfortechDef::DGAmbientCfg* dgAmbientCfg)
{
    if (dgAmbientCfg == nullptr) {
        std::cout << "The receiver configuretion data is empty." << std::endl;
    }

    std::cout
        << "ambientEffect: " << (int)dgAmbientCfg->ambientEffect << "\n"
        << "ambientBrightness: " << (int)dgAmbientCfg->ambientBrightness << "\n"
        << "ambientSpeed: " << (int)dgAmbientCfg->ambientSpeed << "\n"
        << "ambientColor: " << dgAmbientCfg->ambientColor << std::endl;
}

// 获取所有配置
void DialogMouse::getAllCfg()
{
    if(!serviceDevice) return;
    // 接收器版本
    std::cout << std::endl;
    auto dongleVersion = serviceDevice->getDongleVersion();
    if (dongleVersion[0] != 0 && dongleVersion[1] != 0) {
        std::cout << "Successfully obtained the receiver version." << std::endl;
        std::cout << (int)dongleVersion[1] << "." << (int)dongleVersion[0] << std::endl;
    }

    // 设备信息
    auto deviceInf = serviceDevice->getDeviceInf();
    printDeviceInf(deviceInf.get());

    // 按键配置
    std::cout << std::endl;
    auto btnInfo = serviceDevice->getButtonCfg();
    if (btnInfo != nullptr) {
        std::cout << "Successfully obtained the key configuration." << std::endl;
        printButtonInf(btnInfo.get());
    }

    // 鼠标配置
    std::cout << std::endl;
    auto mouseCfg = serviceDevice->getMouseCfg();
    if (mouseCfg != nullptr) {
        std::cout << "Successfully obtained the mouse configuration." << std::endl;
        printMouseCfg(mouseCfg.get());
    }

    // 鼠标额外配置
    std::cout << std::endl;
    auto extraCfg = serviceDevice->getMouseExtraInfo();
    if (extraCfg != nullptr) {
        std::cout << "Successfully obtained the mouse extra information." << std::endl;
        printMouseExtraInfo(extraCfg.get());
    }

    // 传感器配置
    std::cout << std::endl;
    auto sensorCfg = serviceDevice->getSensorCfg();
    if (sensorCfg != nullptr) {
        std::cout << "Successfully obtained the sensor configuration." << std::endl;
        printSensorCfg(sensorCfg.get());
    }

    // 接收器氛围灯配置
    std::cout << std::endl;
    auto dgAmbientCfg = serviceDevice->getDGAmbientCfg();
    if (dgAmbientCfg != nullptr) {
        std::cout << "Successfully obtained the receiver ambient configuration." << std::endl;
        printDGAmbientCfg(dgAmbientCfg.get());
    }

    if(deviceInf)
    {
        ui->buttonGroupConfig->buttons().at((int)deviceInf->confId)->setChecked(true);
        ui->labelFirmwareVer->setText(deviceInf->getMouseFwVerStr().c_str());
        ui->labelReceiverVer->setText(deviceInf->getReceiverFwVerStr().c_str());
        m_nowAngle = sensorCfg->angle;
        ui->checkBoxAngle->setChecked(sensorCfg->angleSwitch);
        update();

        emit genUpdate();
    }
}

// 1. 定义按键映射结构体
typedef struct {
    uint8_t hid0;       // USB HID 码
    uint8_t hid1;       // USB HID 码
    uint16_t vk0;       // Windows VK 码
    uint16_t vk1;       // Windows VK 码
    uint16_t df0;       //
    uint16_t df1;       //
    uint16_t df2;       //
    uint16_t df3;       //
    const char *name;  // 按键名称
} KeyMap;

// 2. 完整的按键映射表
const KeyMap KEYBOARD_MAP[] = {
    {0x04,  4,0x41, 65,1001,   0,   0,   0,"A"},
    {0x05,  5,0x42, 66,1002,   0,   0,   0,"B"},
    {0x06,  6,0x43, 67,1003,   0,   0,   0,"C"},
    {0x07,  7,0x44, 68,1004,   0,   0,   0,"D"},
    {0x08,  8,0x45, 69,1005,   0,   0,   0,"E"},
    {0x09,  9,0x46, 70,1006,   0,   0,   0,"F"},
    {0x0A, 10,0x47, 71,1007,   0,   0,   0,"G"},
    {0x0B, 11,0x48, 72,1008,   0,   0,   0,"H"},
    {0x0C, 12,0x49, 73,1009,   0,   0,   0,"I"},
    {0x0D, 13,0x4A, 74,1010,   0,   0,   0,"J"},
    {0x0E, 14,0x4B, 75,1011,   0,   0,   0,"K"},
    {0x0F, 15,0x4C, 76,1012,   0,   0,   0,"L"},
    {0x10, 16,0x4D, 77,1013,   0,   0,   0,"M"},
    {0x11, 17,0x4E, 78,1014,   0,   0,   0,"N"},
    {0x12, 18,0x4F, 79,1015,   0,   0,   0,"O"},
    {0x13, 19,0x50, 80,1016,   0,   0,   0,"P"},
    {0x14, 20,0x51, 81,1017,   0,   0,   0,"Q"},
    {0x15, 21,0x52, 82,1018,   0,   0,   0,"R"},
    {0x16, 22,0x53, 83,1019,   0,   0,   0,"S"},
    {0x17, 23,0x54, 84,1020,   0,   0,   0,"T"},
    {0x18, 24,0x55, 85,1021,   0,   0,   0,"U"},
    {0x19, 25,0x56, 86,1022,   0,   0,   0,"V"},
    {0x1A, 26,0x57, 87,1023,   0,   0,   0,"W"},
    {0x1B, 27,0x58, 88,1024,   0,   0,   0,"X"},
    {0x1C, 28,0x59, 89,1025,   0,   0,   0,"Y"},
    {0x1D, 29,0x5A, 90,1026,   0,   0,   0,"Z"},
    {0x1E, 30,0x30, 48,1310,   0,   0,   0,"0"},
    {0x1F, 31,0x31, 49,1301,   0,   0,   0,"1"},
    {0x20, 32,0x32, 50,1302,   0,   0,   0,"2"},
    {0x21, 33,0x33, 51,1303,   0,   0,   0,"3"},
    {0x22, 34,0x34, 52,1304,   0,   0,   0,"4"},
    {0x23, 35,0x35, 53,1305,   0,   0,   0,"5"},
    {0x24, 36,0x36, 54,1306,   0,   0,   0,"6"},
    {0x25, 37,0x37, 55,1307,   0,   0,   0,"7"},
    {0x26, 38,0x38, 56,1308,   0,   0,   0,"8"},
    {0x27, 39,0x39, 57,1309,   0,   0,   0,"9"},
    {0x29, 41,0x1B, 27,1502,   0,   0,   0,"ESC"},
    {0x28, 40,0x0D, 13,1501,   0,   0,   0,"Enter"},
    {0x2A, 42,0x08,  8,1503,   0,   0,   0,"Backspace"},
    {0x2B, 43,0x09,  9,1504,   0,   0,   0,"Tab"},
    {0x2C, 44,0x20, 32,1505,   0,   0,   0,"Space"},
    {0x39, 57,0x14, 20,1506,   0,   0,   0,"Caps Lock"},
    {0x49, 73,0x2D, 45,1510,   0,   0,   0,"Insert"},
    {0x4C, 76,0x2E, 46,1513,   0,   0,   0,"Delete"},
    {0x4A, 74,0x24, 36,1511,   0,   0,   0,"Home"},
    {0x4D, 77,0x23, 35,1514,   0,   0,   0,"End"},
    {0x4B, 75,0x21, 33,1512,   0,   0,   0,"Page Up"},
    {0x4E, 78,0x22, 34,1515,   0,   0,   0,"Page Down"},
    {0x52, 82,0x26, 38,1601,   0,   0,   0,"Up Arrow"},
    {0x4F, 79,0x27, 39,1602,   0,   0,   0,"Right Arrow"},
    {0x51, 81,0x28, 40,1603,   0,   0,   0,"Down Arrow"},
    {0x50, 80,0x25, 37,1604,   0,   0,   0,"Left Arrow"},
    {0x3A, 58,0x70,112,1201,   0,   0,   0,"F1"},
    {0x3B, 59,0x71,113,1202,   0,   0,   0,"F2"},
    {0x3C, 60,0x72,114,1203,   0,   0,   0,"F3"},
    {0x3D, 61,0x73,115,1204,   0,   0,   0,"F4"},
    {0x3E, 62,0x74,116,1205,   0,   0,   0,"F5"},
    {0x3F, 63,0x75,117,1206,   0,   0,   0,"F6"},
    {0x40, 64,0x76,118,1207,   0,   0,   0,"F7"},
    {0x41, 65,0x77,119,1208,   0,   0,   0,"F8"},
    {0x42, 66,0x78,120,1209,   0,   0,   0,"F9"},
    {0x43, 67,0x79,121,1210,   0,   0,   0,"F10"},
    {0x44, 68,0x7A,122,1211,   0,   0,   0,"F11"},
    {0x45, 69,0x7B,123,1212,   0,   0,   0,"F12"},
    {0x46, 70,0x2C, 44,1507,   0,   0,   0,"Print Screen"},
    {0x47, 71,0x91,145,1508,   0,   0,   0,"Scroll Lock"},
    {0x48, 72,0x13, 19,1509,   0,   0,   0,"Pause/Break"},
    {0x01,  1,0xA2, 17,1516,   0,   0,   0,"Left Ctrl"},
    {0x10, 16,0xA3, 93,1520,   0,   0,   0,"Right Ctrl"},
    {0x02,  2,0xA0, 16,1517,   0,   0,   0,"Left Shift"},
    {0x20, 32,0xA1, 94,1521,   0,   0,   0,"Right Shift"},
    {0x04,  4,0xA4, 18,1518,   0,   0,   0,"Left Alt"},
    {0x40, 64,0xA5, 95,1522,   0,   0,   0,"Right Alt"},
    {0x08,  8,0x5B, 91,1519,   0,   0,   0,"Left Win"},
    {0x80,128,0x5C, 92,1523,   0,   0,   0,"Right Win"},
    {0x53, 83,0x90,144,1312,   0,   0,   0,"Num Lock"},
    {0x54, 84,0x6F,111,1313,   0,   0,   0,"Num / (Division)"},
    {0x55, 85,0x6A,106,1314,   0,   0,   0,"Num * (Multiple)"},
    {0x56, 86,0x6D,109,1315,   0,   0,   0,"Num - (Minus)"},
    {0x57, 87,0x6B,107,1316,   0,   0,   0,"Num + (Plus)"},
    {0x58, 88,0x1C, 28,1317,   0,   0,   0,"Num Enter"},
    {0x63, 99,0x6E,110,1301,   0,   0,   0,"Num . (Dot)"},
    {0x62, 98,0x60, 96,1302,   0,   0,   0,"Num 0"},
    {0x59, 89,0x61, 97,1303,   0,   0,   0,"Num 1"},
    {0x5A, 90,0x62, 98,1304,   0,   0,   0,"Num 2"},
    {0x5B, 91,0x63, 99,1305,   0,   0,   0,"Num 3"},
    {0x5C, 92,0x64,100,1306,   0,   0,   0,"Num 4"},
    {0x5D, 93,0x65,101,1307,   0,   0,   0,"Num 5"},
    {0x5E, 94,0x66,102,1308,   0,   0,   0,"Num 6"},
    {0x5F, 95,0x67,103,1309,   0,   0,   0,"Num 7"},
    {0x60, 96,0x68,104,1310,   0,   0,   0,"Num 8"},
    {0x61, 97,0x69,105,1311,   0,   0,   0,"Num 9"},
    {0x35, 53,0x60,192,1401,   0,   0,   0,"` ~ (Back quote)"},
    {0x2D, 45,0xBD,189,1402,   0,   0,   0," -_ (Minus)"},
    {0x2E, 46,0xBB,187,1403,   0,   0,   0,"+ = (Plus)"},
    {0x2F, 47,0xDB,219,1404,   0,   0,   0,"[ { (Left square bracket)"},
    {0x30, 48,0xDD,221,1405,   0,   0,   0,"] } (Right square bracket)"},
    {0x31, 49,0xDC,220,1406,   0,   0,   0,"\\ | (Backslash)"},
    {0x33, 51,0xBA,186,1407,   0,   0,   0,"; : (Semicolon)"},
    {0x34, 52,0xDE,222,1408,   0,   0,   0,"' \" (Apostrophe)"},
    {0x36, 54,0xBC,188,1409,   0,   0,   0,", < (Comma)"},
    {0x37, 55,0xBE,190,1410,   0,   0,   0,". > (Period)"},
    {0x38, 56,0xBF,191,1411,   0,   0,   0,"/ ? (Slash)"},
    {0x64,100,0xE0,224,   0,   0,   0,   0,"Num Pad(102 Model)"}
};

// 计算数组长度
#define MAP_SIZE (sizeof(KEYBOARD_MAP) / sizeof(KEYBOARD_MAP[0]))

// 3. HID 转 VK 码
uint16_t hid_to_vk(uint8_t hid_code) {
    for (size_t i = 0; i < MAP_SIZE; i++) {
        if (KEYBOARD_MAP[i].hid0 == hid_code) {
            return KEYBOARD_MAP[i].vk0;
        }
    }
    return 0;
}

// 4. VK 转 HID 码
uint8_t vk_to_hid(uint16_t vk_code) {
    for (size_t i = 0; i < MAP_SIZE; i++) {
        if (KEYBOARD_MAP[i].vk0 == vk_code) {
            return KEYBOARD_MAP[i].hid0;
        }
    }
    return 0;
}

const char *get_keyname_vk(uint16_t vk_code)
{
    for (size_t i = 0; i < MAP_SIZE; i++) {
        if (KEYBOARD_MAP[i].vk0 == vk_code) {
            return KEYBOARD_MAP[i].name;
        }
    }
    return "";
}

const char *get_keyname_hid(uint16_t hid_code)
{
    for (size_t i = 0; i < MAP_SIZE; i++) {
        if (KEYBOARD_MAP[i].hid0 == hid_code) {
            return KEYBOARD_MAP[i].name;
        }
    }
    return "";
}

uint16_t getKeyDefineVK(uint16_t vk_code)
{
    for (size_t i = 0; i < MAP_SIZE; i++) {
        if (KEYBOARD_MAP[i].vk1 == vk_code) {
            return KEYBOARD_MAP[i].df0;
        }
    }
    qDebug() << "not found:" << vk_code;
    return 0;
}

const char *get_keyname_df(uint16_t df_code)
{
    for (size_t i = 0; i < MAP_SIZE; i++) {
        if (KEYBOARD_MAP[i].df0 == df_code) {
            return KEYBOARD_MAP[i].name;
        }
    }
    return "";
}