#-------------------------------------------------
#
# Project created by QtCreator 2012-04-19T18:30:36
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = qgalaxy
TEMPLATE = app

DEFINES += "CUSTOM_GLBUFFER"

QMAKE_CXXFLAGS += -std=c++11

linux-g++-32 {
    LIBS += -L/usr/X11R6/lib -L/usr/lib
}

linux-g++-64 {
    LIBS += -L/usr/X11R6/lib64 -L/usr/lib64
}

linux-g++ {
    LIBS += -lGL -lGLU -lOpenCL
}

win32 {
    LIBS += -lOpenGL32 -lGLU32 -lOpenCL
}

contains(DEFINES, "CUSTOM_GLBUFFER"){
    SOURCES += glbuffer.cpp
    HEADERS += glbuffer.h
}

SOURCES += main.cpp\
        mainwindow.cpp \
    log.cpp \
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
    nbodywidget.cpp \
    nbody.cpp \
    galaxy.cpp \
    spiralgalaxy.cpp \
    point3f.cpp \
    editbodydialog.cpp \
    utils.cpp

HEADERS  += mainwindow.h \
    log.h \
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
    nbodywidget.h \
    nbody.h \
    galaxy.h \
    spiralgalaxy.h \
    point3f.h \
    editbodydialog.h

FORMS    += mainwindow.ui \
    oclsettingsdialog.ui \
    editbodydialog.ui

RESOURCES += \
    res.qrc

OTHER_FILES += \
    nbody.cl
