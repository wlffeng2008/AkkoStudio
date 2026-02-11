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
    {"正弦波",0x0A},
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
                width: 240px;
                height: 28px;
                border-radius: 14px;
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
            )") ;
        pBtnGrp = new QButtonGroup(this) ;
        QLayout *pLayout = ui->scrollAreaWidgetContents->layout();
        pLayout->setSpacing(8);
        pLayout->setContentsMargins(0,0,0,0);

        for(int i=0; i<EfModeList.count(); i++)
        {
            QString strName=QString::asprintf("%s (%02X)",EfModeList[i].name.toStdString().c_str(),EfModeList[i].mode) ;
            QPushButton *btn = new QPushButton(strName,this);
            btn->setFixedSize(240,28);
            btn->setCheckable(true);
            btn->setStyleSheet(strStyle);

            pBtnGrp->addButton(btn,EfModeList[i].mode);
            pLayout->addWidget(btn);
            if(i==0)
                btn->click();
            btn->setFocusPolicy(Qt::NoFocus);
        }

        connect(pBtnGrp,&QButtonGroup::idClicked,this,[=](int id){
            if(!m_bOutSet)
                emit onModeChanged(id);
            m_bOutSet = false;
            bool hide=true;
            ui->comboBoxPic->blockSignals(true);
            QStringList addOption;
            switch(id)
            {
            case 0x04:
                addOption.push_back(tr(""));
                hide = false;
                break;

            }

            ui->comboBoxPic->setHidden(hide);
            ui->labelPicture->setHidden(hide);

            ui->comboBoxPic->blockSignals(false);
        });

        connect(ui->checkBoxEFMode,&QCheckBox::clicked,this,[=](bool checked){ emit onModeChanged(0,checked); });
        connect(ui->comboBoxPic,&QComboBox::currentIndexChanged,this,[=](int index){ emit onModePicture(index); });
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
