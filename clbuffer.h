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
    explicit CLBuffer(const cl_mem& buf_id_);
    CLBuffer(const CLBuffer& buffer_);
    ~CLBuffer();

    cl_mem id() const;
    void setId(const cl_mem& buf_id_);

    bool isValid() const;

    bool create(const CLContext& cxt_, cl_mem_flags flags_, size_t size_,
                void* host_ptr_, cl_int* err_code_ = NULL) throw(CLException&);
    bool retain() throw(CLException&);
    bool release() throw(CLException&);

    cl_mem_flags flags() const throw(CLException&);
    size_t size() const throw(CLException&);
    void* hostPtr() const throw(CLException&);
    cl_context contextId() const throw(CLException&);

    bool enqueueRead(const CLCommandQueue& queue_, bool blocking_,
                     size_t offset_, size_t cb_, void* ptr_,
                     const CLEventList* wait_events_ = NULL, CLEvent* event_ = NULL) throw(CLException&);
    bool enqueueWrite(const CLCommandQueue& queue_, bool blocking_,
                      size_t offset_, size_t cb_, void* ptr_,
                      const CLEventList* wait_events_ = NULL, CLEvent* event_ = NULL) throw(CLException&);

    void *enqueueMap(const CLCommandQueue& queue_, bool blocking_,
                     cl_map_flags flags_, size_t offset_, size_t cb_,
                     const CLEventList* wait_events_ = NULL, CLEvent* event_ = NULL,
                     cl_int* err_code_ = NULL) throw(CLException&);
    bool enqueueUnMap(const CLCommandQueue& queue_, void* ptr_,
                     const CLEventList* wait_events_ = NULL, CLEvent* event_ = NULL) throw(CLException&);

    CLBuffer& operator=(const CLBuffer& buffer_);
    bool operator==(const CLBuffer& buffer_) const;

private:
    cl_mem _id;

    template<class T>
    T _getInfoValue(cl_mem_info info_) const throw(CLException&);
};


template<class T>
T CLBuffer::_getInfoValue(cl_mem_info info_) const throw(CLException&)
{
    T res;
    CL_ERR_THROW(clGetMemObjectInfo(_id, info_, sizeof(T),
                      static_cast<void*>(&res), NULL));
    return res;
}

#endif // CLBUFFER_H
