#ifndef CLCONTEXT_H
#define CLCONTEXT_H

#include <CL/opencl.h>
#include <QList>
#include "clexception.h"
#include "utils.h"
#include "clplatform.h"
#include "cldevice.h"
#include "clcommandqueue.h"
#include "clevent.h"
#include "clbuffer.h"


class CLContext
{
public:
    CLContext();
    explicit CLContext(const cl_context& context_id_);
    CLContext(const CLContext& context_);
    ~CLContext();

    cl_context id() const;
    void setId(const cl_context& id_);

    bool isValid() const;

    bool create(const CLPlatform& platform_, const CLDeviceList& devices_,
                bool shared_with_gl_ = false, cl_int *err_code_ = NULL) throw(CLException&);
    bool retain() throw(CLException&);
    bool release() throw(CLException&);

    size_t devicesCount() const throw(CLException&);
    QList<CLDevice> devices() const throw(CLException&);

    CLContext& operator=(const CLContext& cxt_);
    bool operator==(const CLContext& cxt_) const;

    CLEvent createEvent(cl_int* err_code_ = NULL) const throw(CLException&);
    CLCommandQueue createCommandQueue(const CLDevice& device_, cl_int *err_code_ = NULL) const throw(CLException&);
    CLBuffer createBuffer(cl_mem_flags flags_, size_t size_,
                          void* host_ptr_, cl_int* err_code_ = NULL) throw(CLException&);

private:
    cl_context _id;

    template<class T>
    T _getInfoValue(cl_device_info info_) const throw(CLException&);

    template<class T>
    QVector<T> _getInfoValuev(cl_device_info info_) const throw(CLException&);
};

template<class T>
T CLContext::_getInfoValue(cl_device_info info_) const throw(CLException&)
{
    T res;
    CL_ERR_THROW(clGetContextInfo(_id, info_, sizeof(T),
                        static_cast<void*>(&res), NULL));
    return res;
}

template<class T>
QVector<T> CLContext::_getInfoValuev(cl_context_info info_) const throw(CLException&)
{
    size_t size = 0;

    CL_ERR_THROW(clGetContextInfo(_id, info_, 0, NULL, &size));

    size_t vec_size = size / sizeof(T);
    if(vec_size == 0) vec_size = 1;

    QVector<T> res(vec_size);

    CL_ERR_THROW(clGetContextInfo(_id, info_, size,
                        static_cast<void*>(res.data()), NULL));

    return res;
}

#endif // CLCONTEXT_H
