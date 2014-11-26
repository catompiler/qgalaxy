#include "spiralgalaxy.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>
//#include <QDebug>


const qreal SpiralGalaxy::G = 6.6354E4;
const qreal SpiralGalaxy::depth_div_radius = 0.1;
const qreal SpiralGalaxy::min_radius_k = 0.015;



SpiralGalaxy::SpiralGalaxy()
    :Galaxy()
{
}

bool SpiralGalaxy::generate()
{
    const qreal min_radius = min_radius_k * m_radius;
    const qreal avail_dradius = m_radius - min_radius;

    //QVector3D bh_pos = QVector3D(0.0,0.0,0.0);
    const QVector3D galaxy_up = QVector3D(0.0, 1.0, 0.0);

    const size_t spirals_count = (rand() % 3 + 1) * 2;

    const size_t other_stars_count = m_stars_count - 2;
    const qreal average_mass = (m_star_mass_min + m_star_mass_max) * 0.5f;

    #define DEFAULT_ECCENTRICITY 0.35
    const qreal ellipse_eccentricity = getRandsf() * 0.1 + DEFAULT_ECCENTRICITY;

    const qreal _2pi = 2.0 * M_PI;
    const qreal angle_max = 0.25 * M_PI * spirals_count + _2pi * getRanduf() / spirals_count;
    const qreal angle_delta = (0.125 + getRandsf() * 0.005) * angle_max / spirals_count;

    //qDebug() << "spirals_count" << spirals_count << "angle_max" << angle_max << "angle_delta" << angle_delta;

    qreal orbit_angle;
    qreal orbit_phase_angle;

    // Афелий орбиты.
    qreal r_a;

    qreal max_h;
    qreal h;

    // Большая полуось эллипса.
    qreal ellipse_a;
    // Малая полуось эллипса.
    qreal ellipse_b;
    // Фокальное расстояние.
    qreal ellipse_c;

    // Координаты на орбите.
    qreal local_x;
    qreal local_y;
    // Вторая точка касательной.
    qreal local_tangent_x;
    qreal local_tangent_y;

    QQuaternion orbit_quat;

    qreal mass_stars_in_radius;
    qreal sum_mass;

    qreal v;

    QVector3D tangent;
    QVector3D pos;
    QVector3D vel;

    resizeVectors();

    if(m_stars_count < 2) return false;

    (*m_stars_masses)[0] = m_black_hole_mass;
    vectorToPoint3f((*m_stars_positons)[0], m_position);
    vectorToPoint3f((*m_stars_velosities)[0], m_velocity);

    for(size_t i = 1; i < m_stars_count; i ++){
        // Масса.
        (*m_stars_masses)[i] = lerp(m_star_mass_min, m_star_mass_max, getRanduf());
        // Расстояние до центрального тела.
        //r_a = min_radius + (/**/1.0 - sqrt/**/(getRanduf())) * avail_dradius;
        r_a = min_radius + getRanduf() * avail_dradius;

        // Высота в апогее над диском.
        max_h = depth_div_radius * m_radius * pow(1.0 - r_a/m_radius, 2);
        h = max_h * getRandsf();

        // Параметры эллипса орбиты.
        ellipse_a = r_a / (1.0 + ellipse_eccentricity);
        ellipse_b = ellipse_a * sqrt(1.0 - ellipse_eccentricity * ellipse_eccentricity);
        ellipse_c = ellipse_a * ellipse_eccentricity;

        // Угол поворота орбиты.
        orbit_angle = lerp(static_cast<qreal>(0.0), static_cast<qreal>(angle_max),
                    (r_a - min_radius) / avail_dradius);
        orbit_angle += getRandsf() * angle_delta;
        orbit_angle += rand() % spirals_count * (_2pi / spirals_count);

        orbit_phase_angle = getRanduf() * _2pi;

        local_x = ellipse_a * cos(orbit_phase_angle);
        local_y = ellipse_b * sin(orbit_phase_angle);

        if(local_y != 0.0){
            if(local_y > 0.0){
                local_tangent_x = local_x + ellipse_a;
            }else{
                local_tangent_x = local_x - ellipse_a;
            }
            local_tangent_y = (1.0 - local_tangent_x * local_x / (ellipse_a * ellipse_a)) *
                              (ellipse_b * ellipse_b) / local_y;
        }else if(local_x != 0.0){
            if(local_x > 0.0){
                local_tangent_y = local_y - ellipse_b;
            }else{
                local_tangent_y = local_y + ellipse_b;
            }
            local_tangent_x = (1.0 - local_tangent_y * local_y / (ellipse_b * ellipse_b)) *
                              (ellipse_a * ellipse_a) / local_x;
        }else{
            local_tangent_x = getRandsf();
            local_tangent_y = getRandsf();
        }

        pos.setX(local_x + ellipse_c);
        pos.setY(h);
        pos.setZ(local_y);

        tangent.setX(local_tangent_x - local_x);
        tangent.setY(-h);
        tangent.setZ(local_tangent_y - local_y);

        // Кватернион поворота орбиты.
        orbit_quat = QQuaternion::fromAxisAndAngle(galaxy_up, degrees(-orbit_angle));
        orbit_quat.normalize();

        // Повернём орбиту.
        pos = orbit_quat.rotatedVector(pos);

        // Повернём касательную.
        tangent = orbit_quat.rotatedVector(tangent);
        // Нормализуем.
        tangent.normalize();

        r_a = pos.length();

        //qDebug() << "e_a =" << ellipse_a << "e_b =" << ellipse_b << "e_c =" << ellipse_c;
        //qDebug() << "r_a =" << r_a;

        //mass_stars_in_radius = average_mass * (stars_count - 1) * pow(r / _radius, 2.0);
        mass_stars_in_radius = average_mass * other_stars_count * r_a / m_radius;
        sum_mass = mass_stars_in_radius + m_black_hole_mass;

        //v = sqrt(G * sum_mass / r_a * (1.0 - ellipse_eccentricity));
        v = (G * sum_mass * (2.0 / r_a - 1.0 / ellipse_a));
        if(v < 0.0) v = -v;
        v = sqrt(v);

        //qDebug() << "v =" << v;

        vel = tangent * v;

        vectorToPoint3f((*m_stars_positons)[i], m_orientation.rotatedVector(pos) + m_position);
        vectorToPoint3f((*m_stars_velosities)[i], m_orientation.rotatedVector(vel) + m_velocity);
    }

    return true;
}
