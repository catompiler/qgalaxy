#ifndef CLBUFFER_H
#define CLBUFFER_H

#include <CL/opencl.h>
#include "clexception.h"
#include "utils.h"
#include "clevent.h"

class CLContext;
class CLCommandQueue;


class CLBuffer
{
public:
    CLBuffer();
    explicit CLBuffer(const cl_mem& buf_id);
    CLBuffer(const CLBuffer& buffer);
    ~CLBuffer();

    cl_mem id() const;
    void setId(const cl_mem& buf_id);

    bool isValid() const;

    bool create(const CLContext& cxt, cl_mem_flags flags, size_t size,
                void* host_ptr, cl_int* err_code = NULL) throw(CLException&);
    bool retain() throw(CLException&);
    bool release() throw(CLException&);

    cl_mem_flags flags() const throw(CLException&);
    size_t size() const throw(CLException&);
    void* hostPtr() const throw(CLException&);
    cl_context contextId() const throw(CLException&);

    bool enqueueRead(const CLCommandQueue& queue, bool blocking,
                     size_t offset, size_t cb, void* ptr,
                     const CLEventList* wait_events = NULL, CLEvent* event = NULL) throw(CLException&);
    bool enqueueWrite(const CLCommandQueue& queue, bool blocking,
                      size_t offset, size_t cb, void* ptr,
                      const CLEventList* wait_events = NULL, CLEvent* event = NULL) throw(CLException&);

    void *enqueueMap(const CLCommandQueue& queue, bool blocking,
                     cl_map_flags flags, size_t offset, size_t cb,
                     const CLEventList* wait_events = NULL, CLEvent* event = NULL,
                     cl_int* err_code = NULL) throw(CLException&);
    bool enqueueUnMap(const CLCommandQueue& queue, void* ptr,
                     const CLEventList* wait_events = NULL, CLEvent* event = NULL) throw(CLException&);

    CLBuffer& operator=(const CLBuffer& buffer);
    bool operator==(const CLBuffer& buffer) const;

private:
    cl_mem m_id;

    template<class T>
    T getInfoValue(cl_mem_info info_) const throw(CLException&);
};

#endif // CLBUFFER_H
