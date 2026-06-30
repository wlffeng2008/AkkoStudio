#include "ModuleEfColor.h"
#include "ui_ModuleEfColor.h"
#include "ColorSlider.h"
#include "ColorSquare.h"
#include "DialogColorPicker.h"

#include <QPainter>
#include <QColorDialog>

static QVector<QColor> col_list;

ModuleEfColor::ModuleEfColor(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleEfColor)
{
    ui->setupUi(this);
    ui->tabWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->tabWidget->setStyleSheet("QWidget{background-color: rgba(0, 0, 0, 0.0);}");

    connect(ui->tabWidget,&QTabWidget::currentChanged,this,[=](int index){
        update();
    });

    ui->tabWidget->setCurrentIndex(0);

    {
        m_pModel = new QStandardItemModel(this);
        m_pModel->setHorizontalHeaderLabels(QString("0,0,0,0,0,0,0").split(','));
        ui->tableView->setModel(m_pModel);
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView->setShowGrid(false);

        ColorDelegate *pDele = new ColorDelegate(this);
        pDele->setTableView(ui->tableView);
        pDele->setImage(":/images/Color-picked.png");
        ui->tableView->setItemDelegateForRow(0,pDele);
        ui->tableView->setStyleSheet(R"(
            QTableView{ border: none; background-color: transparent; font: 400 9px "宋体"; }
            QTableView::Item::selected{ background-color: transparent; }
        )") ;
        m_pModel->appendRow({
            new QStandardItem("1"),
            new QStandardItem("1"),
            new QStandardItem("1"),
            new QStandardItem("1"),
            new QStandardItem("1"),
            new QStandardItem("1"),
            new QStandardItem("1")
        }) ;
        QFont font = ui->tableView->font();
        font.setPointSize(24);
        srand(time(nullptr)) ;

        col_list.push_back(0xFF0000);
        col_list.push_back(0x00FF00);
        col_list.push_back(0x0000FF);
        col_list.push_back(0xFFFF00);
        col_list.push_back(0xFF00FF);
        col_list.push_back(0xFF8000);
        col_list.push_back(0x00FFFF);

        for(int i=0; i<7; i++)
        {
            QStandardItem *item = m_pModel->item(0,i);
            item->setTextAlignment(Qt::AlignBottom|Qt::AlignHCenter);
            item->setText("●");
            item->setFont(font);
            item->setEditable(false);
            item->setForeground(QBrush(col_list[i]));
        }
        connect(pDele,&ColorDelegate::onClicked,this,[=](int index){
            static quint64 s_pos = 700000000;
            s_pos += (index - 3);
            for(int i=0; i<7; i++)
            {
                QStandardItem *item = m_pModel->item(0,i);
                item->setForeground(QBrush(col_list[(i+s_pos)%7]));
            }
            ui->tableView->update();

            QColor selColor = m_pModel->item(0,3)->data(Qt::ForegroundRole).value<QBrush>().color();
            for(int i=0; i<7; i++)
            {
                if(selColor == col_list[i])
                {
                    qDebug() << selColor << i;
                    emit onSetColor(selColor,i);
                    break;
                }
            }
        });
    }

    //ui->labelAddColor->installEventFilter(this);
    //ui->labelAddColor->hide();

    ColorSquare *pCSq = new ColorSquare(this);
    ColorSlider *pCSl = new ColorSlider(this);
    pCSl->setRange(0,359);

    pCSq->setFixedSize(300,200);
    pCSl->setFixedWidth(300);
    ui->verticalLayout1->addWidget(pCSq,0,Qt::AlignCenter);
    ui->verticalLayout2->addWidget(pCSl,0,Qt::AlignCenter);

    ui->verticalLayout1->setAlignment(Qt::AlignCenter);

    connect(pCSl,&ColorSlider::valueChanged,this,[=](int value){
        pCSq->setHue(value/360.0);
    });

    connect(pCSq,&ColorSquare::colorSelected,this,[=](QColor color){
        ColorLabel *pLab = ColorLabel::Current(ui->tab2);
        if(pLab) pLab->setColor(color);

        emit onSetColor(color,8);
    });

    ui->checkBoxSingle->setHidden(true);
    ui->labelTitleL1->setHidden(true);
    ui->tabWidget->setTabVisible(2,false);

    QString strStyle(R"(

            QPushButton {
                border-radius: 14px;
                font-size: 14px;
                font-weight:500;
                outline: none;

                color: #333;
                border: 1px solid #ECECEC;
                background: #ECECEC; }

            QPushButton:checked {
                color: white;
                border: 1px solid #6329B6;
                background: #6329B6; }
            QPushButton:hover { border: 1px solid #6329B6; }
            )");
    QList<QAbstractButton*> btns = ui->buttonGroupE->buttons();
    btns.append(ui->pushButtonCol0);
    btns.append(ui->pushButtonCol1);
    foreach (QAbstractButton *btn, btns) {
        btn->setStyleSheet(strStyle);
        btn->setFixedHeight(28);
    }

    {
        ui->labelPickup->installEventFilter(this);

        m_clrDlg = new DialogColorPicker();

        QTimer *pTMSetColor = new QTimer(this);
        pTMSetColor->setInterval(200);
        connect(m_clrDlg,&DialogColorPicker::onPickupColor,this,[=](const QColor&color){
            m_sideClr = color;
            pTMSetColor->stop();
            pTMSetColor->start();
        });

        connect(pTMSetColor,&QTimer::timeout,this,[=]{
            pTMSetColor->stop();
            setSideLed();
        });

        connect(ui->buttonGroupE,&QButtonGroup::idClicked,this,[=](int id){
            m_sideE = abs(id) - 2;
            setSideLed();
        });
        connect(ui->buttonGroupC,&QButtonGroup::idClicked,this,[=](int id){
            m_sideC = abs(id) - 2;
            setSideLed();
            ui->labelColor->setHidden(m_sideC == 0);
            ui->labelPickup->setHidden(m_sideC == 0);
        });
        connect(ui->buttonGroupL,&QButtonGroup::idClicked,this,[=](int id){
            m_sideL = abs(id) - 2;
            setSideLed();
        });
        connect(ui->buttonGroupS,&QButtonGroup::idClicked,this,[=](int id){
            m_sideS = abs(id) - 2;
            setSideLed();
        });
    }
}

ModuleEfColor::~ModuleEfColor()
{
    delete ui;
}

void ModuleEfColor::updateData(const QByteArray &data)
{
    qDebug().noquote() << "get_:" << data.left(16).toHex(' ').toUpper();
    m_bUpdate = true;

    m_sideClr.setRed((quint8)data[5]);
    m_sideClr.setGreen((quint8)data[6]);
    m_sideClr.setBlue((quint8)data[7]);

    m_sideE=data[1];
    ui->buttonGroupE->buttons()[data[1]]->click();

    m_sideS=data[2];
    ui->buttonGroupS->buttons()[data[2]]->click();

    m_sideL=data[3];
    ui->buttonGroupL->buttons()[data[3]]->click();

    m_sideC=data[4];
    ui->buttonGroupC->buttons()[data[4] == 0x08]->click();

    m_bUpdate = false;
}

void ModuleEfColor::setSideLed()
{
    QByteArray data(10,0);
    QColor color = m_sideClr;

    data[1] = m_sideE;
    data[2] = m_sideS;
    data[3] = m_sideL;
    data[4] = (m_sideC == 0 ? 0x07 : 0x08);
    data[5] = color.red();
    data[6] = color.green();
    data[7] = color.blue();

    QString strSheet=QString::asprintf("background-color: rgb(%d, %d, %d);border: none; border-radius:16px;",color.red(),color.green(),color.blue());
    ui->labelColor->setStyleSheet(strSheet);

    if(!m_bUpdate) emit onSetSideLed(data);
}

void ModuleEfColor::paintEvent(QPaintEvent *event)
{
    if(ui->tabWidget->currentIndex() == 0)
    {

        static QPixmap map(":/images/light/light.png");
        QPainter painter(this);
        painter.drawPixmap(this->rect(),map);
    }

    QFrame::paintEvent(event);
}

bool ModuleEfColor::eventFilter(QObject*watched ,QEvent *event)
{
    if(watched == ui->labelPickup && event->type() == QEvent::MouseButtonRelease)
    {
        m_clrDlg->showPostion();
    }
    // if(watched == ui->labelAddColor && event->type() == QEvent::MouseButtonRelease)
    // {
    //     // QColor color = QColorDialog::getColor(Qt::red, nullptr, "选择颜色");
    //     // if(color.isValid())
    //     // {
    //     //     qDebug() << "get Color ......" ;
    //     // }

    //     QColorDialog dialog(nullptr);
    //     dialog.setStyleSheet("QPushButton{ border: 1px solid gray; font: none;background: transparent; color: black; border-radius: 0px;}");
    //     dialog.setWindowTitle("恢复默认样式的颜色对话框");
    //     dialog.setCurrentColor(Qt::blue);
    //     if (dialog.exec() == QColorDialog::Accepted) {
    //         QColor color = dialog.selectedColor();
    //         qDebug() << color;
    //     }
    // }
    return QFrame::eventFilter(watched,event);
}
