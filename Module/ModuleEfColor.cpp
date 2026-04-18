#include "ModuleEfColor.h"
#include "ui_ModuleEfColor.h"
#include "ColorSlider.h"
#include "ColorSquare.h"

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
}

ModuleEfColor::~ModuleEfColor()
{
    delete ui;
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
