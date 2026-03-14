#include "MainWindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QFontDatabase>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <shellapi.h>
#endif

int main(int argc, char *argv[])
{
#if(QT_VERSION <= QT_VERSION_CHECK(5,6,0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);// 启用高分辨率 pixmap
#endif

#ifdef _WIN32
    setlocale(LC_ALL, ".UTF-8");
#else
    setlocale(LC_ALL, "");
#endif

    QApplication a(argc, argv);

    QTranslator translatorM;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    qDebug() << uiLanguages;
    for (const QString &locale : uiLanguages) {
        const QString baseName = "AKKOStudio_" + QLocale(locale).name();
        qDebug() << baseName;
        if (translatorM.load(":/i18n/" + baseName)) {
            a.installTranslator(&translatorM);
            break;
        }
    }

    QTranslator translatorB;
    QTranslator translatorW;
    {
        QString baseName = QLocale::system().name(); // 如 "zh_CN"
        bool getB = translatorB.load("qtbase_" + baseName, QLibraryInfo::path(QLibraryInfo::TranslationsPath));
        bool getW = translatorW.load("qt_" + baseName, QLibraryInfo::path(QLibraryInfo::TranslationsPath));

        a.installTranslator(&translatorB);
        a.installTranslator(&translatorW);
    }


    SetConsoleOutputCP(CP_UTF8);

    QFontDatabase::addApplicationFont(QApplication::applicationDirPath() +
                                      "./font/MiSans-Bold.ttf");
    QFontDatabase::addApplicationFont(QApplication::applicationDirPath() +
                                      "./font/MiSans-Demibold.ttf");
    QFontDatabase::addApplicationFont(QApplication::applicationDirPath() +
                                      "./font/MiSans-ExtraLight.ttf");
    QFontDatabase::addApplicationFont(QApplication::applicationDirPath() +
                                      "./font/MiSans-Heavy.ttf");
    QFontDatabase::addApplicationFont(QApplication::applicationDirPath() +
                                      "./font/MiSans-Light.ttf");
    QFontDatabase::addApplicationFont(QApplication::applicationDirPath() +
                                      "./font/MiSans-Medium.ttf");
    QFontDatabase::addApplicationFont(QApplication::applicationDirPath() +
                                      "./font/MiSans-Normal.ttf");
    QFontDatabase::addApplicationFont(QApplication::applicationDirPath() +
                                      "./font/MiSans-Regular.ttf");
    QFontDatabase::addApplicationFont(QApplication::applicationDirPath() +
                                      "/font/MiSans-Semibold.ttf");
    QFontDatabase::addApplicationFont(QApplication::applicationDirPath() +
                                      "./font/MiSans-Thin.ttf");


    //QApplication::setStyle("Fusion");

    a.setStyleSheet(R"(

        * { font-family: MiSans,MiSans;}
        QLabel { font-size: 14px; font-weight: 400 ;}

        QLabel:disabled { color: #B3B3B3;}

        QLineEdit {
            border: 1px solid #D0D0D0;
            background-color: #EDEDED;
            border-radius: 6px;
            min-height: 20px;
            color: black;
            text-align: center;
            font-size: 14px;
            font-weight: 500;
            }

        QLineEdit:hover { background-color: #F0F0F0;}
        QLineEdit:focus{ border: 1px solid #50b7c1; border-radius: 4px;  background-color: rgb(230, 240, 255);}
        QLineEdit:disabled { background-color: transparent; color: #B7B7B7;border: 1px solid transparent;}

        QLabel#labelTitle1,#labelTitle2,#labelTitle3,#labelTitle4,#labelTitle5,#labelTitle6,#labelTitle,#labelName{ font-size: 16px; font-weight: 600; }
        QLabel#labelTitleL1,#labelTitleL2,#labelTitleL3,#labelTitleL4,#labelTitleL5 { font-size: 14px; font-weight: 500 ;}
        QLabel#labelTitleS1,#labelTitleS2,#labelTitleS3,#labelTitleS4,#labelTitleS5 { font-size: 16px; font-weight: 500 ;}
        QLabel#labelValue1,#labelValue5{ font-size: 10px; font-weight: 500;}
        QLabel#labelValue2,#labelValue4{ font-size: 14px; font-weight: 500;}
        QLabel#labelValue3,#labelValue6{ font-size: 18px; font-weight: 600;}
        QLabel#labelDeviceName,#labelDeviceName1{ font-size: 26px; font-weight: 600;}
        QLabel#labelInfo0{ font-size: 32px; font-weight: 600; color:#202020;}

        QTableView{
                 border: 1px solid gray;
                 gridline-color: transparent;
                 background-color: rgb(230, 240, 255);
                 gridline-color: gray;
                 font-size: 12px ;
                 font-weight: 500;
            }
        QTableView::item{padding-left:2px;  border-top: 0px solid gray; border-bottom: 1px solid transparent;border-right: 0px solid gray;}
        QTableView::item::selected{ background-color: #a0bb9e; color:white; }
        QTableView QTableCornerButton::section { background-color: skyblue ; min-width: 32px; border-top: 0px solid gray; border-bottom: 1px solid gray; border-left: 0px solid gray; border-right: 1px solid gray; }

        QTableView QHeaderView::section{background-color:skyblue;  font-size: 12px; font-weight: 500;}
        QTableView QHeaderView{background-color:skyblue; }
        QTableView::indicator { width: 18px; height: 18px; }
        QTableView::indicator:checked { image: url(:/images/BoxChecked.png); }
        QTableView::indicator:unchecked { image: url(:/images/BoxUncheck.png); }

        QHeaderView::section:horizontal{ padding-left: 2px; border-top: 0px solid gray; border-bottom: 1px solid gray; border-right: 1px solid gray;}
        QHeaderView::section:vertical{ padding-left: 2px; min-width: 36px; border-top: 0px solid gray; border-bottom: 1px solid gray; border-left: 0px solid gray; border-right: 1px solid gray;}
        QHeaderView::section:vertical{ text-align: right;}

        /*QPushButton {
                background-color: #2D7FDD;
                border-radius: 8px;
                color: white;
                border: 1px solid #6C9F50;
                padding: 1px;
                min-width: 16px;
                min-height: 8px;
                max-width: 800px;
                max-height: 200px;
                font-size: 14px ;
                font-weight: 400;
            }
        QPushButton:hover { background-color: #87ceeb; }
        QPushButton:pressed { background-color: #1e90af; }
        QPushButton:checked { background-color: #1e90ff; }
        QPushButton:disabled { background-color: gray; color: #cccccc;}*/

        QMessageBox QPushButton {
                border: 1px solid #6C9F50;
                background-color: #6329B6;
                color: white;
                border-radius: 8px;
                padding: 2px 2px;
                min-width: 80px;
                min-height: 24px; }

        QMessageBox {min-width: 400px; min-height: 150px;}
        QMessageBox QLabel#qt_msgbox_label{min-width: 320px; min-height: 60px; max-width: 450px; max-height: 520px; qproperty-alignment: AlignLeft; white-space: pre-wrap;font: bold 12px 微软雅黑;}
        QMessageBox QLabel#qt_msgboxex_icon_label{ min-width: 32px; min-height: 32px; max-width: 32px; max-height: 32px;qproperty-alignment: AlignTop;}


        #pushButtonPlug,#pushButtonMinus{
                border: none;
                border-radius: 0px;
                background-color: transparent;
                min-width:20px;
                min-height:20px;
                max-width:20px;
                max-height:20px;
                icon-size: 16px;
            }

        #pushButtonPlug,#pushButtonMinus:hover { background-color: #F0F0F0;}

        #pushButtonPlug { icon: url(:/images/rt/plug-normal.png);}
        #pushButtonPlug:disabled { icon: url(:/images/rt/plug-disabled.png);}
        #pushButtonMinus { icon: url(:/images/rt/minus-normal.png);}
        #pushButtonMinus:disabled { icon: url(:/images/rt/minus-disabled.png);}

        QWidget#pageWakeup QPushButton {
                background-color: red;
                border-radius: 10px;
                color: white;
                border: 1px solid #6C9F50;
                padding: 1px;
                min-width: 46px;
                min-height: 8px;
                max-width: 800px;
                max-height: 200px;
                font-size: 18px ;
                font-weight: 600;
            }
        QWidget#pageWakeup{ background-color: #F6F6F6; border-radius: 32px; }

        QSlider { border-radius: 12px; height: 24px; }
        QSlider::groove:horizontal { height: 12px; background: #DCDCDC; border-radius: 6px; }
        QSlider::sub-page:horizontal { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #FDDFBA, stop:1 #FFA73C);  border-radius: 6px; }
        QSlider::sub-page:horizontal:disabled { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 white, stop:1 #B3B3B3);  border-radius: 6px; }
        QSlider::handle:horizontal {
            width: 20px;
            height: 16px;
            margin: -6px 0;
            border-radius: 12px;
            background: white;
            border: 2px solid #FFB459; }
        QSlider::handle:horizontal:disabled { border: 2px solid #B3B3B3; }

        QSlider::groove:vertical { width: 24px; background: #DCDCDC; border-radius: 12px; }
        QSlider::sub-page:vertical { background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #FDDFBA, stop:1 #FFA73C);  border-radius: 12px; }
        QSlider::handle:vertical {
            width: 32px;
            height: 10px;
            margin: 0 -6px;
            border-radius: 5px;
            background: #FFAF4E ;
            border: 2px solid white; }

        QSlider::handle:hover { background: #F0F0F0; }
        QSlider::handle:pressed { background: #E0E0E0; border-color: #2D7FDD; }

        #horiCyanSlider::sub-page:horizontal { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #C3FFFD, stop:1 #39E1DC);  border-radius: 6px;}
        #horiCyanSlider::handle:horizontal { border: 2px solid #5CFFFA; }
        #horiCyanSlider::handle:hover { background: #F0F0F0;}
        #horiCyanSlider::handle:pressed { background: #E0E0E0; border-color: #2D7FDD;}

        QRadioButton { spacing: 5px; color: #333333; font-size: 14px; font-weight: 500 ;}
        QRadioButton::indicator {
            background: transparent;
            border-color: transparent;
            width: 20px;
            height: 20px;
            border: none;
            image: url(:/images/macro/radio-unchecked.png);}

        QRadioButton::indicator:checked { image: url(:/images/macro/radio-checked.png); }

        QRadioButton:disabled { color: #cccccc; }
        QRadioButton:checked {  color: #0085FF; }
        QRadioButton::indicator:disabled { border: 2px solid #dddddd; background: white; }

        QCheckBox { spacing: 5px; color: #333; }
        QCheckBox::indicator { width: 14px; height: 14px; }
        QCheckBox::indicator:unchecked { background: white; border: 2px solid #999; }
        QCheckBox::indicator:checked {
            width: 18px;
            height: 18px;
            background: transparent;
            image: url(:/images/BoxChecked.png);
        }

        QCheckBox::indicator:hover { border-color: #666; }
        QCheckBox:disabled { color: #AAA; }
        QCheckBox::indicator:disabled { background: #EEE; }

        QComboBox {
            border: 1px solid gray;
            border-radius: 4px;
            background: #FFFFFF;
            color: #333333;
            font-weight: normal;
            padding: 2px 2px;
        }

        QComboBox:hover { border-color: #ADADFD; }

        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 18px;
            border-left: 1px solid #E0E0E0;
        }

        QComboBox::down-arrow { width: 14px; height: 14px; image: url(:/images/down-arrow.png); }
        QComboBox:disabled { background: #F5F5F5; color: #9E9E9E; }
        QComboBox:disabled::down-arrow { opacity: 0.5; }

    QScrollBar:vertical,
    QScrollBar:horizontal {
        background: transparent;
        border: none;
        margin: 0px;
        padding: 0px;
    }

    QScrollBar:vertical {
        width: 4px;
        margin: 0px 0px 0px 0px;
    }

    QScrollBar:horizontal {
        height: 4px;
        margin: 0px 2px 0px 2px;
    }

    QScrollBar::handle:vertical,
    QScrollBar::handle:horizontal {
        background: rgba(220, 220, 220, 0.3);
        border-radius: 2px;
        min-height: 20px;
        max-height: 20px;
        min-width: 20px;
    }

    QScrollBar::handle:vertical:hover,
    QScrollBar::handle:horizontal:hover { background: rgba(160, 250, 160, 0.7); }

    QScrollBar::handle:vertical:pressed,
    QScrollBar::handle:horizontal:pressed { background: rgba(160, 250, 160, 0.98); }


QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical,
QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal {
    border: none;
    background: transparent;
    height: 0px;
    width: 0px;
}

QScrollBar::add-page:vertical:hover,
QScrollBar::add-page:horizontal:hover { background: transparent;}
QScrollBar::sub-page:vertical:hover,
QScrollBar::sub-page:horizontal:hover { background: transparent;}

QScrollBar::add-page:vertical,
QScrollBar::add-page:horizontal { background: transparent;}
QScrollBar::sub-page:vertical,
QScrollBar::sub-page:horizontal { background: transparent;}

QScrollArea { background-color: white;border:none }

/*
QScrollArea {
    qproperty-verticalScrollBarPolicy: Qt::ScrollBarAlwaysOff;
    qproperty-horizontalScrollBarPolicy: Qt::ScrollBarAlwaysOff;
}

QScrollArea:hover {
    qproperty-verticalScrollBarPolicy: Qt::ScrollBarAsNeeded;
    qproperty-horizontalScrollBarPolicy: Qt::ScrollBarAsNeeded;
}*/

QTabWidget {
    background-color: transparent;
    border: none;
    border-radius:0;
    padding: 0px;
    margin: 0;
    color: black;
    font-size: 18px;
    font-weight: 400;
}

/* 标签栏的样式 - 水平标签栏 */
QTabBar::tab {
    background-color: transparent;
    border: 3px solid transparent;
    padding: 4px 2px;
    margin-right: 10px;
    min-width: 40px;
    min-height: 20px;
    font-size: 18px;
    font-weight: 400;
}

QTabBar::tab:selected {
    color: #6329B6;
    font-weight: 900;
    border: 3px solid transparent;
    border-bottom: 3px solid #6329B6;
}

QTabBar::tab:hover:!selected {
    background-color: transparent;
    color: #000;
}

QTabBar::tab:vertical {
    min-height: 20px;
}

QTabWidget::pane {
    background-color: transparent;
    border: none;
    border-radius: 0;
    padding: 0px;
    margin-top: -1px; /* 与标签栏无缝连接 */
}

/* 标签栏下方的分隔线 */
QTabBar::separator {
    width: 1px;
    background-color: #aaa;
}

    QSpinBox {
        border: 1px solid #D0D0D0;
        border-radius: 4px;
        padding: 2px 2px 2px 2px;
        font-size: 14px;
        color: #333333;
        background-color: #FFFFFF;
    }

    QSpinBox:focus {
        border-color: #007AFF;
        outline: none;
    }

    QSpinBox::up-button, QSpinBox::down-button {
        width: 12px;
        border: 2px solid transparent;
        background-color: transparent;
    }

    QSpinBox::up-arrow, QSpinBox::down-arrow {
        width: 6px;
        height: 6px;
        image: none;
        border-left: 3px solid transparent;
        border-right: 3px solid transparent;
    }

    QSpinBox::up-arrow {
        border-bottom: 3px solid #007AFF;
    }

    QSpinBox::down-arrow {
        border-top: 3px solid #007AFF;
    }

    /* 只读状态样式 */
    QSpinBox:read-only {
        background-color: #F5F5F5;
        color: #666666;
        border-color: #EEEEEE;
    }

    )");

    MainWindow w;
    w.m_pMainTrM = &translatorM;
    w.m_pMainTrA = &translatorB;
    w.m_pMainTrB = &translatorW;
    w.show();
    return a.exec();
}
