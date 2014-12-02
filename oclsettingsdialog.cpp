#include "oclsettingsdialog.h"
#include "ui_oclsettingsdialog.h"
#include <CL/opencl.h>
#include "clplatform.h"
#include "cldevice.h"
#include "vendor.h"
#include <QIcon>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVariant>
#include <QDebug>




OCLSettingsDialog::OCLSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OCLSettingsDialog)
{
    ui->setupUi(this);
    populateLists();
}

OCLSettingsDialog::~OCLSettingsDialog()
{
    delete ui;
}

void OCLSettingsDialog::setCurrentPlatform(const CLPlatform &platform)
{
    int indx = platformIndex(platform);
    ui->cbPlatform->setCurrentIndex(indx);
}

CLPlatform OCLSettingsDialog::currentPlatform() const
{
    int indx = ui->cbPlatform->currentIndex();
    if(indx == -1) return CLPlatform();
    return ui->cbPlatform->itemData(indx).value<CLPlatform>();
}

void OCLSettingsDialog::setCurrentDevice(const CLDevice &device)
{
    int indx = deviceIndex(device);
    ui->cbDevice->setCurrentIndex(indx);
}

CLDevice OCLSettingsDialog::currentDevice() const
{
    int indx = ui->cbDevice->currentIndex();
    if(indx == -1) return CLDevice();
    return ui->cbDevice->itemData(indx).value<CLDevice>();
}

void OCLSettingsDialog::setBodiesCount(size_t count)
{
    if(count > static_cast<size_t>(ui->sbBodiesCount->maximum())){
        ui->sbBodiesCount->setMaximum(count);
    }
    ui->sbBodiesCount->setValue(count);
}

size_t OCLSettingsDialog::bodiesCount() const
{
    return ui->sbBodiesCount->value();
}

float OCLSettingsDialog::timeStep() const
{
    return ui->dsbTimeStep->value();
}

void OCLSettingsDialog::setTimeStep(float dt)
{
    ui->dsbTimeStep->setValue(dt);
}

void OCLSettingsDialog::on_cbPlatform_currentIndexChanged(int index)
{
    if(index != -1){
        populateDevices(ui->cbPlatform->itemData(index).value<CLPlatform>());
    }
}

void OCLSettingsDialog::on_cbDevice_currentIndexChanged(int index)
{
    if(index != -1){
        updateDeviceInfo(ui->cbDevice->itemData(index).value<CLDevice>());
    }
}

int OCLSettingsDialog::platformIndex(const CLPlatform &platform)
{
    for(int i = 0; i < ui->cbPlatform->count(); i ++){
        if(ui->cbPlatform->itemData(i).value<CLPlatform>() == platform){
            return i;
        }
    }
    return -1;
}

int OCLSettingsDialog::deviceIndex(const CLDevice &device)
{
    for(int i = 0; i < ui->cbDevice->count(); i ++){
        if(ui->cbDevice->itemData(i).value<CLDevice>() == device){
            return i;
        }
    }
    return -1;
}

void OCLSettingsDialog::refreshUi()
{
    bool good_platform = false;
    bool good_device = false;

    CLPlatform platform = CLPlatform::byName(ui->cbPlatform->currentText());
    CLDevice device = platform.deviceByName(ui->cbDevice->currentText());

    good_platform = platform.isValid();
    good_device = device.isValid();

    QPushButton* pbOk = ui->buttonBox->button(QDialogButtonBox::Ok);
    if(pbOk){
        pbOk->setEnabled(good_platform && good_device);
    }
}

void OCLSettingsDialog::populateLists()
{
    populatePlatforms();
}

void OCLSettingsDialog::populatePlatforms()
{
    ui->cbPlatform->clear();
    CLPlatformList platforms = CLPlatform::getPlatforms();
    QIcon item_icon;
    for(CLPlatformList::iterator it = platforms.begin(); it != platforms.end(); ++ it){
        switch((*it).vendorId()){
        case vendor::id::nVidia:
            item_icon = QIcon(":/images/vendor_nvidia.png");
            break;
        case vendor::id::AMD:
            item_icon = QIcon(":/images/vendor_amd.png");
            break;
        case vendor::id::Intel:
            item_icon = QIcon(":/images/vendor_intel.png");
            break;
        default:
            item_icon = QIcon();
        }

        ui->cbPlatform->addItem(item_icon, (*it).name(),
                                QVariant::fromValue(*it));
    }
}

void OCLSettingsDialog::populateDevices(const CLPlatform& platform)
{
    ui->cbDevice->clear();
    if(!platform.isValid()) return;
    CLDeviceList devices = platform.getDevices();
    QIcon item_icon;
    for(CLDeviceList::iterator it = devices.begin(); it != devices.end(); ++ it){
        switch((*it).type()){
        case CL_DEVICE_TYPE_CPU:
            item_icon = QIcon(QString(":/images/device_cpu.png"));
            break;
        case CL_DEVICE_TYPE_GPU:
        case CL_DEVICE_TYPE_ACCELERATOR:
            item_icon = QIcon(QString(":/images/device_gpu.png"));
            break;
        default:
            item_icon = QIcon();
            break;
        }

        ui->cbDevice->addItem(item_icon, (*it).name(),
                              QVariant::fromValue(*it));
    }
}

void OCLSettingsDialog::updateDeviceInfo(const CLDevice &device)
{
    QString compUnits;
    QString clockFreq;
    QString exts;

    if(device.isValid()){
        compUnits = QString::number(device.maxComputeUnits());
        clockFreq = QString::number(device.maxClockFrequency());
        exts = device.extensions().join(", ");
    }

    ui->leDevCompUnits->setText(compUnits);
    ui->leDevClockFreq->setText(clockFreq);
    ui->pteDevExts->setPlainText(exts);
}
