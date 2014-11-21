#-------------------------------------------------
#
# Project created by QtCreator 2012-04-19T18:30:36
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = qgalaxy
TEMPLATE = app

LIBS += -L/usr/X11R6/lib64 -lGL -lOpenCL

SOURCES += main.cpp\
        mainwindow.cpp \
    galaxyviewwidget.cpp \
    log.cpp \
    clglnbody.cpp \
    settings.cpp \
    clplatform.cpp \
    cldevice.cpp \
    clcontext.cpp \
    clcommandqueue.cpp \
    clevent.cpp \
    clbuffer.cpp \
    oclsettingsdialog.cpp \
    vendor.cpp \
    clexception.cpp

HEADERS  += mainwindow.h \
    galaxyviewwidget.h \
    log.h \
    clglnbody.h \
    settings.h \
    clplatform.h \
    cldevice.h \
    clcontext.h \
    clcommandqueue.h \
    clevent.h \
    clbuffer.h \
    oclsettingsdialog.h \
    vendor.h \
    clexception.h \
    utils.h

FORMS    += mainwindow.ui \
    oclsettingsdialog.ui

RESOURCES += \
    res.qrc
