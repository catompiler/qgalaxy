#include "clprogram.h"
#include "clcontext.h"
#include "cldevice.h"
#include "clexception.h"
#include "utils.h"
#include <QString>
#include <QByteArray>


CLProgram::CLProgram()
{
    m_id = nullptr;
}

CLProgram::CLProgram(const cl_program &prog_id)
{
    m_id = prog_id;
}

CLProgram::CLProgram(const CLProgram &prog)
{
    m_id = prog.m_id;
}

CLProgram::~CLProgram()
{
}

cl_program CLProgram::id() const
{
    return m_id;
}

void CLProgram::setId(const cl_program &prog_id)
{
    m_id = prog_id;
}

bool CLProgram::isValid() const
{
    return m_id != nullptr;
}

bool CLProgram::create(const CLContext &clcxt, const QString &source, cl_int* err_code)
{
    cl_int res = CL_SUCCESS;

    QByteArray source_utf = source.toUtf8();
    const char* c_source = source_utf.data();

    m_id = clCreateProgramWithSource(clcxt.id(), 1, &c_source, nullptr, &res);

    if(err_code) *err_code = res;
    CL_ERR_THROW(res);

    return m_id != nullptr;
}

bool CLProgram::retain()
{
    CL_ERR_THROW(clRetainProgram(m_id));
    return true;
}

bool CLProgram::release()
{
    CL_ERR_THROW(clReleaseProgram(m_id));
    return true;
}

bool CLProgram::build(const CLDeviceList &devices, const QStringList &options, cl_int *err_code)
{
    if(devices.empty()) return false;

    QVector<cl_device_id> devices_vec;

    for(CLDeviceList::const_iterator it = devices.begin(); it != devices.end(); ++ it){
        devices_vec.push_back((*it).id());
    }

    QByteArray options_utf = options.join(" ").toUtf8();
    const char* options_cstr = options_utf.data();

    cl_int res = clBuildProgram(m_id, devices_vec.size(), devices_vec.data(), options_cstr, nullptr, nullptr);

    if(err_code) *err_code = res;

    CL_ERR_THROW(res);

    return true;
}

cl_build_status CLProgram::buildStatus(const CLDevice &device) const
{
    return getBuildInfoValue<cl_build_status>(device, CL_PROGRAM_BUILD_STATUS);
}

QString CLProgram::buildLog(const CLDevice &device) const
{
    return getBuildInfoValueStr(device, CL_PROGRAM_BUILD_LOG);
}

QString CLProgram::source() const
{
    return getInfoValueStr(CL_PROGRAM_SOURCE);
}

cl_context CLProgram::contextId() const
{
    return getInfoValue<cl_context>(CL_PROGRAM_CONTEXT);
}

size_t CLProgram::devicesCount() const
{
    return getInfoValue<cl_uint>(CL_PROGRAM_NUM_DEVICES);
}

CLDeviceList CLProgram::devices() const
{
    CLDeviceList res;
    QVector<cl_device_id> dev_ids = getInfoValuev<cl_device_id>(CL_PROGRAM_DEVICES);
    for(QVector<cl_device_id>::iterator it = dev_ids.begin(); it != dev_ids.end(); ++ it){
        res.push_back(CLDevice(*it));
    }
    return res;
}

CLProgram &CLProgram::operator =(const CLProgram &prog)
{
    m_id = prog.m_id;
    return *this;
}

bool CLProgram::operator ==(const CLProgram &prog) const
{
    return m_id == prog.m_id;
}

void CLProgram::unloadCompiler()
{
    clUnloadCompiler();
}

QString CLProgram::getInfoValueStr(cl_program_info info) const
{
    size_t len = 0;

    CL_ERR_THROW(clGetProgramInfo(m_id, info, 0, nullptr, &len));

    char res_str[len];

    CL_ERR_THROW(clGetProgramInfo(m_id, info, len,
                        static_cast<void*>(res_str), nullptr));

    return QString(res_str);
}

template<class T>
T CLProgram::getInfoValue(cl_program_info info) const
{
    T res;
    CL_ERR_THROW(clGetProgramInfo(m_id, info, sizeof(T),
                        static_cast<void*>(&res), nullptr));
    return res;
}

template<class T>
QVector<T> CLProgram::getInfoValuev(cl_program_info info) const
{
    size_t size = 0;

    CL_ERR_THROW(clGetProgramInfo(m_id, info, 0, nullptr, &size));

    size_t vec_size = size / sizeof(T);
    if(vec_size == 0) vec_size = 1;

    QVector<T> res(vec_size);

    CL_ERR_THROW(clGetProgramInfo(m_id, info, size,
                        static_cast<void*>(res.data()), nullptr));

    return res;
}

QString CLProgram::getBuildInfoValueStr(const CLDevice &device, cl_program_build_info info) const
{
    size_t len = 0;

    CL_ERR_THROW(clGetProgramBuildInfo(m_id, device.id(), info, 0, nullptr, &len));

    char res_str[len];

    CL_ERR_THROW(clGetProgramBuildInfo(m_id, device.id(), info, len,
                        static_cast<void*>(res_str), nullptr));

    return QString(res_str);
}

template<class T>
T CLProgram::getBuildInfoValue(const CLDevice &device, cl_program_build_info info) const
{
    T res;
    CL_ERR_THROW(clGetProgramBuildInfo(m_id, device.id(), info, sizeof(T),
                        static_cast<void*>(&res), nullptr));
    return res;
}
