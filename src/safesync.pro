#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T16:29:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = safesync
TEMPLATE = app

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
    filecontentdialog.cpp \
        main.cpp \
        mainwindow.cpp \
    configdialog.cpp \
    ScriptsClass/sync.cpp \
    background.cpp

HEADERS += \
    filecontentdialog.h \
        mainwindow.h \
    configdialog.h \
    ScriptsClass/sync.h \
    background.h \
    ScriptsClass/commandstruct.h

FORMS += \
        mainwindow.ui \
    configdialog.ui

DISTFILES += \
    Qt-AES-master/LICENSE

RESOURCES += \
    resources.qrc
