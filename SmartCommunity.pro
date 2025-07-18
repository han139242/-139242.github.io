
QT       += core gui
QT       += sql
QT       += core gui charts  # 确保包含 charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartCommunitys
TEMPLATE = app
include($$PWD/cell/cell.pri)
include($$PWD/lib/lib.pri)
include($$PWD/showtool/showtool.pri)
include($$PWD/Qxlsx/Qxlsx.pri)
DEFINES += QT_DEPRECATED_WARNINGS

DESTDIR=$$PWD/../bin
CONFIG += c++11

RESOURCES += \
    picture.qrc

