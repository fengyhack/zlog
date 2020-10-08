QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += ZLOG_QT_VERSION

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    test_multi_thread.cpp

LIBS += zlog_win64d.lib

INCLUDEPATH += ../../src

