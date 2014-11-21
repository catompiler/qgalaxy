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

void Log::add(Log::MsgType type_, const QString &who_, const QString &msg_)
{
    emit log(type_, who_, msg_);
}


void log(Log::MsgType msg_type_, const QString& who_, const QString& msg_)
{
    Log::instance().add(msg_type_, who_, msg_);
}

