#include "clcommandqueue.h"
#include "clcontext.h"
#include "cldevice.h"



CLCommandQueue::CLCommandQueue()
{
    _id = NULL;
}

CLCommandQueue::CLCommandQueue(const cl_command_queue &cq_id_)
{
    _id = cq_id_;
}

CLCommandQueue::CLCommandQueue(const CLCommandQueue &cq_)
{
    _id = cq_._id;
}

CLCommandQueue::~CLCommandQueue()
{
}

bool CLCommandQueue::create(const CLContext &clcxt_, const CLDevice &device_, cl_int* err_code_) throw(CLException&)
{
    cl_int res = CL_SUCCESS;
    _id = clCreateCommandQueue(clcxt_.id(), device_.id(), 0, &res);
    if(err_code_) *err_code_ = res;
    CL_ERR_THROW(res);
    return _id != NULL;
}

bool CLCommandQueue::retain() throw(CLException&)
{
    CL_ERR_THROW(clRetainCommandQueue(_id));
    return true;
}

bool CLCommandQueue::release() throw(CLException&)
{
    CL_ERR_THROW(clReleaseCommandQueue(_id));
    return true;
}

cl_command_queue CLCommandQueue::id() const
{
    return _id;
}

void CLCommandQueue::setId(const cl_command_queue &id_)
{
    _id = id_;
}

bool CLCommandQueue::isValid() const
{
    return _id != NULL;
}

cl_context CLCommandQueue::contextId() const throw(CLException&)
{
    return _getInfoValue<cl_context>(CL_QUEUE_CONTEXT);
}

cl_device_id CLCommandQueue::deviceId() const throw(CLException&)
{
    return _getInfoValue<cl_device_id>(CL_QUEUE_DEVICE);
}

CLCommandQueue &CLCommandQueue::operator =(const CLCommandQueue &cq_)
{
    _id = cq_._id;
    return *this;
}

bool CLCommandQueue::operator ==(const CLCommandQueue &cq_) const
{
    return _id == cq_._id;
}
