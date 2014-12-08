#ifndef CLGLNBODY_H
#define CLGLNBODY_H

#include <QObject>
#include <QVector>
#include <QVector3D>
#include <CL/opencl.h>
#include "point3f.h"

#ifdef CUSTOM_GLBUFFER
#include "glbuffer.h"
typedef GLBuffer NBodyGLBuffer;
#else
#include <QGLBuffer>
typedef QGLBuffer NBodyGLBuffer;
#endif


class CLPlatform;
class CLDevice;
class CLContext;
class CLBuffer;
class CLCommandQueue;
class CLProgram;
class CLKernel;
class CLEvent;


//! Число измерений.
#define NDRANGE_DIMENSIONS 1


/**
 * @class NBody.
 * @brief Ксласс симуляции гравитационного взаимодействия.
 */
class NBody : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор.
     * @param parent Объект-родитель.
     */
    explicit NBody(QObject *parent = 0);

    /**
     * @brief Деструкторю.
     */
    ~NBody();

    /**
     * @brief Получение числа тел.
     * @return Число тел.
     */
    size_t bodiesCount() const;

    /**
     * @brief Получение числа моделируемых тел.
     * @return Число моделируемых тел.
     */
    size_t simulatedBodiesCount() const;

    /**
     * @brief Установка числа моделируемых тел.
     * @param count Число моделируемых тел.
     * @return true в случае успеха, иначе false.
     */
    bool setSimulatedBodiesCount(size_t count);

    /**
     * @brief Получение шага симуляции.
     * @return Шаг симуляции.
     */
    float timeStep() const;

    /**
     * @brief Установка шага симуляции.
     * @param dt Шаг симуляции.
     */
    void setTimeStep(float dt);

    /**
     * @brief Получение контекста OpenCL.
     * @return Контекст OpenCL.
     */
    CLContext* clcontext();

    /**
     * @brief Инициализация систему NBody.
     * @param platform Платформа OpenCL.
     * @param device Устройство OpenCL.
     * @param bodies Число тел.
     * @return true в случае успеха, иначе false.
     */
    bool create(const CLPlatform &platform, const CLDevice &device, size_t bodies);

    /**
     * @brief Уничтожение системы NBody.
     * @return true в случае успеха, иначе false.
     */
    bool destroy();

    /**
     * @brief Сбрасывает параметры симуляции и тел.
     * @return true в случае успеха, иначе false.
     */
    bool reset();

    /**
     * @brief Получение флага готовности.
     * @return Флаг готовности.
     */
    bool isReady() const;

    /**
     * @brief Получение флага выполнения вычислений.
     * @return Флаг выполнения вычислений.
     */
    bool isRunning() const;

    /**
     * @brief Ждёт завершения симуляции.
     * @return true в случае успеха, иначе false.
     */
    bool wait() const;

    /**
     * @brief Установка значений масс объектов.
     * @param data Значения масс.
     * @param offset Смещение.
     * @return true в случае успеха, иначе false.
     */
    bool setMasses(const QVector<qreal>& data, size_t offset = 0);

    /**
     * @brief Установка значений масс объектов.
     * @param data Значения масс.
     * @param offset Смещение.
     * @return true в случае успеха, иначе false.
     */
    bool setMasses(const QVector<float>& data, size_t offset = 0);

    /**
     * @brief Установка значений позиций объектов.
     * @param data Значения позиций.
     * @param offset Смещение.
     * @return true в случае успеха, иначе false.
     */
    bool setPositions(const QVector<QVector3D>& data, size_t offset = 0);

    /**
     * @brief Установка значений позиций объектов.
     * @param data Значения позиций.
     * @param offset Смещение.
     * @return true в случае успеха, иначе false.
     */
    bool setPositions(const QVector<Point3f>& data, size_t offset = 0);

    /**
     * @brief Установка значений скоростей объектов.
     * @param data Значения скоростей.
     * @param offset Смещение.
     * @return true в случае успеха, иначе false.
     */
    bool setVelocities(const QVector<QVector3D>& data, size_t offset = 0);

    /**
     * @brief Установка значений скоростей объектов.
     * @param data Значения скоростей.
     * @param offset Смещение.
     * @return true в случае успеха, иначе false.
     */
    bool setVelocities(const QVector<Point3f>& data, size_t offset = 0);

    /**
     * @brief Получение значений масс объектов.
     * @param data Значения масс.
     * @param offset Смещение.
     * @param count Количество.
     * @return true в случае успеха, иначе false.
     */
    bool getMasses(QVector<qreal>& data, size_t offset = 0, size_t count = 0) const;

    /**
     * @brief Получение значений масс объектов.
     * @param data Значения масс.
     * @param offset Смещение.
     * @param count Количество.
     * @return true в случае успеха, иначе false.
     */
    bool getMasses(QVector<float>& data, size_t offset = 0, size_t count = 0) const;

    /**
     * @brief Получение значений позиций объектов.
     * @param data Значения позиций.
     * @param offset Смещение.
     * @param count Количество.
     * @return true в случае успеха, иначе false.
     */
    bool getPositions(QVector<QVector3D>& data, size_t offset = 0, size_t count = 0) const;

    /**
     * @brief Получение значений позиций объектов.
     * @param data Значения позиций.
     * @param offset Смещение.
     * @param count Количество.
     * @return true в случае успеха, иначе false.
     */
    bool getPositions(QVector<Point3f>& data, size_t offset = 0, size_t count = 0) const;

    /**
     * @brief Получение значений скоростей объектов.
     * @param data Значения скоростей.
     * @param offset Смещение.
     * @param count Количество.
     * @return true в случае успеха, иначе false.
     */
    bool getVelocities(QVector<QVector3D>& data, size_t offset = 0, size_t count = 0) const;

    /**
     * @brief Получение значений скоростей объектов.
     * @param data Значения скоростей.
     * @param offset Смещение.
     * @param count Количество.
     * @return true в случае успеха, иначе false.
     */
    bool getVelocities(QVector<Point3f>& data, size_t offset = 0, size_t count = 0) const;

    /**
     * @brief Получение индексного буфера.
     * @return Индексный буфер.
     */
    NBodyGLBuffer* indexBuffer();

    /**
     * @brief Получение буфера масс.
     * @return Буфер масс.
     */
    NBodyGLBuffer* massBuffer();

    /**
     * @brief Получение буфера позиций.
     * @return Буфер позиций.
     */
    NBodyGLBuffer* posBuffer();

    /**
     * @brief Получение буфера скоростей.
     * @return Буфер скоростей.
     */
    NBodyGLBuffer* velBuffer();

