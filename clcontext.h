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
    explicit CLContext(const cl_context& context_id);
    CLContext(const CLContext& context);
    ~CLContext();

    cl_context id() const;
    void setId(const cl_context& cxt_id);

    bool isValid() const;

    bool create(const CLPlatform& platform, const CLDeviceList& devices,
                bool shared_with_gl = false, cl_int *err_code = NULL) throw(CLException&);
    bool retain() throw(CLException&);
    bool release() throw(CLException&);

    size_t devicesCount() const throw(CLException&);
    QList<CLDevice> devices() const throw(CLException&);

    CLContext& operator=(const CLContext& cxt);
    bool operator==(const CLContext& cxt) const;

    CLEvent createEvent(cl_int* err_code = NULL) const throw(CLException&);
    CLCommandQueue createCommandQueue(const CLDevice& device, cl_int *err_code = NULL) const throw(CLException&);
    CLBuffer createBuffer(cl_mem_flags flags, size_t size,
                          void* host_ptr, cl_int* err_code = NULL) throw(CLException&);

private:
    cl_context m_id;

    template<class T>
    T getInfoValue(cl_device_info info_) const throw(CLException&);

    template<class T>
    QVector<T> getInfoValuev(cl_device_info info_) const throw(CLException&);
};

#endif // CLCONTEXT_H
