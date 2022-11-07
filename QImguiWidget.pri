
CONFIG += QT
QT     += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

INCLUDEPATH += $$PWD/imgui/

INCLUDEPATH += $$PWD/

HEADERS += \
    $$PWD/QImguiWidget.h \
    $$PWD/imgui/imconfig.h \
    $$PWD/imgui/imgui.h


SOURCES += \
    $$PWD/QImguiWidget.cpp \
    $$PWD/imgui/imgui_widgets.cpp \
    $$PWD/imgui/imgui_tables.cpp \
    $$PWD/imgui/imgui_draw.cpp \
    $$PWD/imgui/imgui_demo.cpp \
    $$PWD/imgui/imgui.cpp

