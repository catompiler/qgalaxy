#ifndef POINT3F_H
#define POINT3F_H

#include <QVector3D>

struct Point3f
{
    Point3f();
    Point3f(float arg_x, float arg_y, float arg_z);

    QVector3D toVector3D() const;
    static Point3f fromQVector3D(const QVector3D& v);

    float x;
    float y;
    float z;
};

#endif // POINT3F_H
