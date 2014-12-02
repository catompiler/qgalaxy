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

    void setCurrentPlatform(const CLPlatform& platform);
    CLPlatform currentPlatform() const;

    void setCurrentDevice(const CLDevice& device);
    CLDevice currentDevice() const;

    void setBodiesCount(size_t count);
    size_t bodiesCount() const;

    /**
     * @brief Получение шага симуляции.
     * @return Шаг симуляции.
     */
    float timeStep() const;

    /**
     * @brief Установка шага симуляции.
     * @param dt Шаг симуляции.
     */
    void setTimeStep(float dt);

private slots:
    void on_cbPlatform_currentIndexChanged(int index);
    void on_cbDevice_currentIndexChanged(int index);

private:
    Ui::OCLSettingsDialog *ui;

    int platformIndex(const CLPlatform& platform);
    int deviceIndex(const CLDevice& device);

    void refreshUi();
    void populateLists();
    void populatePlatforms();
    void populateDevices(const CLPlatform& platform);
    void updateDeviceInfo(const CLDevice& device);
};

#endif // OCLSETTINGSDIALOG_H
