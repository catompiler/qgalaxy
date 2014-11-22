#ifndef CLCOMMANDQUEUE_H
#define CLCOMMANDQUEUE_H

#include <CL/opencl.h>

class CLContext;
class CLDevice;

/**
 * @class CLCommandQueue.
 * @brief Класс очереди команд OpenCL.
 */
class CLCommandQueue
{
public:
    /**
     * @brief Конструктор.
     */
    CLCommandQueue();

    /**
     * @brief Конструктор.
     * @param cq_id Идентификатор объекта очереди команд OpenCL.
     */
    explicit CLCommandQueue(const cl_command_queue& cq_id);

    /**
     * @brief Конструктор копирования.
     * Создаёт копию объекта, но не очереди команд.
     * @param cq Копируемый объект.
     */
    CLCommandQueue(const CLCommandQueue& cq);

    /**
     * @brief Деструктор.
     */
    ~CLCommandQueue();

    /**
     * @brief Получение идентификатор объекта очереди команд OpenCL.
     * @return Идентификатор объекта очереди OpenCL.
     */
    cl_command_queue id() const;

    /**
     * @brief Установка идентификатора объекта очереди команд OpenCL.
     * @param cq_id Идентификатор объекта очереди команд OpenCL.
     */
    void setId(const cl_command_queue& cq_id);

    /**
     * @brief Получение флага валидности очереди команд OpenCL.
     * @return Флаг валидности очереди команд OpenCL.
     */
    bool isValid() const;

    /**
     * @brief Создание очереди команд OpenCL.
     * @param clcxt Контекст OpenCL.
     * @param device Устройство OpenCL.
     * @param err_code Код ошибки.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool create(const CLContext& clcxt, const CLDevice& device, cl_int *err_code = nullptr);

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
     * @brief Отправляет команды в очереди OpenCL на устройство OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool flush();

    /**
     * @brief Ждёт завершения выполнения команд в очереди OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool finish();

    /**
     * @brief Получение идентификатора контекста OpenCL.
     * @return Идентификатор контекста OpenCL.
     * @throw CLException в случае ошибки.
     */
    cl_context contextId() const;

    /**
     * @brief Получение идентификатор устройства OpenCL.
     * @return Идентификатор устройства OpenCL.
     */
    cl_device_id deviceId() const;

    /**
     * @brief Оператор присваивания.
     * @param cq Очередь команд OpenCL.
     * @return Ссылка на себя.
     */
    CLCommandQueue& operator=(const CLCommandQueue& cq);

    /**
     * @brief Оператор сравнения.
     * @param cq Очередь команд OpenCL.
     * @return true, если Идентификаторы очереди команд равны, иначе false.
     */
    bool operator==(const CLCommandQueue& cq) const;

private:

    /**
     * @brief Идентификатор очереди команд OpenCL.
     */
    cl_command_queue m_id;

    /**
     * @brief Получает информацию об очереди команд OpenCL.
     * @param info Вид информации.
     * @return Информация об очереди команд OpenCL.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    T getInfoValue(cl_command_queue_info info) const;
};

#endif // CLCOMMANDQUEUE_H
