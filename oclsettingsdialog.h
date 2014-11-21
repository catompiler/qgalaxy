#ifndef OCLSETTINGSDIALOG_H
#define OCLSETTINGSDIALOG_H

#include <QDialog>
#include <QString>
#include <stddef.h>


class CLPlatform;
class CLDevice;


namespace Ui {
class OCLSettingsDialog;
}

class OCLSettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OCLSettingsDialog(QWidget *parent = 0);
    ~OCLSettingsDialog();

    void setCurrentPlatform(const CLPlatform& platform_);
    CLPlatform currentPlatform() const;

    void setCurrentDevice(const CLDevice& device_);
    CLDevice currentDevice() const;

    void setBodiesCount(size_t count_);
    size_t bodiesCount() const;

private slots:

private:
    Ui::OCLSettingsDialog *ui;

    int _platformIndex(const CLPlatform& platform_);
    int _deviceIndex(const CLDevice& device_);

    void _refreshUi();
    void _populateLists();
    void _populatePlatforms();
    void _populateDevices(const CLPlatform& platform_);
    void _updateDeviceInfo(const CLDevice& device_);
};

#endif // OCLSETTINGSDIALOG_H
