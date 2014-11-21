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
    CLPlatform();
    explicit CLPlatform(const cl_platform_id& platform_id);
    CLPlatform(const CLPlatform& platform);
    ~CLPlatform();

    cl_platform_id id() const;
    void setId(const cl_platform_id& platform_id);

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

    CLPlatform& operator=(const CLPlatform& platform);
    bool operator==(const CLPlatform& platform) const;
private:
    cl_platform_id m_id;
    QString getInfoValueStr(cl_platform_info info_) const throw(CLException&);
};

Q_DECLARE_METATYPE(CLPlatform)

typedef QList<CLPlatform> CLPlatformList;
typedef QList<CLDevice> CLDeviceList;

#endif // CLPLATFORM_H
