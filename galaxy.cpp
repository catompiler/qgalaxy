#include "galaxy.h"
#include <math.h>


Galaxy::Galaxy()
{
    m_stars_count = 0;
    m_star_mass_min = 0.0;
    m_star_mass_max = 0.0;
    m_black_hole_mass = 0.0;

    m_radius = 0.0;

    m_stars_masses = new QVector<float>();
    m_stars_positons = new QVector<Point3f>();
    m_stars_velosities = new QVector<Point3f>();
}

Galaxy::~Galaxy()
{
    delete m_stars_velosities;
    delete m_stars_positons;
    delete m_stars_masses;
}

const QVector<float> &Galaxy::starsMasses() const
{
    return *m_stars_masses;
}

const QVector<Point3f> &Galaxy::starsPositons() const
{
    return *m_stars_positons;
}

const QVector<Point3f> &Galaxy::starsVelosities() const
{
    return *m_stars_velosities;
}

size_t Galaxy::starsCount() const
{
    return m_stars_count;
}

qreal Galaxy::minStarMass() const
{
    return m_star_mass_min;
}

qreal Galaxy::maxStarMass() const
{
    return m_star_mass_max;
}

qreal Galaxy::blackHoleMass() const
{
    return m_black_hole_mass;
}

qreal Galaxy::radius() const
{
    return m_radius;
}

const QVector3D& Galaxy::velocity() const
{
    return m_velocity;
}

const QVector3D& Galaxy::position() const
{
    return m_position;
}

const QQuaternion& Galaxy::orientation() const
{
    return m_orientation;
}

void Galaxy::setStarsCount(size_t count_)
{
    m_stars_count = count_;
}

void Galaxy::setMinStarMass(qreal mass_min)
{
    m_star_mass_min = mass_min;
}

void Galaxy::setMaxStarMass(qreal mass_max)
{
    m_star_mass_max = mass_max;
}

void Galaxy::setBlackHoleMass(qreal black_hole_mass)
{
    m_black_hole_mass = black_hole_mass;
}

void Galaxy::setRadius(qreal radius)
{
    m_radius = radius;
}

void Galaxy::setVelocity(const QVector3D& vel)
{
    m_velocity = vel;
}

void Galaxy::setPosition(const QVector3D& pos)
{
    m_position = pos;
}

void Galaxy::setOrientation(const QQuaternion& orient)
{
    m_orientation = orient;
}

void Galaxy::resizeVectors()
{
    if(m_stars_count == 0){
        m_stars_masses->clear();
        m_stars_positons->clear();
        m_stars_velosities->clear();
    }else{
        m_stars_masses->resize(m_stars_count);
        m_stars_positons->resize(m_stars_count);
        m_stars_velosities->resize(m_stars_count);
    }
}

void Galaxy::vectorToPoint3f(Point3f &point, const QVector3D &vector)
{
    point.x = vector.x();
    point.y = vector.y();
    point.z = vector.z();
}

qreal Galaxy::lerp(qreal a, qreal b, qreal t)
{
    return a * (1.0 - t) + b * t;
}

qreal Galaxy::getRanduf()
{
    return static_cast<qreal>(rand()%RAND_MAX) / RAND_MAX;
}

qreal Galaxy::getRandsf()
{
    return getRanduf() * 2.0 - 1.0;
}

qreal Galaxy::radians(qreal degrs)
{
    return degrs * M_PI / 180.0;
}

qreal Galaxy::degrees(qreal rads)
{
    return rads * 180.0 / M_PI;
}


