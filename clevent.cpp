#include "clevent.h"
#include "clcontext.h"


CLEvent::CLEvent(QObject *parent) :
    QObject(parent)
{
    m_setId(nullptr);
}

CLEvent::CLEvent(const cl_event &event_id, QObject *parent) :
    QObject(parent)
{
    m_setId(event_id);
}

CLEvent::CLEvent(const CLEvent &event)
    :QObject(const_cast<CLEvent*>(&event))
{
    m_setId(event.m_id);
}

CLEvent::~CLEvent()
{
}

cl_event CLEvent::id() const
{
    return m_id;
}

void CLEvent::setId(const cl_event &event_id)
{
    m_setId(event_id);
}

bool CLEvent::isValid() const
{
    return m_id != nullptr;
}

bool CLEvent::create(const CLContext &cxt, cl_int *err_code)
{
    cl_int res;

    m_setId(clCreateUserEvent(cxt.id(), &res));
    if(err_code) *err_code = res;

    CL_ERR_THROW(res);

    return m_id != nullptr;
}

bool CLEvent::retain()
{
    CL_ERR_THROW(clRetainEvent(m_id));
    return true;
}

bool CLEvent::release()
{
    CL_ERR_THROW(clReleaseEvent(m_id));
    return true;
}

cl_int CLEvent::status() const
{
    return getInfoValue<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS);
}

bool CLEvent::setStatus(cl_int status)
{
    CL_ERR_THROW(clSetUserEventStatus(m_id, status));
    return true;
}

bool CLEvent::isCompleted() const
{
    return status() == CL_COMPLETE;
}

bool CLEvent::isRunning() const
{
    cl_int ev_status = status();
    return ev_status >= 0 && ev_status != CL_COMPLETE;
}

bool CLEvent::wait() const
{
    CL_ERR_THROW(clWaitForEvents(1, &m_id));
    return true;
}

bool CLEvent::waitForEvents(const QList<CLEvent> &events)
{
    if(events.size() == 0) return false;
    QVector<cl_event> events_vec;
    for(QList<CLEvent>::const_iterator it = events.begin(); it != events.end(); ++ it){
        events_vec.push_back((*it).id());
    }
    CL_ERR_THROW(clWaitForEvents(events_vec.size(), events_vec.data()));
    return true;
}

cl_command_queue CLEvent::commandQueueId() const
{
    return getInfoValue<cl_command_queue>(CL_EVENT_COMMAND_QUEUE);
}

cl_context CLEvent::contextId() const
{
    return getInfoValue<cl_context>(CL_EVENT_CONTEXT);
}

cl_command_type CLEvent::commandType() const
{
    return getInfoValue<cl_command_type>(CL_EVENT_COMMAND_TYPE);
}

CLEvent &CLEvent::operator =(const CLEvent &event_)
{
    m_setId(event_.m_id);
    return *this;
}

bool CLEvent::operator ==(const CLEvent &event) const
{
    return m_id == event.m_id;
}

void CLEvent::m_setId(cl_event ev_id)
{
    m_id = ev_id;
    if(m_id != nullptr){
        CL_ERR_THROW(clSetEventCallback(m_id, CL_COMPLETE,
                            CLEvent::event_notify, static_cast<void*>(this)));
    }
}

void CL_CALLBACK CLEvent::event_notify(cl_event /*event*/, cl_int exec_status, void* user_data)
{
    if(user_data != nullptr){
        emit static_cast<CLEvent*>(user_data)->completed(static_cast<int>(exec_status));
    }
}

template<class T>
T CLEvent::getInfoValue(cl_event_info info) const
{
    T res;
    CL_ERR_THROW(clGetEventInfo(m_id, info, sizeof(T),
                        static_cast<void*>(&res), nullptr));
    return res;
}

