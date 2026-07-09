#include "ModuleEfMode.h"
#include "ui_ModuleEfMode.h"

#include <QButtonGroup>

typedef struct{
    QString name ;
    quint8 mode ;
}EfMode;

static QList<EfMode> EfModeList={

    {"常亮",0x01},
    {"呼吸",0x2},
    {"流星",0x12},
    {"涟漪",0x05},
    {"极光",0x17},
    {"雨滴",0x11},
    {"聚合",0x09},
    {"川流不息",0x07},
    {"霓虹",0x03},
    {"光波",0x04},
    {"繁星点点",0x06},
    {"如影随形",0x08},
    {"正弦光波",0x0A},
    {"彩泉涌动",0x0B},
    {"百花争艳",0x0C},
    {"一石二鸟",0x0E},
    {"峰回路转",0x0F},  // 自定义色彩
    {"斜风细雨",0x10},
    {"踏雪无痕",0x13},
    {"层出不穷",0x18},
    {"光影",0x15},     // 持续发送音律 0E --------
    {"音乐律动",0x16}, // 持续发送音律 0D --------  炫彩 0
    {"自定义",0x0D}    // 选图案
};


ModuleEfMode::ModuleEfMode(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::ModuleEfMode)
{
    ui->setupUi(this);

    {
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
        pBtnGrp = new QButtonGroup(this) ;
        QLayout *pLayout = ui->scrollAreaWidgetContents->layout();
        pLayout->setSpacing(8);
        pLayout->setContentsMargins(0,0,0,0);

        for(int i=0; i<EfModeList.count(); i++)
        {
            QString strName=QString::asprintf("%s (%02X)",EfModeList[i].name.toStdString().c_str(),EfModeList[i].mode) ;
            QPushButton *btn = new QPushButton(strName,this);
            btn->setFixedSize(250,28);
            btn->setCheckable(true);
            btn->setStyleSheet(strStyle);
            btn->setCursor(Qt::PointingHandCursor);

            pBtnGrp->addButton(btn,EfModeList[i].mode);
            pLayout->addWidget(btn);
            if(i==0)
                btn->click();
            btn->setFocusPolicy(Qt::NoFocus);
        }

        connect(pBtnGrp,&QButtonGroup::idClicked,this,[=](int id){

// LED Param Table:
// Effect                   Speed	Brightness	Color (Option Low 4bit) 	Type (Option High4bit)
// OFF              0x00	xxx     xxx         xxx                         xxx
// Static           0x01	xxx     0~4         Single-color/Full Color/AP	xxx
// Breathe          0x02	0~4     0~4         Single-color/Full Color/AP	xxx
// Neon             0x03	0~4     0~4         xxx                         xxx
// Wave             0x04	0~4     0~4         Single-color/Full Color/AP	0:Right,1:Left,2:Down,3:Up
// Ripple       	0x05	0~4     0~4         Single-color/Full Color/AP	xxx
// Raindrop     	0x06	0~4     0~4         Single-color/Full Color/AP	xxx
// Snake            0x07	0~4     0~4         Single-color/Full Color/AP	0: Default,1: Clip
// PressAction On	0x08	0~4     0~4         Single-color/Full Color/AP	xxx
// Converge         0x09	0~4     0~4         Single-color/Full Color/AP	xxx
// Sine Wave        0x0A	0~4     0~4         Single-color/Full Color/AP	xxx
// kaleidoscope     0x0B	0~4     0~4         Single-color/Full Color/AP	0:Out,1:In
// Line Wave        0x0C	0~4     0~4         Single-color/Full Color/AP	0: Right,1: Left
// User picture     0x0D	xxx     0~4         Single-color/Full Color/AP	Index (0-4)
// Laser            0x0E	0~4     0~4         Single-color/Full Color/AP	xxx
// Circle Wave      0x0F	0~4     0~4         Single-color/Full Color/AP	0: clockwise,1: anticlockwise
// Dazzing          0x10	0~4     0~4         Single-color/Full Color/AP	xxx
// Rain Down        0x11	0~4     0~4         Single-color/Full Color/AP	xxx
// Meteor           0x12	0~4     0~4         Single-color/Full Color/AP	xxx
// PressAction Off	0x13	0~4     0~4         Single-color/Full Color/AP	xxx
// Music 1          0x14	xxx     0~4         Single-color/Full Color/AP	0：up1：separate2：intersect
// Windows          0x15	xxx     0~4         Single-color/Full Color/AP	xxx
// Music 2          0x16	xxx     0~4         Single-color/Full Color/AP	0：up1：separate2：intersect
// Train            0x17	0~4     0~4         Single-color/Full Color/AP	xxx
// Fireworks        0x18	0~4     0~4         Single-color/Full Color/AP	xxx
// Gif              0x19
// PS:
// Color: 0-6=Single-color: 0xFF,0x00,0x00 red.
//                          0xFF,0x80,0x00 orange.
//                          0xFF,0xFF,0x00 yellow.
//                          0x00,0xFF,0x00 green.
//                          0x00,0xFF,0xFF blueness.
//                          0x00,0x00,0xFF blue.
//                          0xFF,0x00,0xFF purple.
// 7 = AP color: define by AP (Default 0xFF,0xFF,0xFF).
// 8 = Full color: RGB Mode

            if(!m_bOutSet) emit onModeChanged(id,0);
            m_mode = id;
            m_bOutSet  = false;
            bool hide = true;
            ui->comboBoxPic->blockSignals(true);
            QStringList addOption;
            switch(id)
            {
            case 0x04:
                addOption.push_back(tr("左"));
                addOption.push_back(tr("右"));
                addOption.push_back(tr("上"));
                addOption.push_back(tr("下"));
                hide = false;
                break;

            case 0x07:
                addOption.push_back(tr("Z形"));
                addOption.push_back(tr("回形"));
                hide = false;
                break;

            case 0x0B:
                addOption.push_back(tr("向内"));
                addOption.push_back(tr("向外"));
                hide = false;
                break;

            case 0x0C:
                addOption.push_back(tr("向右"));
                addOption.push_back(tr("向左"));
                hide = false;
                break;

            case 0x0D:
                addOption.push_back(tr("图案1"));
                addOption.push_back(tr("图案2"));
                addOption.push_back(tr("图案3"));
                addOption.push_back(tr("图案4"));
                addOption.push_back(tr("图案5"));
                hide = false;
                break;

            case 0x0F:
                addOption.push_back(tr("顺时针"));
                addOption.push_back(tr("逆时针"));
                hide = false;
                break;
            }

            if(hide) addOption.push_back(tr("无"));
            ui->comboBoxPic->clear();
            ui->comboBoxPic->addItems(addOption);
            ui->comboBoxPic->setDisabled(hide);

            ui->comboBoxPic->blockSignals(false);
        });

        connect(ui->checkBoxEFMode,&QCheckBox::clicked,this,[=](bool checked){ emit onModeChanged(-1,checked); });
        connect(ui->comboBoxPic,&QComboBox::currentIndexChanged,this,[=](int index){ emit onModeChanged(m_mode,index); });
    }
}

ModuleEfMode::~ModuleEfMode()
{
    delete ui;
}

void ModuleEfMode::setEfMode(int mode)
{
    m_bOutSet = true;
    if(pBtnGrp->button(mode))
        pBtnGrp->button(mode)->click();
}
