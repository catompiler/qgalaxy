#ifndef CLDEVICE_H
#define CLDEVICE_H

#include <CL/opencl.h>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QList>
#include <QVector>
#include "clexception.h"
#include "utils.h"

/**
 * @class CLDevice.
 * @brief Класс устройтва OpenCL.
 */
class CLDevice
{
public:
    /**
     * @brief Конструктор.
     */
    CLDevice();

    /**
     * @brief Конструктор.
     * @param device_id Идентификатор объекта устройства OpenCL.
     */
    explicit CLDevice(const cl_device_id& device_id);

    /**
     * @brief Конструктор копирования.
     * Создаёт копию объекта, но не устройства.
     * @param device Копируемый объект.
     */
    CLDevice(const CLDevice& device);

    /**
     * @brief Деструктор.
     */
    ~CLDevice();

    /**
     * @brief Получение идентификатор объекта устройства OpenCL.
     * @return Идентификатор объекта устройства OpenCL.
     */
    cl_device_id id() const;

    /**
     * @brief Установка идентификатора объекта устройства OpenCL.
     * @param dev_id Идентификатор объекта устройства OpenCL.
     */
    void setId(const cl_device_id& dev_id);

    /**
     * @brief Получение флага валидности устройства OpenCL.
     * @return Флаг валидности устройства OpenCL.
     */
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

    /**
     * @brief Оператор присваивания.
     * @param device Устройство OpenCL.
     * @return Ссылка на себя.
     */
    CLDevice& operator=(const CLDevice& device);

    /**
     * @brief Оператор сравнения.
     * @param device Устройство OpenCL.
     * @return true, если идентификаторы устройств равны, иначе false.
     */
    bool operator==(const CLDevice& device) const;

private:

    /**
     * @brief Идентификатор устройства OpenCL.
     */
    cl_device_id m_id;

    /**
     * @brief Получает текстовую информацию об устройстве OpenCL.
     * @param info Вид информации.
     * @return Текстовая информация об устройстве OpenCL.
     * @throw CLException в случае ошибки.
     */
    QString getInfoValueStr(cl_device_info info) const;

    /**
     * @brief Получает информацию об устройстве OpenCL.
     * @param info Вид информации.
     * @return Информация об устройстве OpenCL.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    T getInfoValue(cl_device_info info) const;

    /**
     * @brief Получает информацию об устройстве OpenCL в виде вектора.
     * @param info Вид информации.
     * @return Информация об устройстве OpenCL в виде вектора.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    QVector<T> getInfoValuev(cl_device_info info) const;
};


Q_DECLARE_METATYPE(CLDevice)

typedef QList<CLDevice> CLDeviceList;

#endif // CLDEVICE_H
