#-------------------------------------------------
#
# Project created by QtCreator 2015-11-27T18:11:42
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Poro
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    counterwidget.cpp \
    xmppclient.cpp \
    statusxml.cpp

HEADERS  += mainwindow.h \
    counterwidget.h \
    xmppclient.h \
    statusxml.h

FORMS    += mainwindow.ui \
    counterwidget.ui

INCLUDEPATH += $$PWD/gloox/
DEPENDPATH += $$PWD/gloox/
win32: LIBS += -lWininet


RESOURCES += \
    resource.qrc


win32:RC_ICONS



win32: LIBS += -L$$PWD/./ -lgloox

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
