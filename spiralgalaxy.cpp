#include "spiralgalaxy.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "utils.h"
#include "point3f.h"
//#include <QDebug>

const qreal SpiralGalaxy::depth_div_radius = 0.15;
const qreal SpiralGalaxy::min_radius_k = 0.015;


using namespace utils;


#define GALAXY_EXP_GEN


SpiralGalaxy::SpiralGalaxy()
    :Galaxy()
{
}

/**
 * @brief Функция генерации галактики.
 * @return true в случае успеха, иначе false.
 */
bool SpiralGalaxy::generate()
{
    // Минимальный радиус орбиты звезды.
    const qreal min_radius = min_radius_k * m_radius;
    // Доступный для выбора размер орбиты.
    const qreal avail_dradius = m_radius - min_radius;

    // Вектор направления вверх.
    const QVector3D galaxy_up = QVector3D(0.0, 1.0, 0.0);

    // Вектор направления вправо.
    const QVector3D galaxy_right = QVector3D(1.0, 0.0, 0.0);

    // Случайно выберем число спиралей (2 или 4).
    const size_t spirals_count = (rand() % 2 + 1) * 2;

    // Число других взёзд = число звёзд - текущая звезда - чёрная дыра в центре.
    const size_t other_stars_count = m_stars_count - 2;
    // Средняя масса звезды.
    const qreal average_mass = (m_star_mass_min + m_star_mass_max) * 0.5f;

    // Средний эксцентриситет эллиптической орбиты.
    #define DEFAULT_ECCENTRICITY 0.35 //0.35
    // Случайно выберем эксцентриситет орбит звёзд.
    const qreal ellipse_eccentricity = getRandsf() * 0.1 + DEFAULT_ECCENTRICITY;

    // Полный круг.
    const qreal _2pi = 2.0 * M_PI;
    // Случайно выберем угол, на который закручиваются спиральные рукава.
    const qreal angle_max = 1.0 * M_PI + _2pi * getRanduf() / (spirals_count / 2);
    // Случайно выберем отклонение орбит звёзд от спиралей.
    const qreal angle_delta = (0.125 + getRandsf() * 0.005) * angle_max / spirals_count;

    //qDebug() << "spirals_count" << spirals_count << "angle_max" << angle_max << "angle_delta" << angle_delta;

    // Угол орбиты в галактике.
    qreal orbit_angle;
    // Угол (фаза) звезды на орбите.
    qreal orbit_phase_angle;

    // Афелий орбиты.
    qreal r_a;

    // Максимальная высота звезды над диском.
    qreal max_h;
    // Высота орбиты звезды.
    qreal h;
    // Угол наклона орбиты.
    qreal alpha;

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

    // Кватернион вращения орбиты.
    QQuaternion orbit_quat;
    // Кватернион наклона орбиты.
    QQuaternion orbit_angle_quat;

    // Масса звёзд внутри орбиты звезды.
    qreal mass_stars_in_radius;
    // Суммарная масса.
    qreal sum_mass;

    // Орбитальная скорость.
    qreal v;

    // Касательная к орбите в точке положения звезды.
    QVector3D tangent;
    // Позиция звезды.
    QVector3D pos;
    // Вектор скорости звезды.
    QVector3D vel;

    // Если число звёзд равно нулю - нечего генерировать.
    if(m_stars_count == 0) return false;

    // Изменим размер векторов.
    resizeVectors();

    // Нулевое тело - чёрная дыра.
    // Установим её параметры.
    (*m_stars_masses)[0] = m_black_hole_mass;
    Point3f::vector3dToPoint3f((*m_stars_positons)[0], m_position);
    Point3f::vector3dToPoint3f((*m_stars_velosities)[0], m_velocity);

    // Для каждой звезды в галактике.
    for(size_t i = 1; i < m_stars_count; i ++){
        // Масса.
        (*m_stars_masses)[i] = lerp(m_star_mass_min, m_star_mass_max, getRanduf());
        // Расстояние до центрального тела.
#ifdef GALAXY_EXP_GEN
        r_a = min_radius + (/**/1.0 - sqrt/**/(getRanduf())) * avail_dradius;
#else
        r_a = min_radius + getRanduf() * avail_dradius;
#endif

        // Высота над диском.
        max_h = depth_div_radius * m_radius * pow(1.0 - r_a/m_radius, 2.5);
        // Случайно выберем высоту.
        h = max_h * getRandsf();

        // Параметры эллипса орбиты.
        ellipse_a = r_a / (1.0 + ellipse_eccentricity);
        ellipse_b = ellipse_a * sqrt(1.0 - ellipse_eccentricity * ellipse_eccentricity);
        ellipse_c = ellipse_a * ellipse_eccentricity;

        // Угол поворота орбиты.
        orbit_angle = lerp(static_cast<qreal>(0.0), static_cast<qreal>(angle_max),
                    ((r_a - min_radius) / avail_dradius));
        orbit_angle += getRandsf() * angle_delta;
        orbit_angle += rand() % spirals_count * (_2pi / spirals_count);

        // Случайно выберем фазу.
        orbit_phase_angle = getRanduf() * _2pi;

        // Позиция звезды на орбите.
        local_x = ellipse_a * cos(orbit_phase_angle);
        local_y = ellipse_b * sin(orbit_phase_angle);

        // Магия.
        // Вычисление второй точки, находящейся на касательной к эллипсу.
        // см. формулы касательной к эллипсу.
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

        alpha = atan(h / r_a);

        // Установим позицию.
        pos.setX(local_x + ellipse_c);
        pos.setY(0); //h
        pos.setZ(local_y);

        // Установим вектор касательной.
        tangent.setX(local_tangent_x - local_x);
        tangent.setY(0); //-h
        tangent.setZ(local_tangent_y - local_y);

        // Кватернион поворота орбиты.
        orbit_quat = QQuaternion::fromAxisAndAngle(galaxy_up, degrees(-orbit_angle));
        // Нормализуем.
        orbit_quat.normalize();

        // Кватернион наклона орбиты.
        orbit_angle_quat = QQuaternion::fromAxisAndAngle(galaxy_right, degrees(alpha));
        // Нормализуем.
        orbit_angle_quat.normalize();

        // Сложим вращения.
        orbit_quat *= orbit_angle_quat;

        // Повернём орбиту.
        pos = orbit_quat.rotatedVector(pos);

        // Повернём касательную.
        tangent = orbit_quat.rotatedVector(tangent);
        // Нормализуем.
        tangent.normalize();

        // расстояние до центрального тела.
        r_a = pos.length();

        //qDebug() << "e_a =" << ellipse_a << "e_b =" << ellipse_b << "e_c =" << ellipse_c;
        //qDebug() << "r_a =" << r_a;

        // Вычислим массу звёзд + ЧД внутри орбиты.

        mass_stars_in_radius = average_mass * other_stars_count *
#ifdef GALAXY_EXP_GEN
                (1.0 - sqrt(r_a / m_radius));
#else
                (r_a / m_radius);
#endif

        sum_mass = mass_stars_in_radius + m_black_hole_mass;

        // Вычислим орбитальную скорость.
        v = (G * sum_mass * (2.0 / r_a - 1.0 / ellipse_a));
        if(v < 0.0) v = -v;
        v = sqrt(v);

        //qDebug() << "v =" << v;

        // Вычислим вектор скорости.
        vel = tangent * v;

        // Поместим значения скорости и позиции в массивы.
        Point3f::vector3dToPoint3f((*m_stars_positons)[i], m_orientation.rotatedVector(pos) + m_position);
        Point3f::vector3dToPoint3f((*m_stars_velosities)[i], m_orientation.rotatedVector(vel) + m_velocity);
    }

    // Возврат успеха.
    return true;
}
