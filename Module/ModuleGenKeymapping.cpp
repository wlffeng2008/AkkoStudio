#include "ModuleGenKeymapping.h"
#include "qcontainerfwd.h"
#include "qtypes.h"
#include <QDebug>

static ModuleGenKeymapping *s_map = nullptr ;

ModuleGenKeymapping::ModuleGenKeymapping(QWidget *parent):QWidget(parent)
{
    s_map = this;
    initList();
    hide() ;
}

void ModuleGenKeymapping::initList()
{
    m_fnList.clear();
    m_muList.clear();

    m_fnList.push_back(new keyItem({0x0a, 0x01, 0x00, 0x00}, tr("L-Fn键"    ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0xb6, 0x00}, tr("上一曲"    ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0xb5, 0x00}, tr("下一曲"    ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0xcd, 0x00}, tr("播放/暂停" ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0xe2, 0x00}, tr("静音"      ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0xe9, 0x00}, tr("音量+"     ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0xea, 0x00}, tr("音量-"     ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0x83, 0x01}, tr("播放器"    ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0x92, 0x01}, tr("计算器"    ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0x8a, 0x01}, tr("邮件"      ))) ; //09

    m_fnList.push_back(new keyItem({0x03, 0x00, 0x94, 0x01}, tr("我的电脑"  ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0x21, 0x02}, tr("搜索"      ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0x23, 0x02}, tr("主页"      ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0x27, 0x02}, tr("刷新"      ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0x6f, 0x00}, tr("亮度+"     ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0x70, 0x00}, tr("亮度-"     ))) ;
    m_fnList.push_back(new keyItem({0x00, 0x00, 0xe3, 0x2e}, tr("放大"      ))) ;  
    m_fnList.push_back(new keyItem({0x00, 0x00, 0xe3, 0x2d}, tr("缩小"      ))) ;
    m_fnList.push_back(new keyItem({0x12, 0x00, 0xe3, 0x2c}, tr("呼出Siri"  ))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0xb7, 0x00}, tr("停止"      ))) ; //19

    m_fnList.push_back(new keyItem({0x00, 0xe0, 0x2c, 0x00}, tr("切换输入法"))) ;
    m_fnList.push_back(new keyItem({0x03, 0x00, 0x24, 0x02}, tr("返回"      ))) ;
    m_fnList.push_back(new keyItem({0x0a, 0x0d, 0x00, 0x00}, tr("Fn键锁屏"  ))) ;
    m_fnList.push_back(new keyItem({0x00, 0x00, 0xe3, 0x0f}, tr("锁屏"      ))) ;
    m_fnList.push_back(new keyItem({0x0a, 0x01, 0x01, 0x00}, tr("R-Fn键"    ))) ;
    m_fnList.push_back(new keyItem({0x14, 0x00, 0x00, 0x00}, tr("DPI-Loop"  ))) ;
    m_fnList.push_back(new keyItem({0x14, 0x00, 0x01, 0x00}, tr("DPI+"      ))) ;
    m_fnList.push_back(new keyItem({0x14, 0x00, 0x02, 0x00}, tr("DPI-"      ))) ;
    m_fnList.push_back(new keyItem({0x14, 0x00, 0x04, 0x00}, tr("DPI-Shift" ))) ; //29

    m_fnList.push_back(new keyItem({0x08, 0x00, 0x00, 0x00}, tr("Profile-Value")));
    m_fnList.push_back(new keyItem({0x08, 0x00, 0x01, 0x00}, tr("Profile+"     )));
    m_fnList.push_back(new keyItem({0x08, 0x00, 0x02, 0x00}, tr("Profile-"     )));
    m_fnList.push_back(new keyItem({0x08, 0x00, 0x03, 0x00}, tr("Profile-Loop" )));
    m_fnList.push_back(new keyItem({0x08, 0x00, 0x04, 0x00}, tr("Profile-1"    )));
    m_fnList.push_back(new keyItem({0x08, 0x00, 0x04, 0x01}, tr("Profile-2"    )));
    m_fnList.push_back(new keyItem({0x08, 0x00, 0x04, 0x02}, tr("Profile-3"    )));
    m_fnList.push_back(new keyItem({0x08, 0x00, 0x04, 0x03}, tr("Profile-4"    )));
    m_fnList.push_back(new keyItem({0x08, 0x00, 0x04, 0x04}, tr("Profile-5"    ))); // 39

    m_fnList.push_back(new keyItem({0x0a, 0x05, 0x00, 0x00}, tr("切换Win"     )));
    m_fnList.push_back(new keyItem({0x0a, 0x05, 0x01, 0x00}, tr("切换Mac"     )));
    m_fnList.push_back(new keyItem({0x0d, 0x03, 0x01, 0x00}, tr("背光速度-"   )));
    m_fnList.push_back(new keyItem({0x0d, 0x03, 0x02, 0x00}, tr("背光速度+"   )));
    m_fnList.push_back(new keyItem({0x0d, 0x02, 0x01, 0x00}, tr("背光亮度+"   )));
    m_fnList.push_back(new keyItem({0x0d, 0x02, 0x02, 0x00}, tr("背光亮度-"   )));
    m_fnList.push_back(new keyItem({0x0d, 0x01, 0x00, 0x00}, tr("灯效循环切换")));
    m_fnList.push_back(new keyItem({0x0d, 0x05, 0x01, 0x00}, tr("背光颜色切换")));
    m_fnList.push_back(new keyItem({0x0a, 0x0c, 0x00, 0x00}, tr("关闭背光"    )));
    m_fnList.push_back(new keyItem({0x0a, 0x02, 0x00, 0x00}, tr("恢复出厂设置")));
    m_fnList.push_back(new keyItem({0x0a, 0x03, 0x00, 0x00}, tr("锁Win"       )));
    m_fnList.push_back(new keyItem({0x0a, 0x05, 0x03, 0x00}, tr("系统切换"    ))); // 51

    // ---------------------------------------------------------------------------
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf0, 0x00}, tr("左键单击"    )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf1, 0x00}, tr("右键单击"    )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf2, 0x00}, tr("中键按下"    )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf3, 0x00}, tr("光标前进"    )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf4, 0x00}, tr("光标后退"    )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf5, 0x00}, tr("滚轮-向左"   )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf6, 0x00}, tr("滚轮-向右"   )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf7, 0x00}, tr("滚轮-向上"   )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf8, 0x00}, tr("滚轮-向下"   )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf5, 0x01}, tr("向上滚屏"    )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf5, 0xff}, tr("向下滚屏"    )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf6, 0xfb}, tr("XUP"         )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf6, 0x05}, tr("XDOWN"       )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf7, 0xfb}, tr("YUP"         )));
    m_muList.push_back(new keyItem({0x01, 0x00, 0xf7, 0x05}, tr("YDOWN"       )));
}

