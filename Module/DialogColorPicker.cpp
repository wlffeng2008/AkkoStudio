#include "DialogColorPicker.h"
#include "ui_DialogColorPicker.h"

#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QScreen>
#include <Windows.h>

DialogColorPicker::DialogColorPicker(QWidget *parent)
    : QDialog(nullptr)
    , ui(new Ui::DialogColorPicker)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::Tool | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);

    m_picker = new QDialog();
    m_picker->setWindowFlags(Qt::Dialog | Qt::Tool | Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);
    m_picker->setFixedSize(64,64);
    m_picker->setAttribute(Qt::WA_TranslucentBackground);
    m_picker->setWindowOpacity(0);
    ui->labelPick->installEventFilter(this);
    m_picker->installEventFilter(this);

    //qApp->installEventFilter(this);

    ColorSquare *pCSq = ui->framePad;
    ColorSlider *pCSl = ui->horizontalSlider;
    pCSl->setRange(0,360);

    connect(pCSl,&ColorSlider::valueChanged,this,[=](int value){
        pCSq->setHue(value/360.0);
    });

    connect(pCSq,&ColorSquare::colorSelected,this,[=](QColor color){
        m_color = color;
        ui->lineEditR->setText(QString::asprintf("%d",color.red()));
        ui->lineEditG->setText(QString::asprintf("%d",color.green()));
        ui->lineEditB->setText(QString::asprintf("%d",color.blue()));
        refreshColor();
    });

    connect(ui->lineEditR,&QLineEdit::textEdited,this,[=](const QString&text){
        m_color.setRed(text.toInt());
        refreshColor();
    });
    connect(ui->lineEditG,&QLineEdit::textEdited,this,[=](const QString&text){
        m_color.setGreen(text.toInt());
        refreshColor();
    });
    connect(ui->lineEditB,&QLineEdit::textEdited,this,[=](const QString&text){
        m_color.setBlue(text.toInt());
        refreshColor();
    });

    connect(ui->pushButton,&QPushButton::clicked,this,[=]{
        hide();
        m_picker->hide();
    });


    QTimer *pTimer = new QTimer(this);
    pTimer->start(10);
    connect(pTimer,&QTimer::timeout,this,[=]{
        if(this->isHidden()) m_picker->hide();
        if(!m_picker->isVisible()) return;
        QPoint cursorPos = QCursor::pos();
        static QPoint lastPos;
        if(lastPos == cursorPos)
            return;

        lastPos = cursorPos;

        int x = cursorPos.x() - m_picker->width()/2;
        int y = cursorPos.y() - m_picker->width()/2;

        QScreen* primaryScreen = QGuiApplication::primaryScreen();
        m_image = primaryScreen->grabWindow(0,  cursorPos.x(), cursorPos.y(), 1,1).toImage();

        ui->framePad->colorSelected(m_image.pixelColor(0,0));
        m_picker->move(x,y);
        m_picker->update();
    });
}

void DialogColorPicker::refreshColor()
{
    QColor color = m_color;
    emit onPickupColor(color);

    m_HexColor = QString::asprintf("#%02X%02X%02X",color.blue(),color.green(),color.red());
    ui->lineEditHex->setText(m_HexColor);
    QString strSheet=QString::asprintf("background-color:rgb(%d,%d,%d); border-radius: 15px;",color.red(),color.green(),color.blue());
    ui->labelColor->setStyleSheet(strSheet);

    emit onPickupColor(color);
}

DialogColorPicker::~DialogColorPicker()
{
    delete ui;
}

void DialogColorPicker::showPostion(int x, int y)
{
    QPoint p(x,y) ;
    if(x == -1 || y == -1)
    {
        p = QCursor::pos();
        p.setX(p.x() - size().width()/2);
    }
    showPoint(p);
}

void DialogColorPicker::showPoint(const QPoint&point)
{
    QRect rect(point,size());
    setGeometry(rect);
    show();
}

void DialogColorPicker::showEvent(QShowEvent*event)
{
    QTimer::singleShot(200,this,[=]{
        raise();
        activateWindow();
        setFocus();
    });
    QDialog::showEvent(event);
}

bool DialogColorPicker::event(QEvent *event)
{
    if(event->type() == QEvent::WindowDeactivate)
    {
        if(!m_picker->isVisible())
            hide();
        QTimer::singleShot(100,this,[=]{
        });
    }

    return QDialog::event(event);
}

bool DialogColorPicker::eventFilter(QObject *watched,QEvent *event)
{

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            qDebug() << "全局鼠标左键按下，坐标：" << watched << mouseEvent->globalPos();
            if(watched == ui->labelPick)
            {
                m_picker->show();

                QTimer::singleShot(100,this,[=]{
                    static QPixmap pix(":/images/pickup2.png");  // 资源文件 或 路径都可以

                    // 2. 缩放（可选）
                    pix = pix.scaled(32, 32);

                    // 3. 创建自定义光标，第二个参数是热点（点击点）
                    static QCursor customCursor(pix, 0, 0);

                    // 4. 设置给整个窗口
                    m_picker->setCursor(customCursor);
                });

            }

            if(watched == m_picker)
            {
                m_picker->hide();
            }
        }
        else if (mouseEvent->button() == Qt::RightButton)
        {
            qDebug() << "全局鼠标右键按下，坐标：" << mouseEvent->globalPos();
        }
    }

    if (event->type() == QEvent::MouseButtonRelease)
    {
        m_picker->hide();
    }

    if(event->type() == QEvent::Paint && watched == m_picker)
    {
        QPainter painter(m_picker);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHints(QPainter::SmoothPixmapTransform|QPainter::Antialiasing);

        int dist = m_picker->size().height();

        QPainterPath path;
        path.addEllipse(QRect(0,0,dist,dist));
        painter.setClipPath(path);

        if(!m_image.isNull())
        {
            painter.drawImage(m_picker->rect(),m_image);
        }
        else
        {
            painter.setBrush(Qt::red);
            painter.drawEllipse(m_picker->rect());
        }
        painter.drawRoundedRect(m_picker->rect(),32,32);
        for(int i=0; i<=8; i++)
        {
            //painter.drawLine(QPoint(i*8,0),QPoint(i*8,64));
            //painter.drawLine(QPoint(0,i*8),QPoint(64,i*8));
        }
    }

    return QDialog::eventFilter(watched,event);
}

void DialogColorPicker::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawRect(this->rect().adjusted(0,0,-1,-1));

    QDialog::paintEvent(event);
}