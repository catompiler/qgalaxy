#ifndef CLCONTEXT_H
#define CLCONTEXT_H

#include <CL/opencl.h>
#include "clplatform.h"
#include "cldevice.h"
#include "clcommandqueue.h"
#include "clevent.h"
#include "clbuffer.h"

/**
 * @class CLContext.
 * @brief Класс контекста OpenCL.
 */
class CLContext
{
public:
    /**
     * @brief Конструктор.
     */
    CLContext();

    /**
     * @brief Конструктор.
     * @param context_id Идентификатор объекта контекста OpenCL.
     */
    explicit CLContext(const cl_context& context_id);

    /**
     * @brief Конструктор копирования.
     * Создаёт копию объекта, но не контекста.
     * @param context Копируемый объект.
     */
    CLContext(const CLContext& context);

    /**
     * @brief Деструктор.
     */
    ~CLContext();

    /**
     * @brief Получение идентификатор объекта контекста OpenCL.
     * @return Идентификатор объекта контекста OpenCL.
     */
    cl_context id() const;

    /**
     * @brief Установка идентификатора объекта контекста OpenCL.
     * @param cxt_id Идентификатор объекта контекста OpenCL.
     */
    void setId(const cl_context& cxt_id);

    /**
     * @brief Получение флага валидности контекста OpenCL.
     * @return Флаг валидности контекста OpenCL.
     */
    bool isValid() const;

    /**
     * @brief Создание контекста OpenCL.
     * @param platform Платформа OpenCL.
     * @param devices Список устройств OpenCL.
     * @param shared_with_gl Флаг общих данных с OpenGL.
     * @param err_code Код ошибки OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool create(const CLPlatform& platform, const CLDeviceList& devices,
                bool shared_with_gl = false, cl_int *err_code = nullptr);

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
     * @brief Получение числа устройств контекста OpenCL.
     * @return Число устройств контекста OpenCL.
     * @throw CLException в случае ошибки.
     */
    size_t devicesCount() const;

    /**
     * @brief Получение списка устройств контекста OpenCL.
     * @return Список устройств контекста OpenCL.
     * @throw CLException в случае ошибки.
     */
    CLDeviceList devices() const;

    /**
     * @brief Оператор присваивания.
     * @param cxt Контекст OpenCL.
     * @return Ссылка на себя.
     */
    CLContext& operator=(const CLContext& cxt);

    /**
     * @brief Оператор сравнения.
     * @param cxt Контекст OpenCL.
     * @return true, если идентификаторы контекстов равны, иначе false.
     */
    bool operator==(const CLContext& cxt) const;

    /**
     * @brief Создание события OpenCL.
     * @param err_code Код ошибки.
     * @return Событие OpenCL.
     * @throw CLException в случае ошибки.
     */
    CLEvent createEvent(cl_int* err_code = nullptr) const;

    /**
     * @brief Создание очереди команд OpenCL.
     * @param device Устройство OpenCL.
     * @param err_code Код ошибки.
     * @return Очередь комманд OpenCL.
     * @throw CLException в случае ошибки.
     */
    CLCommandQueue createCommandQueue(const CLDevice& device, cl_int *err_code = nullptr) const;

    /**
     * @brief Создание буфера OpenCL.
     * @param flags Флаги буфера OpenCL.
     * @param size Размер буфера.
     * @param host_ptr Указатель с данными.
     * @param err_code Код ошибки OpenCL.
     * @return Буфер OpenCL.
     * @throw CLException в случае ошибки.
     */
    CLBuffer createBuffer(cl_mem_flags flags, size_t size, void* host_ptr, cl_int* err_code = nullptr) const;

    /**
     * @brief Создание буфера OpenCL из буфера OpenGL.
     * @param flags Флаги буфера OpenCL.
     * @param glbuf_id Идентификатор буфера OpenGL.
     * @param err_code Код ошибки OpenCL.
     * @return Буфер OpenCL.
     * @throw CLException в случае ошибки.
     */
    CLBuffer createBufferFromGLBuffer(cl_mem_flags flags, GLuint glbuf_id, cl_int* err_code = nullptr) const;

private:

    /**
     * @brief Идентификатор контекста OpenCL.
     */
    cl_context m_id;

    /**
     * @brief Получает информацию о контексте OpenCL.
     * @param info Вид информации.
     * @return Информация о контексте OpenCL.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    T getInfoValue(cl_device_info info) const;

    /**
     * @brief Получает информацию о контексте OpenCL в виде вектора.
     * @param info Вид информации.
     * @return Информация о контексте OpenCL в виде вектора.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    QVector<T> getInfoValuev(cl_device_info info) const;
};

#endif // CLCONTEXT_H
