#ifndef CLCOMMANDQUEUE_H
#define CLCOMMANDQUEUE_H

#include <CL/opencl.h>
#include "clexception.h"
#include "utils.h"



class CLContext;
class CLDevice;

class CLCommandQueue
{
public:
    CLCommandQueue();
    explicit CLCommandQueue(const cl_command_queue& cq_id_);
    CLCommandQueue(const CLCommandQueue& cq_);
    ~CLCommandQueue();

    bool create(const CLContext& clcxt_, const CLDevice& device_, cl_int *err_code_ = NULL) throw(CLException&);
    bool retain() throw(CLException&);
    bool release() throw(CLException&);

    cl_command_queue id() const;
    void setId(const cl_command_queue& id_);

    bool isValid() const;

    cl_context contextId() const throw(CLException&);
    cl_device_id deviceId() const throw(CLException&);

    CLCommandQueue& operator=(const CLCommandQueue& cq_);
    bool operator==(const CLCommandQueue& cq_) const;

private:
    cl_command_queue _id;

    template<class T>
    T _getInfoValue(cl_command_queue_info info_) const throw(CLException&);
};

template<class T>
T CLCommandQueue::_getInfoValue(cl_command_queue_info info_) const throw(CLException&)
{
    T res;
    CL_ERR_THROW(clGetCommandQueueInfo(_id, info_, sizeof(T),
                        static_cast<void*>(&res), NULL));
    return res;
}

#endif // CLCOMMANDQUEUE_H
