#ifndef GENSETTINGSDIALOG_H
#define GENSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class GenSettingsDialog;
}

class GenSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenSettingsDialog(QWidget *parent = 0);
    ~GenSettingsDialog();

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

private:
    Ui::GenSettingsDialog *ui;
};

#endif // GENSETTINGSDIALOG_H
