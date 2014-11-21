#include <QtGui/QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include "mainwindow.h"
#include "settings.h"

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



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));

    Settings::get().read();

    MainWindow w;

    w.show();

    test();

    int exec_result = a.exec();

    Settings::get().write();
    
    return exec_result;
}
