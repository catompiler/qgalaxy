#include "clevent.h"
#include "clcontext.h"



CLEvent::CLEvent(QObject *parent) :
    QObject(parent)
{
    _setId(NULL);
}

CLEvent::CLEvent(const cl_event &event_id_, QObject *parent) :
    QObject(parent)
{
    _setId(event_id_);
}

CLEvent::CLEvent(const CLEvent &event_)
    :QObject(const_cast<CLEvent*>(&event_))
{
    _setId(event_._id);
}

CLEvent::~CLEvent()
{
}

cl_event CLEvent::id() const
{
    return _id;
}

void CLEvent::setId(const cl_event &event_id_)
{
    _setId(event_id_);
}

bool CLEvent::isValid() const
{
    return _id != NULL;
}

bool CLEvent::create(const CLContext &cxt_, cl_int *err_code_) throw(CLException&)
{
    cl_int res;

    _setId(clCreateUserEvent(cxt_.id(), &res));
    if(err_code_) *err_code_ = res;

    CL_ERR_THROW(res);

    return _id != NULL;
}

bool CLEvent::retain() throw(CLException&)
{
    CL_ERR_THROW(clRetainEvent(_id));
    return true;
}

cl_int CLEvent::status() const throw(CLException&)
{
    return _getInfoValue<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS);
}

bool CLEvent::setStatus(cl_int status_) throw(CLException&)
{
    CL_ERR_THROW(clSetUserEventStatus(_id, status_));
    return true;
}

bool CLEvent::wait() const throw(CLException&)
{
    CL_ERR_THROW(clWaitForEvents(1, &_id));
    return true;
}

bool CLEvent::waitForEvents(const QList<CLEvent> &events_) throw(CLException&)
{
    if(events_.size() == 0) return false;
    QVector<cl_event> events;
    for(QList<CLEvent>::const_iterator it = events_.begin(); it != events_.end(); ++ it){
        events.push_back((*it).id());
    }
    CL_ERR_THROW(clWaitForEvents(events.size(), events.data()));
    return true;
}

cl_command_queue CLEvent::commandQueueId() const throw(CLException&)
{
    return _getInfoValue<cl_command_queue>(CL_EVENT_COMMAND_QUEUE);
}

cl_context CLEvent::contextId() const throw(CLException&)
{
    return _getInfoValue<cl_context>(CL_EVENT_CONTEXT);
}

cl_command_type CLEvent::commandType() const throw(CLException&)
{
    return _getInfoValue<cl_command_type>(CL_EVENT_COMMAND_TYPE);
}

CLEvent &CLEvent::operator =(const CLEvent &event_)
{
    _setId(event_._id);
    return *this;
}

bool CLEvent::operator ==(const CLEvent &event_) const
{
    return _id == event_._id;
}

void CLEvent::_setId(cl_event id_) throw(CLException&)
{
    _id = id_;
    if(_id != NULL){
        CL_ERR_THROW(clSetEventCallback(_id, CL_COMPLETE,
                            CLEvent::_event_notify, static_cast<void*>(this)));
    }
}

void CL_CALLBACK CLEvent::_event_notify(cl_event /*event_*/, cl_int exec_status_, void* user_data_)
{
    emit static_cast<CLEvent*>(user_data_)->completed(exec_status_);
}