ModuleGenKeymapping::~ModuleGenKeymapping()
{

}

keyData* ModuleGenKeymapping::getMuData(quint8 index)
{
    if(index >= m_muList.count())
        return nullptr;
    return &m_muList[index]->data;
}

keyData* ModuleGenKeymapping::getFnData(quint8 index)
{
    if(index >= m_fnList.count())
        return nullptr;
    return &m_fnList[index]->data;
}

static quint8 defaultMatrix_id2807[] =
{
    0,0,41,0,
    0,0,53,0,
    0,0,43,0,
    0,0,57,0,
    0,0,225,0,
    0,0,224,0,
    0,0,58,0,
    0,0,30,0,
    0,0,20,0,
    0,0,4,0,  // 9

    0,0,0,0,
    0,0,227,0,
    0,0,59,0,
    0,0,31,0,
    0,0,26,0,
    0,0,22,0,
    0,0,29,0,
    0,0,226,0,
    0,0,60,0,
    0,0,32,0, // 19

    0,0,8,0,
    0,0,7,0,
    0,0,27,0,
    0,0,0,0,
    0,0,61,0,
    0,0,33,0,
    0,0,21,0,
    0,0,9,0,
    0,0,6,0,
    0,0,0,0,  // 29

    0,0,62,0,
    0,0,34,0,
    0,0,23,0,
    0,0,10,0,
    0,0,25,0,
    0,0,0,0,
    0,0,63,0,
    0,0,35,0,
    0,0,28,0,
    0,0,11,0, // 39

    0,0,5,0,
    0,0,44,0,
    0,0,64,0,
    0,0,36,0,
    0,0,24,0,
    0,0,13,0,
    0,0,17,0,
    0,0,0,0,
    0,0,65,0,
    0,0,37,0, // 49

    0,0,12,0,
    0,0,14,0,
    0,0,16,0,
    0,0,0,0,
    0,0,66,0,
    0,0,38,0,
    0,0,18,0,
    0,0,15,0,
    0,0,54,0,
    0,0,230,0,// 59

    0,0,67,0,
    0,0,39,0,
    0,0,19,0,
    0,0,51,0,
    0,0,55,0,
    10,1,0,0, // 65 R-FN;
    0,0,68,0,
    0,0,45,0,
    0,0,47,0,
    0,0,52,0,// 69

    0,0,56,0,
    0,0,228,0,
    0,0,69,0,
    0,0,46,0,
    0,0,48,0,
    0,0,0,0,
    0,0,229,0,
    0,0,80,0,
    0,0,76,0,
    0,0,42,0, // 79

    0,0,49,0,
    0,0,40,0,
    0,0,82,0,
    0,0,81,0,
    0,0,0,0,
    0,0,74,0,
    0,0,75,0,
    0,0,78,0,
    0,0,77,0,
    0,0,79,0, // 89

    3,0,234,0,  // 音量+  90
    3,0,233,0,  // 音量-  91
    10,14,0,0,  // L-Fn键   92   -- 240
    0,0,0,0,    // 93
    0,0,0,0,    // 94
    0,0,0,0,    // 95
    13,2,2,0,   // 背光亮度- 96 -- 241
    13,2,1,0,   // 背光亮度+ 97 -- 242
    0,0,0,0,
    0,0,0,0, // 99

    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0, // 109

    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0, // 119

    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0   // 127 line
};

