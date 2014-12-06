#include "point3f.h"

Point3f::Point3f()
{
    x = 0; y = 0; z = 0;
}

Point3f::Point3f(float arg_x, float arg_y, float arg_z)
{
    x = arg_x; y = arg_y; z = arg_z;
}

QVector3D Point3f::toVector3D() const
{
    return QVector3D(x, y, z);
}

Point3f Point3f::fromQVector3D(const QVector3D &v)
{
    return Point3f(v.x(), v.y(), v.z());
}

void Point3f::vector3dToPoint3f(Point3f &point, const QVector3D &vector)
{
    point.x = vector.x();
    point.y = vector.y();
    point.z = vector.z();
}