signals:
    /**
     * @brief Сигнал окончания симуляции.
     */
    void simulationFinished();

public slots:

    /**
     * @brief Запускает расчёт симуляции очередного шага.
     * Использует установленное время шага.
     * @return true в случае успеха, иначе false.
     */
    bool simulate();

    /**
     * @brief Запускает расчёт симуляции очередного шага.
     * @param dt Время шага.
     * @return true в случае успеха, иначе false.
     */
    bool simulate(float dt);

private:
    /**
     * @brief Число объектов.
     */
    size_t bodies_count;

    /**
     * @brief Число моделируемых объектов.
     */
    size_t simulated_bodies_count;

    /**
     * @brief Шаг симуляции.
     */
    float time_step;

    /**
     * @brief Флаг готовности.
     */
    bool is_ready;

    /**
     * @brief Текущие буферы для чтения.
     */
    size_t current_in;

    /**
     * @brief Текущие буферы для записи.
     */
    size_t current_out;

    /**
     * @brief Число буферов для переключения чтения/записи.
     */
    static const size_t switch_buffers_count = 2;

    /**
     * @brief Индексный буфер OpenGL.
     */
    NBodyGLBuffer* gl_index_buf;

    /**
     * @brief Буфер масс OpenGL.
     */
    NBodyGLBuffer* gl_mass_buf;

    /**
     * @brief Буферы позиций OpenGL.
     */
    NBodyGLBuffer* gl_pos_buf[switch_buffers_count];

    /**
     * @brief Буферы скоростей OpenGL.
     */
    NBodyGLBuffer* gl_vel_buf[switch_buffers_count];

    /**
     * @brief Контекст OpenCL.
     */
    CLContext* clcxt;

    /**
     * @brief Очередь команд OpenCL.
     */
    CLCommandQueue* clqueue;

    /**
     * @brief Программа OpenCL.
     */
    CLProgram* clprogram;

    /**
     * @brief Ядро OpenCL.
     */
    CLKernel* clkernel;

    /**
     * @brief Событие OpenCL.
     */
    CLEvent* clevent;

    /**
     * @brief Буфер масс OpenCL.
     */
    CLBuffer* cl_mass_buf;

    /**
     * @brief Буферы позиций OpenCL.
     */
    CLBuffer* cl_pos_buf[switch_buffers_count];

    /**
     * @brief Буферы скоростей OpenCL.
     */
    CLBuffer* cl_vel_buf[switch_buffers_count];

    /**
     * @brief Глобальный размер измерений.
     */
    size_t global_dims[NDRANGE_DIMENSIONS];

    /**
     * @brief Локальный размер измерений.
     */
    size_t local_dims[NDRANGE_DIMENSIONS];

    /**
     * @brief Инициализирует OpenCL.
     * @param platform Платформа OpenCL.
     * @param device Устройство OpenCL.
     * @return true в случае успеха, иначе false.
     */
    bool initOpenCL(const CLPlatform& platform, const CLDevice& device);

    /**
     * @brief Завершает OpenCL.
     * @return true в случае успеха, иначе false.
     */
    bool termOpenCL();

    /**
     * @brief Вычисляет размеры измерений X,Y.
     * @return true в случае успеха, иначе false.
     */
    bool calculateDimsSizes();

    /**
     * @brief Уничтожает объект OpenCL.
     * @param clobj Объект OpenCL.
     * @return true в случае успеха, иначе false.
     */
    template <typename T>
    bool destroyCLObject(T* clobj);

    /**
     * @brief Создаёт, считывает и компилирует программу OpenCL.
     * @return true в случае успеха, иначе false.
     */
    bool createCLProgram();

    /**
     * @brief Переключение буферов для чтения/записи.
     */
    void switchCurrentBuffers();

    /**
     * @brief Создаёт буферы OpenGL.
     * @return true в случае успеха, иначе false.
     */
    bool createGLBuffers();

    /**
     * @brief Уничтожает буферы OpenGL.
     * @return true в случае успеха, иначе false.
     */
    bool destroyGLBuffers();

    /**
     * @brief Создаёт буфер OpenGL.
     * @param buf Буфер OpenGL.
     * @param usage Тип использования буфера.
     * @param item_size_bytes Размер элемента буфера в байтах.
     * @return true в случае успеха, иначе false.
     */
    bool createGLBuffer(NBodyGLBuffer* buf, NBodyGLBuffer::UsagePattern usage, size_t item_size_bytes, const void* data = nullptr);

    /**
     * @brief Уничтожает буфер OpenGL.
     * @param buf Буфер OpenGL.
     * @return true в случае успеха, иначе false.
     */
    bool destroyGLBuffer(NBodyGLBuffer* buf);

    /**
     * @brief Запись в буфер данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @return true в случае успеха, иначе false.
     */
    bool setGLBufferData(NBodyGLBuffer* buf, const QVector<QVector3D>& data, size_t offset = 0);

    /**
     * @brief Запись в буфер данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @return true в случае успеха, иначе false.
     */
    bool setGLBufferData(NBodyGLBuffer* buf, const QVector<Point3f>& data, size_t offset = 0);

    /**
     * @brief Запись в буфер данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @return true в случае успеха, иначе false.
     */
    bool setGLBufferData(NBodyGLBuffer* buf, const QVector<qreal>& data, size_t offset = 0);

    /**
     * @brief Запись в буфер данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @return true в случае успеха, иначе false.
     */
    bool setGLBufferData(NBodyGLBuffer* buf, const QVector<float>& data, size_t offset = 0);

    /**
     * @brief Чтение из буфера данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @param count Количество.
     * @return true в случае успеха, иначе false.
     */
    bool getGLBufferData(NBodyGLBuffer* buf, QVector<QVector3D>& data, size_t offset = 0, size_t count = 0) const;

    /**
     * @brief Чтение из буфера данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @param count Количество.
     * @return true в случае успеха, иначе false.
     */
    bool getGLBufferData(NBodyGLBuffer* buf, QVector<Point3f>& data, size_t offset = 0, size_t count = 0) const;

    /**
     * @brief Чтение из буфера данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @param count Количество.
     * @return true в случае успеха, иначе false.
     */
    bool getGLBufferData(NBodyGLBuffer* buf, QVector<qreal>& data, size_t offset = 0, size_t count = 0) const;

    /**
     * @brief Чтение из буфера данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @param count Количество.
     * @return true в случае успеха, иначе false.
     */
    bool getGLBufferData(NBodyGLBuffer* buf, QVector<float>& data, size_t offset = 0, size_t count = 0) const;

    /**
     * @brief Создаёт буферы OpenCL.
     * @return true в случае успеха, иначе false.
     */
    bool createCLBuffers();

    /**
     * @brief Уничтожает буферы OpenCL.
     * @return true в случае успеха, иначе false.
     */
    bool destroyCLBuffers();

    /**
     * @brief Создаёт буфер OpenCL.
     * @param clbuf Буфер OpenCL.
     * @param flags Флаги буфера OpenCL.
     * @param glbuf Буфер OpenGL.
     * @return true в случае успеха, иначе false.
     */
    bool createCLBuffer(CLBuffer* clbuf, cl_mem_flags flags, NBodyGLBuffer* glbuf);

    /**
     * @brief Уничтожает буфер OpenCL.
     * @param buf Буфер OpenCL.
     * @return true в случае успеха, иначе false.
     */
    bool destroyCLBuffer(CLBuffer* buf);
};

#endif // CLGLNBODY_H
