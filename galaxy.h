#ifndef GALAXY_H
#define GALAXY_H

#include <QVector>
#include <QVector3D>
#include <QQuaternion>
#include "point3f.h"



class Galaxy
{
public:
    Galaxy();
    virtual ~Galaxy();

    virtual bool generate() = 0;

    const QVector<float>& starsMasses() const;
    const QVector<Point3f>& starsPositons() const;
    const QVector<Point3f>& starsVelosities() const;

    size_t starsCount() const;
    qreal minStarMass() const;
    qreal maxStarMass() const;
    qreal blackHoleMass() const;
    qreal radius() const;
    const QVector3D& velocity() const;
    const QVector3D& position() const;
    const QQuaternion& orientation() const;

    void setStarsCount(size_t count_);
    void setMinStarMass(qreal mass_min);
    void setMaxStarMass(qreal mass_max_);
    void setBlackHoleMass(qreal black_hole_mass);
    void setRadius(qreal radius);
    void setVelocity(const QVector3D& vel);
    void setPosition(const QVector3D& pos);
    void setOrientation(const QQuaternion& orient);

protected:

    void resizeVectors();

    void vectorToPoint3f(Point3f& point, const QVector3D& vector);

    static qreal lerp(qreal a, qreal b, qreal t);
    static qreal getRanduf();
    static qreal getRandsf();
    static qreal radians(qreal degrs);
    static qreal degrees(qreal rads);

    QVector<float>* m_stars_masses;
    QVector<Point3f>* m_stars_positons;
    QVector<Point3f>* m_stars_velosities;

    size_t m_stars_count;
    qreal m_star_mass_min;
    qreal m_star_mass_max;
    qreal m_black_hole_mass;

    qreal m_radius;
    QVector3D m_position;
    QVector3D m_velocity;
    QQuaternion m_orientation;
};
#endif // GALAXY_H
