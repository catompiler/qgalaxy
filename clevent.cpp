#include "clevent.h"
#include "clcontext.h"



CLEvent::CLEvent(QObject *parent) :
    QObject(parent)
{
    m_setId(NULL);
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
    return m_id != NULL;
}

bool CLEvent::create(const CLContext &cxt, cl_int *err_code) throw(CLException&)
{
    cl_int res;

    m_setId(clCreateUserEvent(cxt.id(), &res));
    if(err_code) *err_code = res;

    CL_ERR_THROW(res);

    return m_id != NULL;
}

bool CLEvent::retain() throw(CLException&)
{
    CL_ERR_THROW(clRetainEvent(m_id));
    return true;
}

cl_int CLEvent::status() const throw(CLException&)
{
    return getInfoValue<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS);
}

bool CLEvent::setStatus(cl_int status) throw(CLException&)
{
    CL_ERR_THROW(clSetUserEventStatus(m_id, status));
    return true;
}

bool CLEvent::wait() const throw(CLException&)
{
    CL_ERR_THROW(clWaitForEvents(1, &m_id));
    return true;
}

bool CLEvent::waitForEvents(const QList<CLEvent> &events) throw(CLException&)
{
    if(events.size() == 0) return false;
    QVector<cl_event> events_vec;
    for(QList<CLEvent>::const_iterator it = events.begin(); it != events.end(); ++ it){
        events_vec.push_back((*it).id());
    }
    CL_ERR_THROW(clWaitForEvents(events_vec.size(), events_vec.data()));
    return true;
}

cl_command_queue CLEvent::commandQueueId() const throw(CLException&)
{
    return getInfoValue<cl_command_queue>(CL_EVENT_COMMAND_QUEUE);
}

cl_context CLEvent::contextId() const throw(CLException&)
{
    return getInfoValue<cl_context>(CL_EVENT_CONTEXT);
}

cl_command_type CLEvent::commandType() const throw(CLException&)
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

void CLEvent::m_setId(cl_event ev_id) throw(CLException&)
{
    m_id = ev_id;
    if(m_id != NULL){
        CL_ERR_THROW(clSetEventCallback(m_id, CL_COMPLETE,
                            CLEvent::event_notify, static_cast<void*>(this)));
    }
}

void CL_CALLBACK CLEvent::event_notify(cl_event /*event*/, cl_int exec_status, void* user_data)
{
    emit static_cast<CLEvent*>(user_data)->completed(exec_status);
}

template<class T>
T CLEvent::getInfoValue(cl_event_info info) const throw(CLException&)
{
    T res;
    CL_ERR_THROW(clGetEventInfo(m_id, info, sizeof(T),
                        static_cast<void*>(&res), NULL));
    return res;
}

