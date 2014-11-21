#include "clcommandqueue.h"
#include "clcontext.h"
#include "cldevice.h"



CLCommandQueue::CLCommandQueue()
{
    m_id = NULL;
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

bool CLCommandQueue::create(const CLContext &clcxt, const CLDevice &device, cl_int* err_code) throw(CLException&)
{
    cl_int res = CL_SUCCESS;
    m_id = clCreateCommandQueue(clcxt.id(), device.id(), 0, &res);
    if(err_code) *err_code = res;
    CL_ERR_THROW(res);
    return m_id != NULL;
}

bool CLCommandQueue::retain() throw(CLException&)
{
    CL_ERR_THROW(clRetainCommandQueue(m_id));
    return true;
}

bool CLCommandQueue::release() throw(CLException&)
{
    CL_ERR_THROW(clReleaseCommandQueue(m_id));
    return true;
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
    return m_id != NULL;
}

cl_context CLCommandQueue::contextId() const throw(CLException&)
{
    return getInfoValue<cl_context>(CL_QUEUE_CONTEXT);
}

cl_device_id CLCommandQueue::deviceId() const throw(CLException&)
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
T CLCommandQueue::getInfoValue(cl_command_queue_info info) const throw(CLException&)
{
    T res;
    CL_ERR_THROW(clGetCommandQueueInfo(m_id, info, sizeof(T),
                        static_cast<void*>(&res), NULL));
    return res;
}
