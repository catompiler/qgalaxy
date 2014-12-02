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
};

#endif // SETTINGS_H
