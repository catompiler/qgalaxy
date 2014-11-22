#-------------------------------------------------
#
# Project created by QtCreator 2012-04-19T18:30:36
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = qgalaxy
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

LIBS += -L/usr/X11R6/lib64 -lGL -lOpenCL

SOURCES += main.cpp\
        mainwindow.cpp \
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
    clexception.cpp \
    clprogram.cpp \
    clkernel.cpp \
    nbodywidget.cpp

HEADERS  += mainwindow.h \
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
    utils.h \
    clprogram.h \
    clkernel.h \
    nbodywidget.h

FORMS    += mainwindow.ui \
    oclsettingsdialog.ui

RESOURCES += \
    res.qrc
