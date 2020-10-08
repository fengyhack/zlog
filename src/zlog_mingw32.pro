#-------------------------------------------------
#
# Project created by QtCreator 2017-12-07T14:36:15
#
#-------------------------------------------------

QT       -= gui

TARGET = zlog
TEMPLATE = lib
CONFIG += shared
DEFINES += ZLOG_LIBRARY ZLOG_QT_VERSION

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        zlog.cpp

HEADERS += \
        zlog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# !!! Remarks abou 'shlwapi.lib' and 'user32.lib' !!!
# When using MSVS:
# If this library can not be found, please check to make sure it is installed correctly
# Or you may try this below if installed:
# "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x86/shlwapi.lib"

#win32 {
#	LIBS += "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x86/shlwapi.lib"
#}
