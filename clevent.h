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
    explicit CLEvent(const cl_event& event_id_, QObject* parent = 0);
    CLEvent(const CLEvent &event_);
    ~CLEvent();

    cl_event id() const;
    void setId(const cl_event& event_id_);

    bool isValid() const;

    bool create(const CLContext& cxt_, cl_int* err_code_ = NULL) throw(CLException&);
    bool retain() throw(CLException&);
    bool release() throw(CLException&);

    cl_int status() const throw(CLException&);
    bool setStatus(cl_int status_) throw(CLException&);

    bool wait() const throw(CLException&);
    static bool waitForEvents(const QList<CLEvent>& events_) throw(CLException&);

    cl_command_queue commandQueueId() const throw(CLException&);
    cl_context contextId() const throw(CLException&);
    cl_command_type commandType() const throw(CLException&);

    CLEvent& operator=(const CLEvent& event_);
    bool operator==(const CLEvent& event_) const;
    
signals:
    void completed(cl_int exec_status_);
private:
    cl_event _id;

    inline void _setId(cl_event id_) throw(CLException&);

    template<class T>
    T _getInfoValue(cl_event_info info_) const throw(CLException&);

    static void CL_CALLBACK _event_notify(cl_event event_, cl_int exec_status_, void* user_data_);
};


typedef QList<CLEvent> CLEventList;


template<class T>
T CLEvent::_getInfoValue(cl_event_info info_) const throw(CLException&)
{
    T res;
    CL_ERR_THROW(clGetEventInfo(_id, info_, sizeof(T),
                        static_cast<void*>(&res), NULL));
    return res;
}

#endif // CLEVENT_H
