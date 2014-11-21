#ifndef LOG_H
#define LOG_H

#include <QObject>

class QString;


class Log : public QObject
{
    Q_OBJECT
public:

    enum MsgType{NONE=0, DEBUG=1, INFO=2, WARNING=3, ERROR=4};

    static Log& instance();

signals:
    void log(Log::MsgType, const QString&, const QString&);
public slots:
    void add(Log::MsgType type, const QString& who, const QString& msg);

private:
    explicit Log(QObject *parent = 0);
    ~Log();
};

void log(Log::MsgType msg_type, const QString& who, const QString& msg);

#endif // LOG_H
