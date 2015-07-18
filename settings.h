#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <stddef.h>
#include <QString>



class Settings : public QObject
{
    Q_OBJECT
public:

    static Settings& get();

    void read();
    void write();

    size_t bodiesCount() const;
    void setBodiesCount(size_t bodies);

    QString clPlatformName() const;
    void setClPlatformName(const QString& name);

    QString clDeviceName() const;
    void setClDeviceName(const QString& name);

    float timeStep() const;
    void setTimeStep(float dt);

    float starMassMin() const;
    void setStarMassMin(float mass);

    float starMassMax() const;
    void setStarMassMax(float mass);

    float bhMassMin() const;
    void setBhMassMin(float mass);

    float bhMassMax() const;
    void setBhMassMax(float mass);

    float singleRadius() const;
    void setSingleRadius(float radius);

    float radiusMin() const;
    void setRadiusMin(float radius);

    float radiusMax() const;
    void setRadiusMax(float radius);

    float distanceMax() const;
    void setDistanceMax(float dist);

    float velocityMax() const;
    void setVelocityMax(float vel);
    
signals:
    void settingsChanged();

public slots:

private:
    Settings();
    ~Settings();

    size_t bodies_count;
    QString cl_platform_name;
    QString cl_device_name;
    float time_step;

    float star_mass_min;
    float star_mass_max;
    float bh_mass_min;
    float bh_mass_max;

    float single_radius;
    float radius_max;
    float radius_min;
    float distance_max;
    float velocity_max;
};

#endif // SETTINGS_H
