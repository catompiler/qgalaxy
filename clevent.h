#ifndef CLEVENT_H
#define CLEVENT_H

#include <QObject>
#include <QList>
#include <CL/opencl.h>
#include "clexception.h"
#include "utils.h"
#include "clcommandqueue.h"


class CLContext;


class CLEvent : public QObject
{
    Q_OBJECT
public:
    explicit CLEvent(QObject *parent = 0);
    explicit CLEvent(const cl_event& event_id, QObject* parent = 0);
    CLEvent(const CLEvent &event);
    ~CLEvent();

    cl_event id() const;
    void setId(const cl_event& event_id);

    bool isValid() const;

    bool create(const CLContext& cxt, cl_int* err_code = NULL) throw(CLException&);
    bool retain() throw(CLException&);
    bool release() throw(CLException&);

    cl_int status() const throw(CLException&);
    bool setStatus(cl_int status) throw(CLException&);

    bool wait() const throw(CLException&);
    static bool waitForEvents(const QList<CLEvent>& events) throw(CLException&);

    cl_command_queue commandQueueId() const throw(CLException&);
    cl_context contextId() const throw(CLException&);
    cl_command_type commandType() const throw(CLException&);

    CLEvent& operator=(const CLEvent& event);
    bool operator==(const CLEvent& event) const;
    
signals:
    void completed(cl_int exec_status);
private:
    cl_event m_id;

    inline void m_setId(cl_event ev_id) throw(CLException&);

    template<class T>
    T getInfoValue(cl_event_info info) const throw(CLException&);

    static void CL_CALLBACK event_notify(cl_event event, cl_int exec_status, void* user_data);
};


typedef QList<CLEvent> CLEventList;

#endif // CLEVENT_H
