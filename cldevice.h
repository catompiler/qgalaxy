#ifndef CLDEVICE_H
#define CLDEVICE_H

#include <CL/opencl.h>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QVector>
#include "clexception.h"
#include "utils.h"


class CLDevice
{
public:
    CLDevice();
    explicit CLDevice(const cl_device_id& device_id);
    CLDevice(const CLDevice& device);
    ~CLDevice();

    cl_device_id id() const;
    void setId(const cl_device_id& dev_id);

    bool isValid() const;

    cl_device_type type() const throw(CLException&);

    cl_uint vendorId() const throw(CLException&);

    cl_uint maxComputeUnits() const throw(CLException&);

    cl_uint maxWorkItemDimensions() const throw(CLException&);
    QVector<size_t> maxWorkItemSizes() const throw(CLException&);
    size_t maxWorkGroupSize() const throw(CLException&);

    cl_uint maxClockFrequency() const throw(CLException&);

    cl_uint addressBits() const throw(CLException&);

    cl_ulong maxMemAllocSize() const throw(CLException&);

    size_t maxParameterSize() const throw(CLException&);

    cl_uint memBaseAddrAlign() const throw(CLException&);

    cl_uint minDataTypeAlignSize() const throw(CLException&);

    cl_device_fp_config singleFpConfig() const throw(CLException&);

    cl_device_mem_cache_type globalMemCacheType() const throw(CLException&);
    cl_uint globalMemCachelineSize() const throw(CLException&);
    cl_uint globalMemCacheSize() const throw(CLException&);

    cl_ulong globalMemSize() const throw(CLException&);

    cl_ulong maxConstantBufferSize() const throw(CLException&);
    cl_uint maxConstantArgs() const throw(CLException&);

    cl_device_local_mem_type localMemType() const throw(CLException&);
    cl_ulong localMemSize() const throw(CLException&);

    cl_bool errorCorrectionSupport() const throw(CLException&);

    cl_bool hostUnifiedMemory() const throw(CLException&);

    cl_bool endianLittle() const throw(CLException&);

    cl_bool available() const throw(CLException&);
    cl_bool compilerAvailable() const throw(CLException&);

    cl_device_exec_capabilities executionCapabilities() const throw(CLException&);

    cl_command_queue_properties queueProperties() const throw(CLException&);

    cl_platform_id platformId() const throw(CLException&);

    QString name() const throw(CLException&);
    QString vendor() const throw(CLException&);
    QString driverVersion() const throw(CLException&);
    QString profile() const throw(CLException&);
    QString version() const throw(CLException&);
    QString deviceCVersion() const throw(CLException&);
    QStringList extensions() const throw(CLException&);

    bool hasExtension(const QString &ext_name) const throw(CLException&);

    CLDevice& operator=(const CLDevice& device);
    bool operator==(const CLDevice& device) const;

private:
    cl_device_id m_id;
    QString getInfoValueStr(cl_device_info info) const throw(CLException&);

    template<class T>
    T getInfoValue(cl_device_info info) const throw(CLException&);

    template<class T>
    QVector<T> getInfoValuev(cl_device_info info) const throw(CLException&);
};


Q_DECLARE_METATYPE(CLDevice)


#endif // CLDEVICE_H
