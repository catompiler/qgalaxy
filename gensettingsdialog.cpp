#include "gensettingsdialog.h"
#include "ui_gensettingsdialog.h"

GenSettingsDialog::GenSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenSettingsDialog)
{
    ui->setupUi(this);
    ui->sbBhMassMin->setMaximum(1e38);
    ui->sbBhMassMax->setMaximum(1e38);
    ui->sbStarMassMin->setMaximum(1e38);
    ui->sbStarMassMax->setMaximum(1e38);
}

GenSettingsDialog::~GenSettingsDialog()
{
    delete ui;
}

float GenSettingsDialog::starMassMin() const
{
    return ui->sbStarMassMin->value();
}

void GenSettingsDialog::setStarMassMin(float mass)
{
    ui->sbStarMassMin->setValue(mass);
}

float GenSettingsDialog::starMassMax() const
{
    return ui->sbStarMassMax->value();
}

void GenSettingsDialog::setStarMassMax(float mass)
{
    ui->sbStarMassMax->setValue(mass);
}

float GenSettingsDialog::bhMassMin() const
{
    return ui->sbBhMassMin->value();
}

void GenSettingsDialog::setBhMassMin(float mass)
{
    ui->sbBhMassMin->setValue(mass);
}

float GenSettingsDialog::bhMassMax() const
{
    return ui->sbBhMassMax->value();
}

void GenSettingsDialog::setBhMassMax(float mass)
{
    ui->sbBhMassMax->setValue(mass);
}

float GenSettingsDialog::singleRadius() const
{
    return ui->sbSingleRadius->value();
}

void GenSettingsDialog::setSingleRadius(float radius)
{
    ui->sbSingleRadius->setValue(radius);
}

float GenSettingsDialog::radiusMin() const
{
    return ui->sbRadiusMin->value();
}

void GenSettingsDialog::setRadiusMin(float radius)
{
    ui->sbRadiusMin->setValue(radius);
}

float GenSettingsDialog::radiusMax() const
{
    return ui->sbRadiusMax->value();
}

void GenSettingsDialog::setRadiusMax(float radius)
{
    ui->sbRadiusMax->setValue(radius);
}

float GenSettingsDialog::distanceMax() const
{
    return ui->sbDistMax->value();
}

void GenSettingsDialog::setDistanceMax(float dist)
{
    ui->sbDistMax->setValue(dist);
}

float GenSettingsDialog::velocityMax() const
{
    return ui->sbVelMax->value();
}

void GenSettingsDialog::setVelocityMax(float vel)
{
    ui->sbVelMax->setValue(vel);
}
