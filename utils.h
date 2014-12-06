#ifndef UTILS_H
#define UTILS_H

#include <QtGlobal>


#define CONCAT_IMPL(a,b) a ## b
#define CONCAT(a,b) CONCAT_IMPL(a,b)
#define CL_ERR_THROW(func)\
    do{\
        cl_int CONCAT(res, __LINE__) = func;\
        if(CONCAT(res, __LINE__) != CL_SUCCESS){\
            throw(CLException::make(CONCAT(res, __LINE__)));\
        }\
    }while(0)

namespace utils{

/**
 * @brief Линейная интерполяция.
 * @param a Значение при 0.0.
 * @param b Значение при 1.0.
 * @param t Параметр интерполяции.
 * @return Интерполированное значение.
 */
qreal lerp(qreal a, qreal b, qreal t);

/**
 * @brief Генерация случайного числа в диапазоне [0.0; 1.0].
 * @return Случайное число.
 */
qreal getRanduf();

/**
 * @brief Генерация случайного числа в диапазоне [-1.0; 1.0].
 * @return Случайное число.
 */
qreal getRandsf();

/**
 * @brief Генерация случайного числа в диапазоне [a; b].
 * @param a Начало диапазона.
 * @param b Конец диапазона.
 * @return Случайное число.
 */
qreal getRandf(qreal a, qreal b);

/**
 * @brief Перевод градусов в радианы.
 * @param degrs Градусы.
 * @return Радианы.
 */
qreal radians(qreal degrs);

/**
 * @brief Перевод в градусы.
 * @param rads Радианы.
 * @return Градусы.
 */
qreal degrees(qreal rads);

}

#endif // UTILS_H
