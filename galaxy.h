#ifndef GALAXY_H
#define GALAXY_H

#include <QVector>
#include <QVector3D>
#include <QQuaternion>
#include "point3f.h"


/**
 * @brief Класс галактики.
 * @class Galaxy.
 */
class Galaxy
{
public:
    /**
     * @brief Конструктор.
     */
    Galaxy();

    /**
     * @brief Деструктор.
     */
    virtual ~Galaxy();

    /**
     * @brief Функция генерации галактики.
     * @return true в случае успеха, иначе false.
     */
    virtual bool generate() = 0;

    /**
     * @brief Получение масс звёзд.
     * @return Массы звёзд.
     */
    const QVector<float>& starsMasses() const;

    /**
     * @brief Получение позиций звёзд.
     * @return Позиции звёзд.
     */
    const QVector<Point3f>& starsPositons() const;

    /**
     * @brief Получение векторов скоростей звёзд.
     * @return Векторы скоростей звёзд.
     */
    const QVector<Point3f>& starsVelosities() const;

    /**
     * @brief Получение числа звёзд.
     * @return Число звёзд.
     */
    size_t starsCount() const;

    /**
     * @brief Получение минимальной массы звезды.
     * @return Мимнимальная масса звезды.
     */
    qreal minStarMass() const;

    /**
     * @brief Получение максимальной массы звезды.
     * @return Максимальная масса звезды.
     */
    qreal maxStarMass() const;

    /**
     * @brief Получение массы чёрной дыры.
     * @return Масса чёрной дыры.
     */
    qreal blackHoleMass() const;

    /**
     * @brief Получение радиуса.
     * @return Радиус галактики.
     */
    qreal radius() const;

    /**
     * @brief Получение скорости галактики.
     * @return Скорость галактики.
     */
    const QVector3D& velocity() const;

    /**
     * @brief Получение позиции галактики.
     * @return Позиция галактики.
     */
    const QVector3D& position() const;

    /**
     * @brief Получение ориентации галактики.
     * @return Ориентация галактики.
     */
    const QQuaternion& orientation() const;

    /**
     * @brief Установка числа звёзд.
     * @param count Число звёзд.
     */
    void setStarsCount(size_t count);

    /**
     * @brief Установка минимальной массы звезды.
     * @param mass_min Мимнимальная масса звезды.
     */
    void setMinStarMass(qreal mass_min);

    /**
     * @brief Установка максимальной массы звезды.
     * @param mass_max Максимальная масса звезды.
     */
    void setMaxStarMass(qreal mass_max);

    /**
     * @brief Установка массы чёрной дыры.
     * @param black_hole_mass Масса чёрной дыры.
     */
    void setBlackHoleMass(qreal black_hole_mass);

    /**
     * @brief Установка радиуса.
     * @param radius Радиус галактики.
     */
    void setRadius(qreal radius);

    /**
     * @brief Установка скорости галактики.
     * @param vel Скорость галактики.
     */
    void setVelocity(const QVector3D& vel);

    /**
     * @brief Установка позиции галактики.
     * @param pos Позиция галактики.
     */
    void setPosition(const QVector3D& pos);

    /**
     * @brief Установка ориентации галактики.
     * @param orient Ориентация галактики.
     */
    void setOrientation(const QQuaternion& orient);

protected:

    /**
     * @brief Гравитационная постоянная.
     */
    static const qreal G;

    /**
     * @brief Изменение размера массивов.
     */
    void resizeVectors();

    /**
     * @brief Массив масс.
     */
    QVector<float>* m_stars_masses;

    /**
     * @brief Массив позиций.
     */
    QVector<Point3f>* m_stars_positons;

    /**
     * @brief Массив векторов скоростей.
     */
    QVector<Point3f>* m_stars_velosities;

    //! Число звёзд.
    size_t m_stars_count;

    //! Минимальная масса звезды.
    qreal m_star_mass_min;

    //! Максимальная масса звезды.
    qreal m_star_mass_max;

    //! Масса чёрной дыры.
    qreal m_black_hole_mass;

    //! Радиус галактики.
    qreal m_radius;

    //! Позиция галактики.
    QVector3D m_position;

    //! Вектор скорости галактики.
    QVector3D m_velocity;

    //! Ориентация галактики.
    QQuaternion m_orientation;
};
#endif // GALAXY_H
