#include "clcontext.h"
#include "clexception.h"
#include "utils.h"

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <GL/glx.h>
#endif


CLContext::CLContext()
{
    m_id = nullptr;
}

CLContext::CLContext(const cl_context &context_id)
{
    m_id = context_id;
}

CLContext::CLContext(const CLContext &context)
{
    m_id = context.m_id;
}

CLContext::~CLContext()
{
}

cl_context CLContext::id() const
{
    return m_id;
}

void CLContext::setId(const cl_context &cxt_id)
{
    m_id = cxt_id;
}

bool CLContext::isValid() const
{
    return m_id != nullptr;
}

bool CLContext::create(const CLPlatform &platform, const CLDeviceList &devices,
                       bool shared_with_gl, cl_int* err_code)
{
    static const char* gl_sharing_ext_name = "cl_khr_gl_sharing";

    if(devices.empty()) return false;

    QVector<cl_context_properties> properties_vec;
    QVector<cl_device_id> devices_vec;

    for(CLDeviceList::const_iterator it = devices.begin(); it != devices.end(); ++ it){
        if(shared_with_gl && !(*it).hasExtension(gl_sharing_ext_name))
            continue;
        devices_vec.push_back((*it).id());
    }

    if(devices_vec.empty()) return false;

    properties_vec.push_back(CL_CONTEXT_PLATFORM);
    properties_vec.push_back(reinterpret_cast<cl_context_properties>(platform.id()));

    if(shared_with_gl){
            #ifdef Q_WS_X11
            properties_vec.push_back(CL_GL_CONTEXT_KHR);
            properties_vec.push_back(reinterpret_cast<cl_context_properties>(glXGetCurrentContext()));
            properties_vec.push_back(CL_GLX_DISPLAY_KHR);
            properties_vec.push_back(reinterpret_cast<cl_context_properties>(glXGetCurrentDisplay()));
            #endif

            #ifdef Q_WS_WIN
            properties.push_back(CL_GL_CONTEXT_KHR);
            properties.push_back(reinterpret_cast<cl_context_properties>(wglGetCurrentContext()));
            properties.push_back(CL_WGL_HDC_KHR);
            properties.push_back(reinterpret_cast<cl_context_properties>(wglGetCurrentDC()));
            #endif
    }

    properties_vec.push_back(0);

    cl_int res = CL_SUCCESS;

    m_id = clCreateContext(properties_vec.data(), devices_vec.size(), devices_vec.data(), nullptr, nullptr, &res);
    if(err_code) *err_code = res;

    CL_ERR_THROW(res);

    return m_id != nullptr;
}

bool CLContext::retain()
{
    CL_ERR_THROW(clRetainContext(m_id));
    return true;
}

bool CLContext::release()
{
    CL_ERR_THROW(clReleaseContext(m_id));
    return true;
}

size_t CLContext::devicesCount() const
{
    return getInfoValue<cl_uint>(CL_CONTEXT_NUM_DEVICES);
}

CLDeviceList CLContext::devices() const
{
    CLDeviceList res;
    QVector<cl_device_id> dev_ids = getInfoValuev<cl_device_id>(CL_CONTEXT_DEVICES);
    for(QVector<cl_device_id>::iterator it = dev_ids.begin(); it != dev_ids.end(); ++ it){
        res.push_back(CLDevice(*it));
    }
    return res;
}

CLContext &CLContext::operator =(const CLContext &cxt)
{
    m_id = cxt.m_id;
    return *this;
}

bool CLContext::operator ==(const CLContext &cxt) const
{
    return m_id == cxt.m_id;
}

CLEvent CLContext::createEvent(cl_int *err_code) const
{
    CLEvent res_event;
    cl_int res = CL_SUCCESS;

    res_event.create(*this, &res);

    if(err_code) *err_code = res;
    CL_ERR_THROW(res);

    return res_event;
}

CLCommandQueue CLContext::createCommandQueue(const CLDevice &device, cl_int *err_code) const
{
    CLCommandQueue res_queue;
    cl_int res = CL_SUCCESS;

    res_queue.create(*this, device, &res);

    if(err_code) *err_code = res;
    CL_ERR_THROW(res);

    return res_queue;
}

CLBuffer CLContext::createBuffer(cl_mem_flags flags, size_t size, void *host_ptr, cl_int *err_code) const
{
    CLBuffer res_buffer;
    cl_int res = CL_SUCCESS;

    res_buffer.create(*this, flags, size, host_ptr, &res);
    if(err_code) *err_code = res;

    CL_ERR_THROW(res);

    return res_buffer;
}

template<class T>
T CLContext::getInfoValue(cl_device_info info) const
{
    T res;
    CL_ERR_THROW(clGetContextInfo(m_id, info, sizeof(T),
                        static_cast<void*>(&res), nullptr));
    return res;
}

template<class T>
QVector<T> CLContext::getInfoValuev(cl_context_info info) const
{
    size_t size = 0;

    CL_ERR_THROW(clGetContextInfo(m_id, info, 0, nullptr, &size));

    size_t vec_size = size / sizeof(T);
    if(vec_size == 0) vec_size = 1;

    QVector<T> res(vec_size);

    CL_ERR_THROW(clGetContextInfo(m_id, info, size,
                        static_cast<void*>(res.data()), nullptr));

    return res;
}
