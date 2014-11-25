#ifndef CLKERNEL_H
#define CLKERNEL_H

#include <CL/opencl.h>
#include <QVector>
#include <QString>
#include "clevent.h"

class CLProgram;
class CLDevice;
class CLCommandQueue;

/**
 * @class CLKernel.
 * @brief Класс ядра OpenCL.
 */
class CLKernel
{
public:
    /**
     * @brief Конструктор.
     */
    CLKernel();

    /**
     * @brief Конструктор.
     * @param kernel_id Идентификатор объекта ядра OpenCL.
     */
    explicit CLKernel(const cl_kernel& kernel_id);

    /**
     * @brief Конструктор копирования.
     * Создаёт копию объекта, но не ядра.
     * @param kernel Копируемый объект.
     */
    CLKernel(const CLKernel& kernel);

    /**
     * @brief Деструктор.
     */
    ~CLKernel();

    /**
     * @brief Получение идентификатор объекта ядра OpenCL.
     * @return Идентификатор объекта ядра OpenCL.
     */
    cl_kernel id() const;

    /**
     * @brief Установка идентификатора объекта ядра OpenCL.
     * @param kernel_id Идентификатор объекта ядра OpenCL.
     */
    void setId(const cl_kernel& kernel_id);

    /**
     * @brief Получение флага валидности ядра OpenCL.
     * @return Флаг валидности ядра OpenCL.
     */
    bool isValid() const;

    /**
     * @brief Создание ядра OpenCL.
     * @param program Программа OpenCL.
     * @param kernel_func_name Имя функции ядра OpenCL.
     * @param err_code Код ошибки.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool create(const CLProgram& program, const QString& kernel_func_name, cl_int *err_code = nullptr);

    /**
     * @brief Увеличение числа ссылок на объект.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool retain();

    /**
     * @brief Уменьшение числа ссылок на объект.
     * После удаления последней ссылки, объект удаляется OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool release();

    /**
     * @brief Установка значения аргумента ядра OpenCL.
     * @param index Индекс аргумента ядра OpenCL.
     * @param arg_value Значение аргумента ядра OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    template <typename T>
    bool setArg(size_t index, T arg_value);

    /**
     * @brief Установка размера буфера, выделяемого для локального аргумента ядра OpenCL.
     * @param index Индекс аргумента ядра OpenCL.
     * @param size Размера буфера, выделяемого для локального аргумента ядра OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool setLocalArgSize(size_t index, size_t size);

    /**
     * @brief Выполняет ядро на устройстве OpenCL.
     * @param queue Очередь команд OpenCL.
     * @param work_dims Число измерений.
     * @param global_work_size Глобальные размеры по каждому измерению.
     * @param local_work_size Локальные размеры по каждому измерению.
     * @param wait_events Список событий для ожидания.
     * @param event Отслеживающее событие.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool execute(const CLCommandQueue& queue, size_t work_dims,
                 const size_t *global_work_size, const size_t *local_work_size,
                 const CLEventList* wait_events = nullptr, CLEvent* event = nullptr);

    /**
     * @brief Получение идентификатора контекста OpenCL.
     * @return Идентификатор контекста OpenCL.
     * @throw CLException в случае ошибки.
     */
    cl_context contextId() const;

    /**
     * @brief Получение идентификатора программы OpenCL.
     * @return Идентификатор программы OpenCL.
     * @throw CLException в случае ошибки.
     */
    cl_program programId() const;

    /**
     * @brief Получение имени функции ядра OpenCL.
     * @return Имя функции ядра OpenCL.
     * @throw CLException в случае ошибки.
     */
    QString functionName() const;

    /**
     * @brief Получение количества аргументов в ядре OpenCL.
     * @return Количество аргументов в ядре OpenCL.
     * @throw CLException в случае ошибки.
     */
    size_t argsCount() const;

    /**
     * @brief Получение размера рабочей группы ядра OpenCL.
     * @param device Устройство OpenCL.
     * @return Размер рабочей группы ядра OpenCL.
     * @throw CLException в случае ошибки.
     */
    size_t workGroupSize(const CLDevice& device) const;

    /**
     * @brief Получение компилированного размера рабочей группы ядра OpenCL для трёх измерений.
     * @param device Устройство OpenCL.
     * @return Компилированный размер рабочей группы ядра OpenCL для трёх измерений.
     * @throw CLException в случае ошибки.
     */
    QVector<size_t> compileWorkGroupSize(const CLDevice& device) const;

    /**
     * @brief Получение размера локальной памяти рабочей группы ядра OpenCL.
     * @param device Устройство OpenCL.
     * @return Размер локальной памяти рабочей группы ядра OpenCL.
     * @throw CLException в случае ошибки.
     */
    size_t localMemSize(const CLDevice& device) const;

    /**
     * @brief Оператор присваивания.
     * @param kernel Программа OpenCL.
     * @return Ссылка на себя.
     */
    CLKernel& operator=(const CLKernel& kernel);

    /**
     * @brief Оператор сравнения.
     * @param kernel Программа OpenCL.
     * @return true, если Идентификаторы ядер равны, иначе false.
     */
    bool operator==(const CLKernel& kernel) const;

private:

    /**
     * @brief Идентификатор ядра OpenCL.
     */
    cl_kernel m_id;

    /**
     * @brief Получает текстовую информацию о ядре OpenCL.
     * @param info Вид информации.
     * @return Текстовая информация о ядре OpenCL.
     * @throw CLException в случае ошибки.
     */
    QString getInfoValueStr(cl_kernel_info info) const;

    /**
     * @brief Получает информацию о ядре OpenCL.
     * @param info Вид информации.
     * @return Информация о ядре OpenCL.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    T getInfoValue(cl_kernel_info info) const;

    /**
     * @brief Получает информацию о рабочей группе ядра OpenCL.
     * @param info Вид информации.
     * @return Информация о рабочей группе ядра OpenCL.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    T getWorkGroupInfoValue(const CLDevice& device, cl_kernel_info info) const;

    /**
     * @brief Получает информацию о рабочей группе ядра OpenCL в виде вектора.
     * @param info Вид информации.
     * @return Информация о рабочей группе ядра OpenCL в виде вектора.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    QVector<T> getWorkGroupInfoValuev(const CLDevice& device, cl_kernel_info info) const;
};

template <typename T>
bool CLKernel::setArg(size_t index, T arg_value)
{
    CL_ERR_THROW(clSetKernelArg(m_id, index, sizeof(T), &arg_value));
    return true;
}

#endif // CLKERNEL_H
