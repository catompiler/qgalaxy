#include "clbuffer.h"
#include "clcontext.h"


CLBuffer::CLBuffer()
{
    m_id = NULL;
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
    return m_id != NULL;
}

bool CLBuffer::create(const CLContext &cxt, cl_mem_flags flags, size_t size, void *host_ptr, cl_int *err_code) throw(CLException&)
{
    cl_int res = CL_SUCCESS;

    m_id = clCreateBuffer(cxt.id(), flags, size, host_ptr, &res);
    if(err_code) *err_code = res;

    CL_ERR_THROW(res);

    return m_id != NULL;
}

bool CLBuffer::retain() throw(CLException&)
{
    CL_ERR_THROW(clRetainMemObject(m_id));
    return true;
}

bool CLBuffer::release() throw(CLException&)
{
    CL_ERR_THROW(clReleaseMemObject(m_id));
    return true;
}

cl_mem_flags CLBuffer::flags() const throw(CLException&)
{
    return getInfoValue<cl_mem_flags>(CL_MEM_FLAGS);
}

size_t CLBuffer::size() const throw(CLException&)
{
    return getInfoValue<size_t>(CL_MEM_SIZE);
}

void *CLBuffer::hostPtr() const throw(CLException&)
{
    return getInfoValue<void*>(CL_MEM_HOST_PTR);
}

cl_context CLBuffer::contextId() const throw(CLException&)
{
    return getInfoValue<cl_context>(CL_MEM_CONTEXT);
}

bool CLBuffer::enqueueRead(const CLCommandQueue &queue, bool blocking,
                           size_t offset, size_t cb, void *ptr,
                           const CLEventList *wait_events, CLEvent *event) throw(CLException&)
{
    cl_bool clblocking = CL_FALSE;

    QVector<cl_event> wait_events_vec;
    const cl_event* wait_events_ptr = NULL;

    cl_event event_id = NULL;
    cl_event* event_id_ptr = NULL;

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
                            const CLEventList *wait_events, CLEvent *event) throw(CLException&)
{
    cl_bool clblocking = CL_FALSE;

    QVector<cl_event> wait_events_vec;
    const cl_event* wait_events_ptr = NULL;

    cl_event event_id = NULL;
    cl_event* event_id_ptr = NULL;

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
                          cl_int *err_code) throw(CLException&)
{
    cl_bool clblocking = CL_FALSE;

    QVector<cl_event> wait_events_vec;
    const cl_event* wait_events_ptr = NULL;

    cl_event event_id = NULL;
    cl_event* event_id_ptr = NULL;

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

bool CLBuffer::enqueueUnMap(const CLCommandQueue &queue, void *ptr, const CLEventList *wait_events, CLEvent *event) throw(CLException&)
{
    QVector<cl_event> wait_events_vec;
    const cl_event* wait_events_ptr = NULL;

    cl_event event_id = NULL;
    cl_event* event_id_ptr = NULL;

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
T CLBuffer::getInfoValue(cl_mem_info info) const throw(CLException&)
{
    T res;
    CL_ERR_THROW(clGetMemObjectInfo(m_id, info, sizeof(T),
                      static_cast<void*>(&res), NULL));
    return res;
}
