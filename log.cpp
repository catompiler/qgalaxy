#include "log.h"
#include <QString>
#include "mainwindow.h"



Log::Log(QObject *parent) :
    QObject(parent)
{
}

Log::~Log()
{
}

Log& Log::instance()
{
    static Log log_inst;
    return log_inst;
}

void Log::add(Log::MsgType type, const QString &who, const QString &msg)
{
    emit log(type, who, msg);
}


void log(Log::MsgType msg_type, const QString& who, const QString& msg)
{
    Log::instance().add(msg_type, who, msg);
}

