#ifndef CLGLNBODY_H
#define CLGLNBODY_H

#include <QObject>
#include <QVector>
#include <QVector3D>
#include <QGLBuffer>
#include <CL/opencl.h>
#include "point3f.h"


class CLPlatform;
class CLDevice;
class CLContext;
class CLBuffer;
class CLCommandQueue;
class CLProgram;
class CLKernel;
class CLEvent;

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
     * @brief Получение индексного буфера.
     * @return Индексный буфер.
     */
    QGLBuffer* indexBuffer();

    /**
     * @brief Получение буфера масс.
     * @return Буфер масс.
     */
    QGLBuffer* massBuffer();

    /**
     * @brief Получение буфера позиций.
     * @return Буфер позиций.
     */
    QGLBuffer* posBuffer();

    /**
     * @brief Получение буфера скоростей.
     * @return Буфер скоростей.
     */
    QGLBuffer* velBuffer();

signals:
    /**
     * @brief Сигнал окончания симуляции.
     */
    void simulationFinished();

public slots:

    bool simulate(float dt);

private:
    /**
     * @brief Число объектов.
     */
    size_t bodies_count;

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
    QGLBuffer* gl_index_buf;

    /**
     * @brief Буфер масс OpenGL.
     */
    QGLBuffer* gl_mass_buf;

    /**
     * @brief Буферы позиций OpenGL.
     */
    QGLBuffer* gl_pos_buf[switch_buffers_count];

    /**
     * @brief Буферы скоростей OpenGL.
     */
    QGLBuffer* gl_vel_buf[switch_buffers_count];

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
     * @brief Глобальный размер измерений X,Y.
     */
    size_t global_dims[2];

    /**
     * @brief Локальный размер измерений X,Y.
     */
    size_t local_dims[2];

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
    bool createGLBuffer(QGLBuffer* buf, QGLBuffer::UsagePattern usage, size_t item_size_bytes);

    /**
     * @brief Уничтожает буфер OpenGL.
     * @param buf Буфер OpenGL.
     * @return true в случае успеха, иначе false.
     */
    bool destroyGLBuffer(QGLBuffer* buf);

    /**
     * @brief Запись в буфер данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @return true в случае успеха, иначе false.
     */
    bool setGLBufferData(QGLBuffer* buf, const QVector<QVector3D>& data, size_t offset = 0);

    /**
     * @brief Запись в буфер данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @return true в случае успеха, иначе false.
     */
    bool setGLBufferData(QGLBuffer* buf, const QVector<Point3f>& data, size_t offset = 0);

    /**
     * @brief Запись в буфер данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @return true в случае успеха, иначе false.
     */
    bool setGLBufferData(QGLBuffer* buf, const QVector<qreal>& data, size_t offset = 0);

    /**
     * @brief Запись в буфер данных.
     * @param buf Буфер.
     * @param data Данные.
     * @param offset Смещение в буфере, в элементах вектора.
     * @return true в случае успеха, иначе false.
     */
    bool setGLBufferData(QGLBuffer* buf, const QVector<float>& data, size_t offset = 0);

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
    bool createCLBuffer(CLBuffer* clbuf, cl_mem_flags flags, QGLBuffer* glbuf);

    /**
     * @brief Уничтожает буфер OpenCL.
     * @param buf Буфер OpenCL.
     * @return true в случае успеха, иначе false.
     */
    bool destroyCLBuffer(CLBuffer* buf);
};

#endif // CLGLNBODY_H
