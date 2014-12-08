#ifndef POINT3F_H
#define POINT3F_H

#include <QVector3D>

/**
 * @brief Тип точки в трёхмерном пространстве.
 */
struct Point3f
{
    /**
     * @brief Конструктор.
     */
    Point3f();

    /**
     * @brief Конструктор.
     * @param arg_x Координата X.
     * @param arg_y Координата Y.
     * @param arg_z Координата Z.
     */
    Point3f(float arg_x, float arg_y, float arg_z);

    /**
     * @brief Преобразование точки в тип QVector3D;
     * @return Точка в формате QVector3D.
     */
    QVector3D toVector3D() const;

    /**
     * @brief Преобразование типа QVector3D в точку.
     * @param v Точка в формате QVector3D.
     * @return Точка.
     */
    static Point3f fromQVector3D(const QVector3D& v);

    /**
     * @brief Копирование данных из типа QVector3D в точку.
     * @param point Точка.
     * @param vector Точка в формате QVector3D.
     */
    static void vector3dToPoint3f(Point3f& point, const QVector3D& vector);

    float x; //!< Координата X.
    float y; //!< Координата Y.
    float z; //!< Координата Z.
};

#endif // POINT3F_H
