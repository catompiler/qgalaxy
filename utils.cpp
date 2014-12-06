#include "utils.h"
#include <math.h>

namespace utils{

qreal lerp(qreal a, qreal b, qreal t)
{
    return a * (1.0 - t) + b * t;
}

qreal getRanduf()
{
    return static_cast<qreal>(rand() % RAND_MAX) / RAND_MAX;
}

qreal getRandsf()
{
    return getRanduf() * 2.0 - 1.0;
}

qreal getRandf(qreal a, qreal b)
{
    return a + getRanduf() * (b - a);
}

qreal radians(qreal degrs)
{
    return degrs * M_PI / 180.0;
}

qreal degrees(qreal rads)
{
    return rads * 180.0 / M_PI;
}

}
