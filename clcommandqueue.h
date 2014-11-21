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
    explicit CLCommandQueue(const cl_command_queue& cq_id);
    CLCommandQueue(const CLCommandQueue& cq);
    ~CLCommandQueue();

    bool create(const CLContext& clcxt, const CLDevice& device, cl_int *err_code = NULL) throw(CLException&);
    bool retain() throw(CLException&);
    bool release() throw(CLException&);

    cl_command_queue id() const;
    void setId(const cl_command_queue& cq_id);

    bool isValid() const;

    cl_context contextId() const throw(CLException&);
    cl_device_id deviceId() const throw(CLException&);

    CLCommandQueue& operator=(const CLCommandQueue& cq);
    bool operator==(const CLCommandQueue& cq) const;

private:
    cl_command_queue m_id;

    template<class T>
    T getInfoValue(cl_command_queue_info info) const throw(CLException&);
};

#endif // CLCOMMANDQUEUE_H
