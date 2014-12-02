#include "clbuffer.h"
#include "clcontext.h"
#include "clexception.h"
#include "utils.h"


CLBuffer::CLBuffer()
{
    m_id = nullptr;
}

CLBuffer::CLBuffer(const cl_mem &buf_id)
{
    m_id = buf_id;
}

CLBuffer::CLBuffer(const CLBuffer &buffer)
{
    m_id = buffer.m_id;
}

CLBuffer::~CLBuffer()
{
}

cl_mem CLBuffer::id() const
{
    return m_id;
}

void CLBuffer::setId(const cl_mem &buf_id)
{
    m_id = buf_id;
}

bool CLBuffer::isValid() const
{
    return m_id != nullptr;
}

bool CLBuffer::create(const CLContext &cxt, cl_mem_flags flags, size_t size, void *host_ptr, cl_int *err_code)
{
    cl_int res = CL_SUCCESS;

    m_id = clCreateBuffer(cxt.id(), flags, size, host_ptr, &res);
    if(err_code) *err_code = res;

    CL_ERR_THROW(res);

    return m_id != nullptr;
}

bool CLBuffer::createFromGLBuffer(const CLContext &cxt, cl_mem_flags flags, GLuint glbuf_id, cl_int *err_code)
{
    cl_int res = CL_SUCCESS;

    m_id = clCreateFromGLBuffer(cxt.id(), flags,glbuf_id, &res);
    if(err_code) *err_code = res;

    CL_ERR_THROW(res);

    return m_id != nullptr;
}

bool CLBuffer::retain()
{
    CL_ERR_THROW(clRetainMemObject(m_id));
    return true;
}

bool CLBuffer::release()
{
    cl_uint refs_count = 0;
    try{ refs_count = getInfoValue<cl_uint>(CL_MEM_REFERENCE_COUNT); }catch(...){}

    CL_ERR_THROW(clReleaseMemObject(m_id));

    if(refs_count == 1) m_id = nullptr;
    return true;
}

bool CLBuffer::enqueueAcquireGLObject(const CLCommandQueue &queue, const CLEventList *wait_events, CLEvent *event)
{
    QVector<cl_event> wait_events_vec;
    const cl_event* wait_events_ptr = nullptr;

    cl_event event_id = nullptr;
    cl_event* event_id_ptr = nullptr;

    if(wait_events){
        for(CLEventList::const_iterator it = wait_events->begin(); it != wait_events->end(); ++ it){
            wait_events_vec.push_back((*it).id());
        }
    }

    if(!wait_events_vec.empty()) wait_events_ptr = wait_events_vec.data();

    if(event) event_id_ptr = &event_id;

    CL_ERR_THROW(clEnqueueAcquireGLObjects(queue.id(), 1, &m_id, wait_events_vec.size(), wait_events_ptr, event_id_ptr));

    if(event) event->setId(event_id);

    return true;
}

bool CLBuffer::enqueueReleaseGLObject(const CLCommandQueue &queue, const CLEventList *wait_events, CLEvent *event)
{
    QVector<cl_event> wait_events_vec;
    const cl_event* wait_events_ptr = nullptr;

    cl_event event_id = nullptr;
    cl_event* event_id_ptr = nullptr;

    if(wait_events){
        for(CLEventList::const_iterator it = wait_events->begin(); it != wait_events->end(); ++ it){
            wait_events_vec.push_back((*it).id());
        }
    }

    if(!wait_events_vec.empty()) wait_events_ptr = wait_events_vec.data();

    if(event) event_id_ptr = &event_id;

    CL_ERR_THROW(clEnqueueReleaseGLObjects(queue.id(), 1, &m_id, wait_events_vec.size(), wait_events_ptr, event_id_ptr));

    if(event) event->setId(event_id);

    return true;
}

cl_mem_flags CLBuffer::flags() const
{
    return getInfoValue<cl_mem_flags>(CL_MEM_FLAGS);
}

size_t CLBuffer::size() const
{
    return getInfoValue<size_t>(CL_MEM_SIZE);
}

void *CLBuffer::hostPtr() const
{
    return getInfoValue<void*>(CL_MEM_HOST_PTR);
}

cl_context CLBuffer::contextId() const
{
    return getInfoValue<cl_context>(CL_MEM_CONTEXT);
}

