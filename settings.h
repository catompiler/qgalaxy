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
    void setBodiesCount(size_t bodies_count_);

    QString clPlatformName() const;
    void setClPlatformName(const QString& name_);

    QString clDeviceName() const;
    void setClDeviceName(const QString& name_);
    
signals:
    void settingsChanged();

public slots:

private:
    Settings();
    ~Settings();

    size_t _bodies_count;
    QString _cl_platform_name;
    QString _cl_device_name;
};

#endif // SETTINGS_H
