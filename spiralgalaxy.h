#ifndef SPIRALGALAXY_H
#define SPIRALGALAXY_H

#include "galaxy.h"

class SpiralGalaxy
    :public Galaxy
{
public:
    SpiralGalaxy();

    /**
     * @brief Функция генерации галактики.
     * @return true в случае успеха, иначе false.
     */
    bool generate();
private:

    static const qreal depth_div_radius;
    static const qreal min_radius_k;
};

#endif // SPIRALGALAXY_H
