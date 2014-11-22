#include "clplatform.h"
#include "cldevice.h"

CLDevice::CLDevice()
{
    m_id = nullptr;
}

CLDevice::CLDevice(const cl_device_id &device_id)
{
    m_id = device_id;
}

CLDevice::CLDevice(const CLDevice &device)
{
    m_id = device.m_id;
}

CLDevice::~CLDevice()
{
}

cl_device_id CLDevice::id() const
{
    return m_id;
}

void CLDevice::setId(const cl_device_id &dev_id)
{
    m_id = dev_id;
}

bool CLDevice::isValid() const
{
    return m_id != nullptr;
}

cl_device_type CLDevice::type() const throw(CLException&)
{
    return getInfoValue<cl_device_type>(CL_DEVICE_TYPE);
}

cl_uint CLDevice::vendorId() const throw(CLException&)
{
    return getInfoValue<cl_uint>(CL_DEVICE_VENDOR_ID);
}

cl_uint CLDevice::maxComputeUnits() const throw(CLException&)
{
    return getInfoValue<cl_uint>(CL_DEVICE_MAX_COMPUTE_UNITS);
}

cl_uint CLDevice::maxWorkItemDimensions() const throw(CLException&)
{
    return getInfoValue<cl_uint>(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
}

QVector<size_t> CLDevice::maxWorkItemSizes() const throw(CLException&)
{
    return getInfoValuev<size_t>(CL_DEVICE_MAX_WORK_ITEM_SIZES);
}

size_t CLDevice::maxWorkGroupSize() const throw(CLException&)
{
    return getInfoValue<size_t>(CL_DEVICE_MAX_WORK_GROUP_SIZE);
}

cl_uint CLDevice::maxClockFrequency() const throw(CLException&)
{
    return getInfoValue<cl_uint>(CL_DEVICE_MAX_CLOCK_FREQUENCY);
}

cl_uint CLDevice::addressBits() const throw(CLException&)
{
    return getInfoValue<cl_uint>(CL_DEVICE_ADDRESS_BITS);
}

cl_ulong CLDevice::maxMemAllocSize() const throw(CLException&)
{
    return getInfoValue<cl_ulong>(CL_DEVICE_MAX_MEM_ALLOC_SIZE);
}

size_t CLDevice::maxParameterSize() const throw(CLException&)
{
    return getInfoValue<size_t>(CL_DEVICE_MAX_PARAMETER_SIZE);
}

cl_uint CLDevice::memBaseAddrAlign() const throw(CLException&)
{
    return getInfoValue<cl_uint>(CL_DEVICE_MEM_BASE_ADDR_ALIGN);
}

cl_uint CLDevice::minDataTypeAlignSize() const throw(CLException&)
{
    return getInfoValue<cl_uint>(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE);
}

cl_device_fp_config CLDevice::singleFpConfig() const throw(CLException&)
{
    return getInfoValue<cl_device_fp_config>(CL_DEVICE_SINGLE_FP_CONFIG);
}

cl_device_mem_cache_type CLDevice::globalMemCacheType() const throw(CLException&)
{
    return getInfoValue<cl_device_mem_cache_type>(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE);
}

cl_uint CLDevice::globalMemCachelineSize() const throw(CLException&)
{
    return getInfoValue<cl_uint>(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
}

cl_uint CLDevice::globalMemCacheSize() const throw(CLException&)
{
    return getInfoValue<cl_uint>(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
}

cl_ulong CLDevice::globalMemSize() const throw(CLException&)
{
    return getInfoValue<cl_ulong>(CL_DEVICE_GLOBAL_MEM_SIZE);
}

cl_ulong CLDevice::maxConstantBufferSize() const throw(CLException&)
{
    return getInfoValue<cl_ulong>(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
}

cl_uint CLDevice::maxConstantArgs() const throw(CLException&)
{
    return getInfoValue<cl_uint>(CL_DEVICE_MAX_CONSTANT_ARGS);
}

cl_device_local_mem_type CLDevice::localMemType() const throw(CLException&)
{
    return getInfoValue<cl_device_local_mem_type>(CL_DEVICE_LOCAL_MEM_TYPE);
}

cl_ulong CLDevice::localMemSize() const throw(CLException&)
{
    return getInfoValue<cl_ulong>(CL_DEVICE_LOCAL_MEM_SIZE);
}

cl_bool CLDevice::errorCorrectionSupport() const throw(CLException&)
{
    return getInfoValue<cl_bool>(CL_DEVICE_ERROR_CORRECTION_SUPPORT);
}

cl_bool CLDevice::hostUnifiedMemory() const throw(CLException&)
{
    return getInfoValue<cl_bool>(CL_DEVICE_HOST_UNIFIED_MEMORY);
}

cl_bool CLDevice::endianLittle() const throw(CLException&)
{
    return getInfoValue<cl_bool>(CL_DEVICE_ENDIAN_LITTLE);
}

cl_bool CLDevice::available() const throw(CLException&)
{
    return getInfoValue<cl_bool>(CL_DEVICE_AVAILABLE);
}

cl_bool CLDevice::compilerAvailable() const throw(CLException&)
{
    return getInfoValue<cl_bool>(CL_DEVICE_COMPILER_AVAILABLE);
}

cl_device_exec_capabilities CLDevice::executionCapabilities() const throw(CLException&)
{
    return getInfoValue<cl_device_exec_capabilities>(CL_DEVICE_EXECUTION_CAPABILITIES);
}

cl_command_queue_properties CLDevice::queueProperties() const throw(CLException&)
{
    return getInfoValue<cl_command_queue_properties>(CL_DEVICE_QUEUE_PROPERTIES);
}

cl_platform_id CLDevice::platformId() const throw(CLException&)
{
    return getInfoValue<cl_platform_id>(CL_DEVICE_PLATFORM);
}

QString CLDevice::name() const throw(CLException&)
{
    return getInfoValueStr(CL_DEVICE_NAME);
}

QString CLDevice::vendor() const throw(CLException&)
{
    return getInfoValueStr(CL_DEVICE_VENDOR);
}

QString CLDevice::driverVersion() const throw(CLException&)
{
    return getInfoValueStr(CL_DRIVER_VERSION);
}

QString CLDevice::profile() const throw(CLException&)
{
    return getInfoValueStr(CL_DEVICE_PROFILE);
}

QString CLDevice::version() const throw(CLException&)
{
    return getInfoValueStr(CL_DEVICE_VERSION);
}

QString CLDevice::deviceCVersion() const throw(CLException&)
{
    return getInfoValueStr(CL_DEVICE_OPENCL_C_VERSION);
}

QStringList CLDevice::extensions() const throw(CLException&)
{
    return getInfoValueStr(CL_DEVICE_EXTENSIONS).split(" ", QString::SkipEmptyParts);
}

bool CLDevice::hasExtension(const QString &ext_name) const throw(CLException&)
{
    return extensions().contains(ext_name);
}

CLDevice &CLDevice::operator =(const CLDevice &device)
{
    m_id = device.m_id;
    return *this;
}

bool CLDevice::operator ==(const CLDevice &device) const
{
    return m_id == device.m_id;
}

QString CLDevice::getInfoValueStr(cl_device_info info) const
{
    size_t len = 0;

    CL_ERR_THROW(clGetDeviceInfo(m_id, info, 0, nullptr, &len));

    char res_str[len];

    CL_ERR_THROW(clGetDeviceInfo(m_id, info, len,
                        static_cast<void*>(res_str), nullptr));

    return QString(res_str);
}

template<class T>
T CLDevice::getInfoValue(cl_device_info info) const
{
    T res;
    CL_ERR_THROW(clGetDeviceInfo(m_id, info, sizeof(T),
                        static_cast<void*>(&res), nullptr));
    return res;
}

template<class T>
QVector<T> CLDevice::getInfoValuev(cl_device_info info) const
{
    size_t size = 0;

    CL_ERR_THROW(clGetDeviceInfo(m_id, info, 0, nullptr, &size));

    size_t vec_size = size / sizeof(T);
    if(vec_size == 0) vec_size = 1;

    QVector<T> res(vec_size);

    CL_ERR_THROW(clGetDeviceInfo(m_id, info, size,
                        static_cast<void*>(res.data()), nullptr));

    return res;
}