static QByteArray matrixData((char *)defaultMatrix_id2807,512);

QByteArray getDefaultMatrix()
{
    return matrixData;
}

void setDefaultMatrix(const QByteArray&data)
{
    matrixData = data;
}

quint8 getIndex(quint8 hid)
{
    switch(hid)
    {
    case 250: return 65;
    case 251: return 92;
    case 252: return 96;
    case 253: return 97;
    }

    for(int i=0; i<512; i += 4)
    {
        if(matrixData[i+2] == hid)
        {
            return i/4;
        }
    }
    return 0x00;
}

quint8 getHid(quint8 index)
{
    switch(index)
    {
    case 65: return 250;
    case 92: return 251;
    case 96: return 252;
    case 97: return 253;
    }

    if(index<128)
        return matrixData[index*4 + 2];
    return 0;
}

// --------------------------------------------------------------------------

struct keyMapItem{
    QString value;
    quint16 hid;
    quint16 code=0;
};

static QList<keyMapItem> s_keMapTable=
{
    { "` ~",          53,    41 },
    { "1 !",          30,     2 },
    { "2 @",          31,     3 },
    { "3 #",          32,     4 },
    { "4 $",          33,     5 },
    { "5 %",          34,     6 },
    { "6 ^",          35,     7 },
    { "7 &",          36,     8 },
    { "8 *",          37,     9 },
    { "9 (",          38,    10 },
    { "0 )",          39,    11 },
    { "- _",          45,    12 },
    { "= +",          46,    13 },
    { "_¥",          137,  0x00 },
    { "BackSpace",    42,    14 },

    { "Tab",          43,    15 },
    { "Q",            20,    16 },
    { "W",            26,    17 },
    { "E",             8,    18 },
    { "R",            21,    19 },
    { "T",            23,    20 },
    { "Y",            28,    21 },
    { "U",            24,    22 },
    { "I",            12,    23 },
    { "O",            18,    24 },
    { "P",            19,    25 },
    { "[ {",          47,    26 },
    { "] }" ,         48,    27 },
    { "\\ |",         49,    43 },

    { "Caps-Lock",    57,    58 },
    { "A",             4,    30 },
    { "S",            22,    31 },
    { "D",             7,    32 },
    { "F",             9,    33 },
    { "G",            10,    34 },
    { "H",            11,    35 },
    { "J",            13,    36 },
    { "K",            14,    37 },
    { "L",            15,    38 },
    { "; :",          51,    39 },
    { "\' \"",        52,    40 },
    { "_]",           50,  0x00 },
    { "Enter",        40,    28 },

    { "L-Shift",     225,    42 },
    { "L-\\",        100,    53 },
    { "Z",            29,    44 },
    { "X",            27,    45 },
    { "C",             6,    46 },
    { "V",            25,    47 },
    { "B",             5,    48 },
    { "N",            17,    49 },
    { "M",            16,    50 },
    { ", <",          54,    51 },
    { ". >",          55,    52 },
    { "/ ?",          56,    53 },
    { "R-\\"   ,     135,    43 },
    { "R-Shift",     229,    54 },

    { "L-Ctrl",      224,    29 },
    { "L-Alt",       226,    56 },
    { "L-Win",       227, 57435 },
    { "R-Win",       231, 57436 },
    { "Menu",        101, 57437 },
    { "Space",        44,    57 },
    { "R-Alt",       230, 57400 },
    { "R-Ctrl",      228, 57373 },

    { "Insert",       73, 57426 },
    { "Delete",       76, 57427 },
    { "←"    ,       80, 57419 },
    { "Home",         74, 57415 },
    { "End",          77, 57423 },
    { "↑",           82, 57416 },
    { "↓",           81, 57424 },
    { "Pgup",         75, 57417 },
    { "Pgdn",         78, 57425 },
    { "→",           79, 57421 },

    { "Num-Lock",     83, 57413 },
    { "Num-7",        95,    71 },
    { "Num-Home",     74,    71 },
    { "Num-4",        92,    75 },
    { "Num-←",       80,    75 },
    { "Num-1",        89,    79 },
    { "Num-End",      77,    79 },
    { "Num-/",        84, 57397 },
    { "Num-8",        96,    72 },
    { "Num-↑",       82,    72 },
    { "Num-5",        93,    76 },
    { "Num-2",        90,    80 },
    { "Num-↓",       81,    80 },
    { "Num-0",        98,    82 },
    { "Num-*",        85,    55 },
    { "Num-9",        97,    73 },
    { "Num-Pgup",     75,    73 },
    { "Num-6",        94,    77 },
    { "Num-→",       79,    77 },
    { "Num-3",        91,    81 },
    { "Num-Pgdn",     78,    82 },
    { "Num-.",        99,    83 },
    { "Num--",        86,    74 },
    { "Num-+",        87,    78 },
    { "Num-Enter",    88, 57372 },

    { "Esc",          41,     1 },
    { "F1",           58,    59 },
    { "F2",           59,    60 },
    { "F3",           60,    61 },
    { "F4",           61,    62 },
    { "F5",           62,    63 },
    { "F6",           63,    64 },
    { "F7",           64,    65 },
    { "F8",           65,    66 },
    { "F9",           66,    67 },
    { "F10",          67,    68 },
    { "F11",          68,    87 },
    { "F12",          69,    88 },
    { "Print-Screen", 70, 57436 },
    { "Scroll-Lock" , 71,    70 },
    { "Pause-Break" , 72,    69 },
    { "VOL+",        234,     0 },
    { "VOL-",        233,     0 }
};

