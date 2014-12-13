#include "editbodydialog.h"
#include "ui_editbodydialog.h"
#include "nbodywidget.h"
#include <QVector>
#include "point3f.h"
#include "log.h"
#include <numeric>

#define LOG_WHO "EditBodyDialog"



EditBodyDialog::EditBodyDialog(NBodyWidget *nbodywgt, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditBodyDialog)
{
    ui->setupUi(this);

    double max_value = std::numeric_limits<double>::max();

    ui->dsbMass->setRange(0.0, max_value);

    ui->dsbPositionX->setRange(-max_value, max_value);
    ui->dsbPositionY->setRange(-max_value, max_value);
    ui->dsbPositionZ->setRange(-max_value, max_value);

    ui->dsbVelocityX->setRange(-max_value, max_value);
    ui->dsbVelocityY->setRange(-max_value, max_value);
    ui->dsbVelocityZ->setRange(-max_value, max_value);

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(buttonBoxClicked(QAbstractButton*)));

    nbodyWidget = nbodywgt;
}

EditBodyDialog::~EditBodyDialog()
{
    delete ui;
}

void EditBodyDialog::buttonBoxClicked(QAbstractButton *button)
{
    switch(ui->buttonBox->standardButton(button)){
    case QDialogButtonBox::Save:
        saveBody();
        break;
    default:
        accept();
        break;
    }
}

void EditBodyDialog::on_sbIndex_valueChanged(int /*i*/)
{
    refreshUi();
}

void EditBodyDialog::showEvent(QShowEvent* /*event*/)
{
    refreshUi();
}

void EditBodyDialog::refreshUi()
{
    size_t count = nbodyWidget->simulatedBodiesCount();//nbodyWidget->bodiesCount();
    if(count > 0){
        ui->sbIndex->setMaximum(count - 1);
    }else{
        ui->sbIndex->setMaximum(0);
    }
    ui->hsIndex->setRange(ui->sbIndex->minimum(), ui->sbIndex->maximum());

    QVector<float> mass(1);
    QVector<Point3f> pos(1), vel(1);

    if(!nbodyWidget->getBodies(ui->sbIndex->value(), 1, mass, pos, vel)){
        log(Log::WARNING, LOG_WHO, tr("Error getting body parameters %1").arg(ui->sbIndex->value()));
        return;
    }

    ui->dsbMass->setValue(mass[0]);

    ui->dsbPositionX->setValue(pos[0].x);
    ui->dsbPositionY->setValue(pos[0].y);
    ui->dsbPositionZ->setValue(pos[0].z);

    ui->dsbVelocityX->setValue(vel[0].x);
    ui->dsbVelocityY->setValue(vel[0].y);
    ui->dsbVelocityZ->setValue(vel[0].z);
}

void EditBodyDialog::saveBody()
{
    QVector<float> mass(1);
    QVector<Point3f> pos(1), vel(1);

    mass[0] = ui->dsbMass->value();

    pos[0] = Point3f(
                    ui->dsbPositionX->value(),
                    ui->dsbPositionY->value(),
                    ui->dsbPositionZ->value()
                );

    vel[0] = Point3f(
                    ui->dsbVelocityX->value(),
                    ui->dsbVelocityY->value(),
                    ui->dsbVelocityZ->value()
                );

    if(!nbodyWidget->setBodies(ui->sbIndex->value(), mass, pos, vel)){
        log(Log::WARNING, LOG_WHO, tr("Error setting body parameters %1").arg(ui->sbIndex->value()));
        return;
    }
}
