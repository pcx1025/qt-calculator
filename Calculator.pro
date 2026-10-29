QT       += core gui widgets

CONFIG   += c++17

TARGET = Calculator
TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp

HEADERS += mainwindow.h

# 界面由 UI 设计器的 .ui 文件生成
FORMS   += mainwindow.ui
