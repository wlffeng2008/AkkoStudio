QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

QMAKE_PROJECT_DEPTH = 0

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

DESTDIR = ../../bin
RC_ICONS = AKKO.ico
VERSION = 1.0.0.0
TARGET = AKKOStudio
QMAKE_TARGET_COMPANY = "AKKO"
QMAKE_TARGET_PRODUCT = "AKKO "
QMAKE_TARGET_DESCRIPTION = "AKKO: Created by Qt6.10.0"
QMAKE_TARGET_COPYRIGHT = "AKKO(2025.10)"
RC_LANG = 0x0004

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -L$$PWD -lhidapi

INCLUDEPATH += ./Module


SOURCES += \
    Module/ColorLabel.cpp \
    Module/ColorSquare.cpp \
    Module/CustumSlider.cpp \
    DialogDeviceConnect.cpp \
    DialogMainwork.cpp \
    FrameAbout.cpp \
    FrameKeySetting.cpp \
    FrameLight.cpp \
    FrameMacro.cpp \
    FrameMagic.cpp \
    FrameMain.cpp \
    LinearFixing1.cpp \
    LinearFixing2.cpp \
    Module/MacroSquare.cpp \
    Module/ColorSlider.cpp \
    Module/DialogFNPicker.cpp \
    Module/DialogVKPicker.cpp \
    Module/MacroItem.cpp \
    Module/ModuleDKSAdjust.cpp \
    Module/ModuleDKSItem.cpp \
    Module/ModuleEfColor.cpp \
    Module/ModuleEfLumi.cpp \
    Module/ModuleEfMode.cpp \
    Module/ModuleEfSpeed.cpp \
    Module/ModuleGenKeymapping.cpp \
    Module/ModuleGeneralMasker.cpp \
    Module/ModuleLangMenu.cpp \
    Module/ModuleLinear.cpp \
    Module/ModuleRtSet.cpp \
    Module/ModuleKeyboard.cpp \
    Module/ModuleVideoItem.cpp \
    Module/VirtualKeyboard.cpp \
    Module/keyboardbutton.cpp \
    Module/modulemacromanager.cpp \
    Module/ModuleScrollValue.cpp \
    Module/NewTabWidget.cpp \
    Module/SuperLabel.cpp \
    Module/ToggleButton.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    Module/ColorLabel.h \
    Module/ColorSquare.h \
    Module/CustumSlider.h \
    DialogDeviceConnect.h \
    DialogMainwork.h \
    FrameAbout.h \
    FrameKeySetting.h \
    FrameLight.h \
    FrameMacro.h \
    FrameMagic.h \
    FrameMain.h \
    LinearFixing1.h \
    LinearFixing2.h \
    MainWindow.h \
    Module/MacroSquare.h \
    Module/ColorSlider.h \
    Module/DialogFNPicker.h \
    Module/DialogVKPicker.h \
    Module/MacroItem.h \
    Module/ModuleDKSAdjust.h \
    Module/ModuleDKSItem.h \
    Module/ModuleEfColor.h \
    Module/ModuleEfLumi.h \
    Module/ModuleEfMode.h \
    Module/ModuleEfSpeed.h \
    Module/ModuleGenKeymapping.h \
    Module/ModuleGeneralMasker.h \
    Module/ModuleLangMenu.h \
    Module/ModuleLinear.h \
    Module/ModuleRtSet.h \
    Module/ModuleKeyboard.h \
    Module/ModuleVideoItem.h \
    Module/VirtualKeyboard.h \
    Module/keyboardbutton.h \
    Module/modulemacromanager.h \
    Module/ModuleScrollValue.h \
    Module/NewTabWidget.h \
    Module/SuperLabel.h \
    Module/ToggleButton.h

FORMS += \
    DialogDeviceConnect.ui \
    DialogMainwork.ui \
    FrameAbout.ui \
    FrameKeySetting.ui \
    FrameLight.ui \
    FrameMacro.ui \
    FrameMagic.ui \
    FrameMain.ui \
    MainWindow.ui \
    Module/DialogFNPicker.ui \
    Module/DialogVKPicker.ui \
    Module/MacroItem.ui \
    Module/ModuleDKSAdjust.ui \
    Module/ModuleDKSItem.ui \
    Module/ModuleEfColor.ui \
    Module/ModuleEfLumi.ui \
    Module/ModuleEfMode.ui \
    Module/ModuleEfSpeed.ui \
    Module/ModuleLangMenu.ui \
    Module/ModuleLinear.ui \
    Module/ModuleRtSet.ui \
    Module/ModuleKeyboard.ui \
    Module/ModuleVideoItem.ui \
    Module/VirtualKeyboard.ui \
    Module/ModuleScrollValue.ui

TRANSLATIONS += \
    AKKOStudio_en_US.ts \
    AKKOStudio_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