QString getKeyValue(quint16 hid)
{
    QString value="";
    for(const keyMapItem&key:std::as_const(s_keMapTable))
    {
        if(key.hid == hid)
        {
            value = key.value;
            break;
        }
    }
    return value;
}

quint16 getKeyHid(quint16 nCode)
{
    quint16 hid=0;
    for(const keyMapItem&key:std::as_const(s_keMapTable))
    {
        if(key.code == nCode)
        {
            hid = key.hid;
            break;
        }
    }
    return hid;
}

quint16 getKeyNCode(quint16 hid)
{
    quint16 nCode=0;
    for(const keyMapItem&key:std::as_const(s_keMapTable))
    {
        if(key.hid == hid)
        {
            nCode = key.code;
            break;
        }
    }
    return nCode;
}

QString ModuleGenKeymapping::getKeyString(const keyData *data,bool addPreffix)
{
    if(data->b0 == 0x09)
    {
        return QString(tr("宏设置"));
    }

    for(const keyItem *k:std::as_const(m_fnList))
    {
        if(isKeyEqual(&k->data,data))
        {
            return QString(tr("功能: ")) + k->text;
        }
    }

    for(const keyItem *k:std::as_const(m_muList))
    {
        if(isKeyEqual(&k->data,data))
        {
            return QString(tr("鼠标: ")) + k->text;
        }
    }

    QString strV1 = getKeyValue(data->b1);
    QString strV2 = getKeyValue(data->b2);
    QString strV3 = getKeyValue(data->b3);

    if(strV1.isEmpty()) return QString("");

    QString strTxt(tr("组合: "));
    if(!addPreffix) strTxt="";
    if(!strV1.isEmpty()) strTxt += strV1;
    if(!strV2.isEmpty()) strTxt += QString(" + ") + strV2;
    if(!strV3.isEmpty()) strTxt += QString(" + ") + strV3;

    return strTxt;
}

bool isKeyEqual(const keyData*kdA, const keyData*kdB)
{
    return (
            kdA->b0 == kdB->b0 &&
            kdA->b1 == kdB->b1 &&
            kdA->b2 == kdB->b2 &&
            kdA->b3 == kdB->b3
        );
}

bool isKeyChanged(quint8 index,const keyData*kd)
{
    return !isKeyEqual(getMatData(index),kd);
}

bool isKeyDisabled(quint8 index)
{
    keyData*tk0 = getMatData(index);
    keyData tk1 = {0,0,0,0};
    return isKeyEqual(tk0,&tk1);
}

keyData* getMuData(quint8 index)
{
    return s_map->getMuData(index);
}

keyData* getFnData(quint8 index)
{
    return s_map->getFnData(index);
}

keyData* getMatData(quint8 index)
{
    static keyData kd;

    kd.b0 = matrixData[index * 4 + 0];
    kd.b1 = matrixData[index * 4 + 1];
    kd.b2 = matrixData[index * 4 + 2];
    kd.b3 = matrixData[index * 4 + 3];

    return &kd;
}

QString getKeyString(keyData *data, bool addPreffix)
{
    if( data->b0 == 0 && data->b1 == 0 &&  data->b2 ==0 && data->b3 == 0)
        return "";
    return s_map->getKeyString(data,addPreffix);
}
