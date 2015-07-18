#include "settings.h"
#include <QSettings>
#include <QVariant>


static const char* company_name = "artem.lab";
static const char* app_name = "QGalaxy";
static const char* param_bodies_count_name = "bodies_count";
static const char* param_cl_platform_name = "cl_platform_name";
static const char* param_cl_device_name = "cl_device_name";
static const char* param_time_step_name = "time_step";

static const char* param_star_mass_min = "star_mass_min";
static const char* param_star_mass_max = "star_mass_max";
static const char* param_bh_mass_min = "bh_mass_min";
static const char* param_bh_mass_max = "bh_mass_max";

static const char* param_single_radius = "single_radius";
static const char* param_radius_min = "radius_min";
static const char* param_radius_max = "radius_max";
static const char* param_distance_max = "distance_max";
static const char* param_velocity_max = "velocity_max";


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
    bodies_count = settings.value(param_bodies_count_name, 1024U).toUInt();
    cl_platform_name = settings.value(param_cl_platform_name, QString()).toString();
    cl_device_name = settings.value(param_cl_device_name, QString()).toString();
    time_step = settings.value(param_time_step_name, 100000.0f).toFloat();

    star_mass_min = settings.value(param_star_mass_min, 1e0f).toFloat();
    star_mass_max = settings.value(param_star_mass_max, 1e2f).toFloat();
    bh_mass_min = settings.value(param_bh_mass_min, 1e6f).toFloat();
    bh_mass_max = settings.value(param_bh_mass_max, 1e7f).toFloat();

    single_radius = settings.value(param_single_radius, 2000.0f).toFloat();
    radius_min = settings.value(param_radius_min, 1000.0f).toFloat();
    radius_max = settings.value(param_radius_max, 2000.0f).toFloat();
    distance_max = settings.value(param_distance_max, 1000.0f).toFloat();
    velocity_max = settings.value(param_velocity_max, 0.00001f).toFloat();
}

void Settings::write()
{
    QSettings settings(company_name, app_name);
    settings.setValue(param_bodies_count_name, static_cast<unsigned int>(bodies_count));
    settings.setValue(param_cl_platform_name, cl_platform_name);
    settings.setValue(param_cl_device_name, cl_device_name);
    settings.setValue(param_time_step_name, time_step);

    settings.setValue(param_star_mass_min, star_mass_min);
    settings.setValue(param_star_mass_max, star_mass_max);
    settings.setValue(param_bh_mass_min, bh_mass_min);
    settings.setValue(param_bh_mass_max, bh_mass_max);

    settings.setValue(param_single_radius, single_radius);
    settings.setValue(param_radius_min, radius_min);
    settings.setValue(param_radius_max, radius_max);
    settings.setValue(param_distance_max, distance_max);
    settings.setValue(param_velocity_max, velocity_max);
}

size_t Settings::bodiesCount() const
{
    return bodies_count;
}

void Settings::setBodiesCount(size_t bodies)
{
    bodies_count = bodies;
    emit settingsChanged();
}

QString Settings::clPlatformName() const
{
    return cl_platform_name;
}

void Settings::setClPlatformName(const QString &name)
{
    cl_platform_name = name;
    emit settingsChanged();
}

QString Settings::clDeviceName() const
{
    return cl_device_name;
}

void Settings::setClDeviceName(const QString &name)
{
    cl_device_name = name;
    emit settingsChanged();
}

float Settings::timeStep() const
{
    return time_step;
}

void Settings::setTimeStep(float dt)
{
    time_step = dt;
    emit settingsChanged();
}

float Settings::starMassMin() const
{
    return star_mass_min;
}

void Settings::setStarMassMin(float mass)
{
    star_mass_min = mass;
    emit settingsChanged();
}

float Settings::starMassMax() const
{
    return star_mass_max;
}

void Settings::setStarMassMax(float mass)
{
    star_mass_max = mass;
    emit settingsChanged();
}

float Settings::bhMassMin() const
{
    return bh_mass_min;
}

void Settings::setBhMassMin(float mass)
{
    bh_mass_min = mass;
    emit settingsChanged();
}

float Settings::bhMassMax() const
{
    return bh_mass_max;
}

void Settings::setBhMassMax(float mass)
{
    bh_mass_max = mass;
    emit settingsChanged();
}

float Settings::singleRadius() const
{
    return single_radius;
}

void Settings::setSingleRadius(float radius)
{
    single_radius = radius;
    emit settingsChanged();
}

float Settings::radiusMin() const
{
    return radius_min;
}

void Settings::setRadiusMin(float radius)
{
    radius_min = radius;
    emit settingsChanged();
}

float Settings::radiusMax() const
{
    return radius_max;
}

void Settings::setRadiusMax(float radius)
{
    radius_max = radius;
    emit settingsChanged();
}

float Settings::distanceMax() const
{
    return distance_max;
}

void Settings::setDistanceMax(float dist)
{
    distance_max = dist;
    emit settingsChanged();
}

float Settings::velocityMax() const
{
    return velocity_max;
}

void Settings::setVelocityMax(float vel)
{
    velocity_max = vel;
    emit settingsChanged();
}
