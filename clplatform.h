#ifndef CLPLATFORM_H
#define CLPLATFORM_H

#include <CL/opencl.h>
#include <stddef.h>
#include <QMetaType>
#include <QList>
#include <QString>
#include <QStringList>
#include "cldevice.h"
#include "vendor.h"
#include "clexception.h"


class CLPlatform
{
public:
    /**
     * @brief Конструктор.
     */
    CLPlatform();

    /**
     * @brief Конструктор.
     * @param platform_id Идентификатор объекта платформы OpenCL.
     */
    explicit CLPlatform(const cl_platform_id& platform_id);

    /**
     * @brief Конструктор копирования.
     * Создаёт копию объекта, но не платформы.
     * @param platform Копируемый объект.
     */
    CLPlatform(const CLPlatform& platform);

    /**
     * @brief Деструктор.
     */
    ~CLPlatform();

    /**
     * @brief Получение идентификатор объекта платформы OpenCL.
     * @return Идентификатор объекта платформы OpenCL.
     */
    cl_platform_id id() const;

    /**
     * @brief Установка идентификатора объекта платформы OpenCL.
     * @param platform_id Идентификатор объекта платформы OpenCL.
     */
    void setId(const cl_platform_id& platform_id);

    /**
     * @brief Получение флага валидности платформы OpenCL.
     * @return Флаг валидности платформы OpenCL.
     */
    bool isValid() const;

    QString profile() const throw(CLException&);
    QString version() const throw(CLException&);
    QString name() const throw(CLException&);
    QString vendor() const throw(CLException&);
    QStringList extensions() const throw(CLException&);

    vendor::id_t vendorId() const throw(CLException&);

    bool hasExtension(const QString& ext_name) const throw(CLException&);

    static size_t count() throw(CLException&);
    static QList<CLPlatform> getPlatforms() throw(CLException&);
    static CLPlatform byName(const QString& name_) throw(CLException&);

    size_t devicesCount() const throw(CLException&);
    size_t devicesCount(cl_device_type device_type) const throw(CLException&);
    QList<CLDevice> getDevices() const throw(CLException&);
    QList<CLDevice> getDevices(cl_device_type device_type) const throw(CLException&);
    CLDevice deviceByName(const QString& dev_name) const throw(CLException&);

    /**
     * @brief Оператор присваивания.
     * @param platform Платформа OpenCL.
     * @return Ссылка на себя.
     */
    CLPlatform& operator=(const CLPlatform& platform);

    /**
     * @brief Оператор сравнения.
     * @param platform Платформа OpenCL.
     * @return true, если идентификаторы платформ равны, иначе false.
     */
    bool operator==(const CLPlatform& platform) const;
private:

    /**
     * @brief Идентификатор платформы OpenCL.
     */
    cl_platform_id m_id;

    /**
     * @brief Получает текстовую информацию о платформе OpenCL.
     * @param info Вид информации.
     * @return Текстовая информация о платформе OpenCL.
     * @throw CLException в случае ошибки.
     */
    QString getInfoValueStr(cl_platform_info info) const;
};


Q_DECLARE_METATYPE(CLPlatform)

typedef QList<CLPlatform> CLPlatformList;

#endif // CLPLATFORM_H
