#include "settings.h"
#include <QSettings>
#include <QVariant>


static const char* company_name = "artem.lab";
static const char* app_name = "QGalaxy";
static const char* param_bodies_count = "bodies_count";
static const char* param_cl_platform_name = "cl_platform_name";
static const char* param_cl_device_name = "cl_device_name";


Settings::Settings() :
    QObject()
{
}

Settings::~Settings()
{
}

Settings &Settings::get()
{
    static Settings settings_inst;
    return settings_inst;
}

void Settings::read()
{
    QSettings settings(company_name, app_name);
    _bodies_count = settings.value(param_bodies_count, 100U).toUInt();
    _cl_platform_name = settings.value(param_cl_platform_name, QString()).toString();
    _cl_device_name = settings.value(param_cl_device_name, QString()).toString();
}

void Settings::write()
{
    QSettings settings(company_name, app_name);
    settings.setValue(param_bodies_count, static_cast<unsigned int>(_bodies_count));
    settings.setValue(param_cl_platform_name, _cl_platform_name);
    settings.setValue(param_cl_device_name, _cl_device_name);
}

size_t Settings::bodiesCount() const
{
    return _bodies_count;
}

void Settings::setBodiesCount(size_t bodies_count_)
{
    _bodies_count = bodies_count_;
    emit settingsChanged();
}

QString Settings::clPlatformName() const
{
    return _cl_platform_name;
}

void Settings::setClPlatformName(const QString &name_)
{
    _cl_platform_name = name_;
    emit settingsChanged();
}

QString Settings::clDeviceName() const
{
    return _cl_device_name;
}

void Settings::setClDeviceName(const QString &name_)
{
    _cl_device_name = name_;
    emit settingsChanged();
}
