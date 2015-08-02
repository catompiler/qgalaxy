#include <QtGui/QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include <stdlib.h>
#include <time.h>
#include "mainwindow.h"
#include "settings.h"

//#define LIST_CL_PLATFORMS_DEVICES

#ifdef LIST_CL_PLATFORMS_DEVICES
#include <QDebug>
#include "clplatform.h"
#include "cldevice.h"

void test(){
    CLPlatformList platforms = CLPlatform::getPlatforms();
    for(CLPlatformList::iterator it = platforms.begin(); it != platforms.end(); ++ it){
        qDebug() << "platform name: " << (*it).name();
        qDebug() << "platform vendor: " << (*it).vendor();
        qDebug() << "devices: (" << (*it).devicesCount() << ")";
        CLDeviceList devices = (*it).getDevices();
        for(CLDeviceList::iterator d_it = devices.begin(); d_it != devices.end(); ++ d_it){
            qDebug() << "device name: " << (*d_it).name();
            qDebug() << "device vendor id: " << (*d_it).vendorId();
        }
    }
}
#endif


int main(int argc, char *argv[])
{
    QLocale::setDefault(QLocale());

    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));

    QTranslator qttr;
    qttr.load("qt_" + QLocale::system().name());
    a.installTranslator(&qttr);

    QTranslator apptr;
    apptr.load("qgalaxy_" + QLocale::system().name());
    a.installTranslator(&apptr);

    Settings::get().read();

    MainWindow w;

    w.show();

#ifdef LIST_CL_PLATFORMS_DEVICES
    test();
#endif

    srand(time(nullptr));

    int exec_result = a.exec();

    Settings::get().write();
    
    return exec_result;
}
