#include "clcontext.h"

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <GL/glx.h>
#endif


CLContext::CLContext()
{
    _id = NULL;
}

CLContext::CLContext(const cl_context &context_id_)
{
    _id = context_id_;
}

CLContext::CLContext(const CLContext &context_)
{
    _id = context_._id;
}

CLContext::~CLContext()
{
}

cl_context CLContext::id() const
{
    return _id;
}

void CLContext::setId(const cl_context &id_)
{
    _id = id_;
}

bool CLContext::isValid() const
{
    return _id != NULL;
}

bool CLContext::create(const CLPlatform &platform_, const CLDeviceList &devices_,
                       bool shared_with_gl_, cl_int* err_code_) throw(CLException&)
{
    static const char* gl_sharing_ext_name = "cl_khr_gl_sharing";

    if(devices_.empty()) return false;

    QVector<cl_context_properties> properties;
    QVector<cl_device_id> devices;

    for(CLDeviceList::const_iterator it = devices_.begin(); it != devices_.end(); ++ it){
        if(shared_with_gl_ && !(*it).hasExtension(gl_sharing_ext_name))
            continue;
        devices.push_back((*it).id());
    }

    if(devices.empty()) return false;

    properties.push_back(CL_CONTEXT_PLATFORM);
    properties.push_back(reinterpret_cast<cl_context_properties>(platform_.id()));

    if(shared_with_gl_){
            #ifdef Q_WS_X11
            properties.push_back(CL_GL_CONTEXT_KHR);
            properties.push_back(reinterpret_cast<cl_context_properties>(glXGetCurrentContext()));
            properties.push_back(CL_GLX_DISPLAY_KHR);
            properties.push_back(reinterpret_cast<cl_context_properties>(glXGetCurrentDisplay()));
            #endif

            #ifdef Q_WS_WIN
            properties.push_back(CL_GL_CONTEXT_KHR);
            properties.push_back(reinterpret_cast<cl_context_properties>(wglGetCurrentContext()));
            properties.push_back(CL_WGL_HDC_KHR);
            properties.push_back(reinterpret_cast<cl_context_properties>(wglGetCurrentDC()));
            #endif
    }

    properties.push_back(0);

    cl_int res = CL_SUCCESS;

    _id = clCreateContext(properties.data(), devices.size(), devices.data(), NULL, NULL, &res);
    if(err_code_) *err_code_ = res;

    CL_ERR_THROW(res);

    return _id != NULL;
}

bool CLContext::retain() throw(CLException&)
{
    CL_ERR_THROW(clRetainContext(_id));
    return true;
}

bool CLContext::release() throw(CLException&)
{
    CL_ERR_THROW(clReleaseContext(_id));
    return true;
}

size_t CLContext::devicesCount() const throw(CLException&)
{
    return _getInfoValue<cl_uint>(CL_CONTEXT_NUM_DEVICES);
}

QList<CLDevice> CLContext::devices() const throw(CLException&)
{
    QList<CLDevice> res;
    QVector<cl_device_id> dev_ids = _getInfoValuev<cl_device_id>(CL_CONTEXT_DEVICES);
    for(QVector<cl_device_id>::iterator it = dev_ids.begin(); it != dev_ids.end(); ++ it){
        res.push_back(CLDevice(*it));
    }
    return res;
}

CLContext &CLContext::operator =(const CLContext &cxt_)
{
    _id = cxt_._id;
    return *this;
}

bool CLContext::operator ==(const CLContext &cxt_) const
{
    return _id == cxt_._id;
}

CLEvent CLContext::createEvent(cl_int *err_code_) const throw(CLException&)
{
    CLEvent res_event;
    cl_int res = CL_SUCCESS;

    res_event.create(*this, &res);

    if(err_code_) *err_code_ = res;
    CL_ERR_THROW(res);

    return res_event;
}

CLCommandQueue CLContext::createCommandQueue(const CLDevice &device_, cl_int *err_code_) const throw(CLException&)
{
    CLCommandQueue res_queue;
    cl_int res = CL_SUCCESS;

    res_queue.create(*this, device_, &res);

    if(err_code_) *err_code_ = res;
    CL_ERR_THROW(res);

    return res_queue;
}

CLBuffer CLContext::createBuffer(cl_mem_flags flags_, size_t size_, void *host_ptr_, cl_int *err_code_) throw(CLException&)
{
    CLBuffer res_buffer;
    cl_int res = CL_SUCCESS;

    res_buffer.create(*this, flags_, size_, host_ptr_, &res);
    if(err_code_) *err_code_ = res;

    CL_ERR_THROW(res);

    return res_buffer;
}
