#include "clbuffer.h"
#include "clcontext.h"


CLBuffer::CLBuffer()
{
    _id = NULL;
}

CLBuffer::CLBuffer(const cl_mem &buf_id_)
{
    _id = buf_id_;
}

CLBuffer::CLBuffer(const CLBuffer &buffer_)
{
    _id = buffer_._id;
}

CLBuffer::~CLBuffer()
{
}

cl_mem CLBuffer::id() const
{
    return _id;
}

void CLBuffer::setId(const cl_mem &buf_id_)
{
    _id = buf_id_;
}

bool CLBuffer::isValid() const
{
    return _id != NULL;
}

bool CLBuffer::create(const CLContext &cxt_, cl_mem_flags flags_, size_t size_, void *host_ptr_, cl_int *err_code_) throw(CLException&)
{
    cl_int res = CL_SUCCESS;

    _id = clCreateBuffer(cxt_.id(), flags_, size_, host_ptr_, &res);
    if(err_code_) *err_code_ = res;

    CL_ERR_THROW(res);

    return _id != NULL;
}

bool CLBuffer::retain() throw(CLException&)
{
    CL_ERR_THROW(clRetainMemObject(_id));
    return true;
}

bool CLBuffer::release() throw(CLException&)
{
    CL_ERR_THROW(clReleaseMemObject(_id));
    return true;
}

cl_mem_flags CLBuffer::flags() const throw(CLException&)
{
    return _getInfoValue<cl_mem_flags>(CL_MEM_FLAGS);
}

size_t CLBuffer::size() const throw(CLException&)
{
    return _getInfoValue<size_t>(CL_MEM_SIZE);
}

void *CLBuffer::hostPtr() const throw(CLException&)
{
    return _getInfoValue<void*>(CL_MEM_HOST_PTR);
}

cl_context CLBuffer::contextId() const throw(CLException&)
{
    return _getInfoValue<cl_context>(CL_MEM_CONTEXT);
}

bool CLBuffer::enqueueRead(const CLCommandQueue &queue_, bool blocking_,
                           size_t offset_, size_t cb_, void *ptr_,
                           const CLEventList *wait_events_, CLEvent *event_) throw(CLException&)
{
    cl_bool blocking = CL_FALSE;

    QVector<cl_event> wait_events;
    const cl_event* wait_events_ptr = NULL;

    cl_event event_id = NULL;
    cl_event* event_id_ptr = NULL;

    if(blocking_) blocking = CL_TRUE;

    if(wait_events_){
        for(CLEventList::const_iterator it = wait_events_->begin(); it != wait_events_->end(); ++ it){
            wait_events.push_back((*it).id());
        }
    }

    if(!wait_events.empty()) wait_events_ptr = wait_events.data();

    if(event_) event_id_ptr = &event_id;

    CL_ERR_THROW(clEnqueueReadBuffer(queue_.id(), _id, blocking, offset_, cb_,
                    ptr_, wait_events.size(), wait_events_ptr, event_id_ptr));

    if(event_) event_->setId(event_id);

    return true;
}

bool CLBuffer::enqueueWrite(const CLCommandQueue &queue_, bool blocking_,
                            size_t offset_, size_t cb_, void *ptr_,
                            const CLEventList *wait_events_, CLEvent *event_) throw(CLException&)
{
    cl_bool blocking = CL_FALSE;

    QVector<cl_event> wait_events;
    const cl_event* wait_events_ptr = NULL;

    cl_event event_id = NULL;
    cl_event* event_id_ptr = NULL;

    if(blocking_) blocking = CL_TRUE;

    if(wait_events_){
        for(CLEventList::const_iterator it = wait_events_->begin(); it != wait_events_->end(); ++ it){
            wait_events.push_back((*it).id());
        }
    }

    if(!wait_events.empty()) wait_events_ptr = wait_events.data();

    if(event_) event_id_ptr = &event_id;

    CL_ERR_THROW(clEnqueueReadBuffer(queue_.id(), _id, blocking, offset_, cb_,
                        ptr_, wait_events.size(), wait_events_ptr, event_id_ptr));

    if(event_) event_->setId(event_id);

    return true;
}

void* CLBuffer::enqueueMap(const CLCommandQueue &queue_, bool blocking_,
                          cl_map_flags flags_, size_t offset_, size_t cb_,
                          const CLEventList *wait_events_, CLEvent *event_,
                          cl_int *err_code_) throw(CLException&)
{
    cl_bool blocking = CL_FALSE;

    QVector<cl_event> wait_events;
    const cl_event* wait_events_ptr = NULL;

    cl_event event_id = NULL;
    cl_event* event_id_ptr = NULL;

    if(blocking_) blocking = CL_TRUE;

    if(wait_events_){
        for(CLEventList::const_iterator it = wait_events_->begin(); it != wait_events_->end(); ++ it){
            wait_events.push_back((*it).id());
        }
    }

    if(!wait_events.empty()) wait_events_ptr = wait_events.data();

    if(event_) event_id_ptr = &event_id;

    cl_int res = CL_SUCCESS;
    void* res_ptr = clEnqueueMapBuffer(queue_.id(), _id, blocking, flags_, offset_, cb_,
                                       wait_events.size(), wait_events_ptr, event_id_ptr, &res);
    if(err_code_) *err_code_ = res;

    CL_ERR_THROW(res);

    if(res_ptr){
        if(event_) event_->setId(event_id);
    }

    return res_ptr;
}

bool CLBuffer::enqueueUnMap(const CLCommandQueue &queue_, void *ptr_, const CLEventList *wait_events_, CLEvent *event_) throw(CLException&)
{
    QVector<cl_event> wait_events;
    const cl_event* wait_events_ptr = NULL;

    cl_event event_id = NULL;
    cl_event* event_id_ptr = NULL;

    if(wait_events_){
        for(CLEventList::const_iterator it = wait_events_->begin(); it != wait_events_->end(); ++ it){
            wait_events.push_back((*it).id());
        }
    }

    if(!wait_events.empty()) wait_events_ptr = wait_events.data();

    if(event_) event_id_ptr = &event_id;

    CL_ERR_THROW(clEnqueueUnmapMemObject(queue_.id(), _id, ptr_,
            wait_events.size(), wait_events_ptr, event_id_ptr));

    if(event_) event_->setId(event_id);

    return true;
}

CLBuffer &CLBuffer::operator =(const CLBuffer &buffer_)
{
    _id = buffer_._id;
    return *this;
}

bool CLBuffer::operator ==(const CLBuffer &buffer_) const
{
    return _id == buffer_._id;
}