bool CLBuffer::enqueueRead(const CLCommandQueue &queue, bool blocking,
                           size_t offset, size_t cb, void *ptr,
                           const CLEventList *wait_events, CLEvent *event)
{
    cl_bool clblocking = CL_FALSE;

    QVector<cl_event> wait_events_vec;
    const cl_event* wait_events_ptr = nullptr;

    cl_event event_id = nullptr;
    cl_event* event_id_ptr = nullptr;

    if(blocking) clblocking = CL_TRUE;

    if(wait_events){
        for(CLEventList::const_iterator it = wait_events->begin(); it != wait_events->end(); ++ it){
            wait_events_vec.push_back((*it).id());
        }
    }

    if(!wait_events_vec.empty()) wait_events_ptr = wait_events_vec.data();

    if(event) event_id_ptr = &event_id;

    CL_ERR_THROW(clEnqueueReadBuffer(queue.id(), m_id, clblocking, offset, cb,
                    ptr, wait_events_vec.size(), wait_events_ptr, event_id_ptr));

    if(event) event->setId(event_id);

    return true;
}

bool CLBuffer::enqueueWrite(const CLCommandQueue &queue, bool blocking,
                            size_t offset, size_t cb, void *ptr,
                            const CLEventList *wait_events, CLEvent *event)
{
    cl_bool clblocking = CL_FALSE;

    QVector<cl_event> wait_events_vec;
    const cl_event* wait_events_ptr = nullptr;

    cl_event event_id = nullptr;
    cl_event* event_id_ptr = nullptr;

    if(blocking) clblocking = CL_TRUE;

    if(wait_events){
        for(CLEventList::const_iterator it = wait_events->begin(); it != wait_events->end(); ++ it){
            wait_events_vec.push_back((*it).id());
        }
    }

    if(!wait_events_vec.empty()) wait_events_ptr = wait_events_vec.data();

    if(event) event_id_ptr = &event_id;

    CL_ERR_THROW(clEnqueueReadBuffer(queue.id(), m_id, clblocking, offset, cb,
                        ptr, wait_events_vec.size(), wait_events_ptr, event_id_ptr));

    if(event) event->setId(event_id);

    return true;
}

void* CLBuffer::enqueueMap(const CLCommandQueue &queue, bool blocking,
                          cl_map_flags flags, size_t offset, size_t cb,
                          const CLEventList *wait_events, CLEvent *event,
                          cl_int *err_code)
{
    cl_bool clblocking = CL_FALSE;

    QVector<cl_event> wait_events_vec;
    const cl_event* wait_events_ptr = nullptr;

    cl_event event_id = nullptr;
    cl_event* event_id_ptr = nullptr;

    if(blocking) clblocking = CL_TRUE;

    if(wait_events){
        for(CLEventList::const_iterator it = wait_events->begin(); it != wait_events->end(); ++ it){
            wait_events_vec.push_back((*it).id());
        }
    }

    if(!wait_events_vec.empty()) wait_events_ptr = wait_events_vec.data();

    if(event) event_id_ptr = &event_id;

    cl_int res = CL_SUCCESS;
    void* res_ptr = clEnqueueMapBuffer(queue.id(), m_id, clblocking, flags, offset, cb,
                                       wait_events_vec.size(), wait_events_ptr, event_id_ptr, &res);
    if(err_code) *err_code = res;

    CL_ERR_THROW(res);

    if(res_ptr){
        if(event) event->setId(event_id);
    }

    return res_ptr;
}

bool CLBuffer::enqueueUnMap(const CLCommandQueue &queue, void *ptr, const CLEventList *wait_events, CLEvent *event)
{
    QVector<cl_event> wait_events_vec;
    const cl_event* wait_events_ptr = nullptr;

    cl_event event_id = nullptr;
    cl_event* event_id_ptr = nullptr;

    if(wait_events){
        for(CLEventList::const_iterator it = wait_events->begin(); it != wait_events->end(); ++ it){
            wait_events_vec.push_back((*it).id());
        }
    }

    if(!wait_events_vec.empty()) wait_events_ptr = wait_events_vec.data();

    if(event) event_id_ptr = &event_id;

    CL_ERR_THROW(clEnqueueUnmapMemObject(queue.id(), m_id, ptr,
            wait_events_vec.size(), wait_events_ptr, event_id_ptr));

    if(event) event->setId(event_id);

    return true;
}

CLBuffer &CLBuffer::operator =(const CLBuffer &buffer)
{
    m_id = buffer.m_id;
    return *this;
}

bool CLBuffer::operator ==(const CLBuffer &buffer) const
{
    return m_id == buffer.m_id;
}

template<class T>
T CLBuffer::getInfoValue(cl_mem_info info) const
{
    T res;
    CL_ERR_THROW(clGetMemObjectInfo(m_id, info, sizeof(T),
                      static_cast<void*>(&res), nullptr));
    return res;
}
