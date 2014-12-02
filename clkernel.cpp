#include "clkernel.h"
#include "clprogram.h"
#include "cldevice.h"
#include "clexception.h"
#include "clcommandqueue.h"
#include "utils.h"
#include <QString>



CLKernel::CLKernel()
{
    m_id = nullptr;
}

CLKernel::CLKernel(const cl_kernel &kernel_id)
{
    m_id = kernel_id;
}

CLKernel::CLKernel(const CLKernel &kernel)
{
    m_id = kernel.m_id;
}

CLKernel::~CLKernel()
{
}

cl_kernel CLKernel::id() const
{
    return m_id;
}

void CLKernel::setId(const cl_kernel &kernel_id)
{
    m_id = kernel_id;
}

bool CLKernel::isValid() const
{
    return m_id != nullptr;
}

bool CLKernel::create(const CLProgram &program, const QString &kernel_func_name, cl_int* err_code)
{
    cl_int res = CL_SUCCESS;

    const char* c_kernel_name = kernel_func_name.toUtf8().data();

    m_id = clCreateKernel(program.id(), c_kernel_name, &res);

    if(err_code) *err_code = res;
    CL_ERR_THROW(res);

    return m_id != nullptr;
}

bool CLKernel::retain()
{
    CL_ERR_THROW(clRetainKernel(m_id));
    return true;
}

bool CLKernel::release()
{
    cl_uint refs_count = 0;
    try{ refs_count = getInfoValue<cl_uint>(CL_KERNEL_REFERENCE_COUNT); }catch(...){}

    CL_ERR_THROW(clReleaseKernel(m_id));

    if(refs_count == 1) m_id = nullptr;
    return true;
}

bool CLKernel::setLocalArgSize(size_t index, size_t size)
{
    CL_ERR_THROW(clSetKernelArg(m_id, index, size, nullptr));
    return true;
}

bool CLKernel::execute(const CLCommandQueue &queue, size_t work_dims, const size_t* global_work_size, const size_t* local_work_size, const CLEventList *wait_events, CLEvent *event)
{
    if(work_dims < 1 || work_dims > 3) return false;

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

    CL_ERR_THROW(clEnqueueNDRangeKernel(queue.id(), m_id, work_dims, nullptr, global_work_size, local_work_size, wait_events_vec.size(), wait_events_ptr, event_id_ptr));

    if(event) event->setId(event_id);

    return true;
}

cl_context CLKernel::contextId() const
{
    return getInfoValue<cl_context>(CL_KERNEL_CONTEXT);
}

cl_program CLKernel::programId() const
{
    return getInfoValue<cl_program>(CL_KERNEL_PROGRAM);
}

QString CLKernel::functionName() const
{
    return getInfoValueStr(CL_KERNEL_FUNCTION_NAME);
}

size_t CLKernel::argsCount() const
{
    return getInfoValue<cl_uint>(CL_KERNEL_NUM_ARGS);
}

size_t CLKernel::workGroupSize(const CLDevice &device) const
{
    return getWorkGroupInfoValue<size_t>(device, CL_KERNEL_WORK_GROUP_SIZE);
}

QVector<size_t> CLKernel::compileWorkGroupSize(const CLDevice &device) const
{
    return getWorkGroupInfoValuev<size_t>(device, CL_KERNEL_COMPILE_WORK_GROUP_SIZE);
}

size_t CLKernel::localMemSize(const CLDevice &device) const
{
    return getWorkGroupInfoValue<cl_ulong>(device, CL_KERNEL_LOCAL_MEM_SIZE);
}

CLKernel &CLKernel::operator =(const CLKernel &kernel)
{
    m_id = kernel.m_id;
    return *this;
}

bool CLKernel::operator ==(const CLKernel &kernel) const
{
    return m_id == kernel.m_id;
}

QString CLKernel::getInfoValueStr(cl_kernel_info info) const
{
    size_t len = 0;

    CL_ERR_THROW(clGetKernelInfo(m_id, info, 0, nullptr, &len));

    char res_str[len];

    CL_ERR_THROW(clGetKernelInfo(m_id, info, len,
                        static_cast<void*>(res_str), nullptr));

    return QString(res_str);
}

template<class T>
T CLKernel::getInfoValue(cl_kernel_info info) const
{
    T res;
    CL_ERR_THROW(clGetKernelInfo(m_id, info, sizeof(T),
                        static_cast<void*>(&res), nullptr));
    return res;
}

template<class T>
T CLKernel::getWorkGroupInfoValue(const CLDevice &device, cl_kernel_info info) const
{
    T res;
    CL_ERR_THROW(clGetKernelWorkGroupInfo(m_id, device.id(), info, sizeof(T),
                        static_cast<void*>(&res), nullptr));
    return res;
}

template<class T>
QVector<T> CLKernel::getWorkGroupInfoValuev(const CLDevice &device, cl_kernel_info info) const
{
    size_t size = 0;

    CL_ERR_THROW(clGetKernelWorkGroupInfo(m_id, device.id(), info, 0, nullptr, &size));

    size_t vec_size = size / sizeof(T);
    if(vec_size == 0) vec_size = 1;

    QVector<T> res(vec_size);

    CL_ERR_THROW(clGetKernelWorkGroupInfo(m_id, device.id(), info, size,
                        static_cast<void*>(res.data()), nullptr));

    return res;
}
