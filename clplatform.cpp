#include "clplatform.h"
#include "utils.h"


CLPlatform::CLPlatform()
{
    _id = NULL;
}

CLPlatform::CLPlatform(const cl_platform_id &platform_id_)
{
    _id = platform_id_;
}

CLPlatform::CLPlatform(const CLPlatform &platform_)
{
    _id = platform_._id;
}

CLPlatform::~CLPlatform()
{
}

cl_platform_id CLPlatform::id() const
{
    return _id;
}

void CLPlatform::setId(const cl_platform_id &id_)
{
    _id = id_;
}

bool CLPlatform::isValid() const
{
    return _id != NULL;
}

QString CLPlatform::profile() const throw(CLException&)
{
    return _getInfoValueStr(CL_PLATFORM_PROFILE);
}

QString CLPlatform::version() const throw(CLException&)
{
    return _getInfoValueStr(CL_PLATFORM_VERSION);
}

QString CLPlatform::name() const throw(CLException&)
{
    return _getInfoValueStr(CL_PLATFORM_NAME);
}

QString CLPlatform::vendor() const throw(CLException&)
{
    return _getInfoValueStr(CL_PLATFORM_VENDOR);
}

QStringList CLPlatform::extensions() const throw(CLException&)
{
    return _getInfoValueStr(CL_PLATFORM_EXTENSIONS).split(" ", QString::SkipEmptyParts);
}

vendor::id_t CLPlatform::vendorId() const throw(CLException&)
{
    return vendor::id::byname(vendor());
}

bool CLPlatform::hasExtension(const QString &ext_name_) const throw(CLException&)
{
    return extensions().contains(ext_name_);
}

size_t CLPlatform::count() throw(CLException&)
{
    cl_uint platforms_count;
    CL_ERR_THROW(clGetPlatformIDs(0, NULL, &platforms_count));
    return platforms_count;
}

QList<CLPlatform> CLPlatform::getPlatforms() throw(CLException&)
{
    QList<CLPlatform> res;

    size_t platforms_count = count();

    if(platforms_count == 0) return res;

    cl_platform_id platform_ids[platforms_count];
    CL_ERR_THROW(clGetPlatformIDs(platforms_count, platform_ids, NULL));

    for(size_t i = 0; i < platforms_count; i ++){
        res.push_back(CLPlatform(platform_ids[i]));
    }
    return res;
}

CLPlatform CLPlatform::byName(const QString &name_) throw(CLException&)
{
    QList<CLPlatform> platforms = CLPlatform::getPlatforms();
    for(QList<CLPlatform>::iterator it = platforms.begin(); it != platforms.end(); ++ it){
        if((*it).name() == name_) return *it;
    }
    return CLPlatform();
}

CLDevice CLPlatform::deviceByName(const QString &name_) const throw(CLException&)
{
    QList<CLDevice> devices = getDevices();
    for(QList<CLDevice>::iterator it = devices.begin(); it != devices.end(); ++ it){
        if((*it).name() == name_) return *it;
    }
    return CLDevice();
}

size_t CLPlatform::devicesCount() const throw(CLException&)
{
    return devicesCount(CL_DEVICE_TYPE_ALL);
}

size_t CLPlatform::devicesCount(cl_device_type device_type_) const throw(CLException&)
{
    cl_uint devs_count;
    CL_ERR_THROW(clGetDeviceIDs(_id, device_type_, 0, NULL, &devs_count));
    return devs_count;
}

QList<CLDevice> CLPlatform::getDevices() const throw(CLException&)
{
    return getDevices(CL_DEVICE_TYPE_ALL);
}

QList<CLDevice> CLPlatform::getDevices(cl_device_type device_type_) const throw(CLException&)
{
    QList<CLDevice> res;

    size_t devs_count = devicesCount(device_type_);

    if(devs_count == 0) return res;

    cl_device_id dev_ids[devs_count];
    CL_ERR_THROW(clGetDeviceIDs(_id, device_type_, devs_count, dev_ids, NULL));

    for(size_t i = 0; i < devs_count; i ++){
        res.push_back(CLDevice(dev_ids[i]));
    }
    return res;
}

CLPlatform &CLPlatform::operator =(const CLPlatform &platform_)
{
    _id = platform_._id;
    return *this;
}

bool CLPlatform::operator ==(const CLPlatform &platform_) const
{
    return _id == platform_._id;
}

QString CLPlatform::_getInfoValueStr(cl_platform_info info_) const throw(CLException&)
{
    size_t len = 0;

    CL_ERR_THROW(clGetPlatformInfo(_id, info_, 0, NULL, &len));

    char res_str[len];

    CL_ERR_THROW(clGetPlatformInfo(_id, info_, len,
            static_cast<void*>(res_str), NULL));

    return QString(res_str);
}
