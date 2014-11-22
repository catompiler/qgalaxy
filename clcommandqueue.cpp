#include "clcommandqueue.h"
#include "clcontext.h"
#include "cldevice.h"
#include "clexception.h"
#include "utils.h"



CLCommandQueue::CLCommandQueue()
{
    m_id = nullptr;
}

CLCommandQueue::CLCommandQueue(const cl_command_queue &cq_id)
{
    m_id = cq_id;
}

CLCommandQueue::CLCommandQueue(const CLCommandQueue &cq)
{
    m_id = cq.m_id;
}

CLCommandQueue::~CLCommandQueue()
{
}

cl_command_queue CLCommandQueue::id() const
{
    return m_id;
}

void CLCommandQueue::setId(const cl_command_queue &cq_id)
{
    m_id = cq_id;
}

bool CLCommandQueue::isValid() const
{
    return m_id != nullptr;
}

bool CLCommandQueue::create(const CLContext &clcxt, const CLDevice &device, cl_int* err_code)
{
    cl_int res = CL_SUCCESS;
    m_id = clCreateCommandQueue(clcxt.id(), device.id(), 0, &res);
    if(err_code) *err_code = res;
    CL_ERR_THROW(res);
    return m_id != nullptr;
}

bool CLCommandQueue::retain()
{
    CL_ERR_THROW(clRetainCommandQueue(m_id));
    return true;
}

bool CLCommandQueue::release()
{
    CL_ERR_THROW(clReleaseCommandQueue(m_id));
    return true;
}

cl_context CLCommandQueue::contextId() const
{
    return getInfoValue<cl_context>(CL_QUEUE_CONTEXT);
}

cl_device_id CLCommandQueue::deviceId() const
{
    return getInfoValue<cl_device_id>(CL_QUEUE_DEVICE);
}

CLCommandQueue &CLCommandQueue::operator =(const CLCommandQueue &cq)
{
    m_id = cq.m_id;
    return *this;
}

bool CLCommandQueue::operator ==(const CLCommandQueue &cq) const
{
    return m_id == cq.m_id;
}

template<class T>
T CLCommandQueue::getInfoValue(cl_command_queue_info info) const
{
    T res;
    CL_ERR_THROW(clGetCommandQueueInfo(m_id, info, sizeof(T),
                        static_cast<void*>(&res), nullptr));
    return res;
}
