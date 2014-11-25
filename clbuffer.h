#ifndef CLBUFFER_H
#define CLBUFFER_H

#include <CL/opencl.h>
#include <QtOpenGL>
#include "clevent.h"

class CLContext;
class CLCommandQueue;

/**
 * @class CLBuffer.
 * @brief Класс буфера OpenCL.
 */
class CLBuffer
{
public:
    /**
     * @brief Конструктор.
     */
    CLBuffer();

    /**
     * @brief Конструктор.
     * @param buf_id Идентификатор объекта буфера OpenCL.
     */
    explicit CLBuffer(const cl_mem& buf_id);

    /**
     * @brief Конструктор копирования.
     * Создаёт копию объекта, но не памяти.
     * @param buffer Копируемый объект.
     */
    CLBuffer(const CLBuffer& buffer);

    /**
     * @brief Деструктор.
     */
    ~CLBuffer();

    /**
     * @brief Получение идентификатор объекта буфера OpenCL.
     * @return Идентификатор объекта буфера OpenCL.
     */
    cl_mem id() const;

    /**
     * @brief Установка идентификатора объекта буфера OpenCL.
     * @param buf_id Идентификатор объекта буфера OpenCL.
     */
    void setId(const cl_mem& buf_id);

    /**
     * @brief Получение флага валидности буфера OpenCL.
     * @return Флаг валидности буфера OpenCL.
     */
    bool isValid() const;

    /**
     * @brief Создание буфера OpenCL.
     * @param cxt Контекст OpenCL.
     * @param flags Флаги буфера OpenCL.
     * @param size Размер буфера.
     * @param host_ptr Указатель с данными.
     * @param err_code Код ошибки OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool create(const CLContext& cxt, cl_mem_flags flags, size_t size,
                void* host_ptr, cl_int* err_code = nullptr);

    /**
     * @brief Создание буфера OpenCL из буфера OpenGL.
     * @param cxt Контекст OpenCL.
     * @param flags Флаги буфера OpenCL.
     * @param glbuf_id Идентификатор буфера OpenGL.
     * @param err_code Код ошибки OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool createFromGLBuffer(const CLContext& cxt, cl_mem_flags flags, GLuint glbuf_id, cl_int* err_code = nullptr);

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
     * @brief Захватывает объект OpenGL, на основе которого создан буфер OpenCL.
     * @param queue Очередь комманд OpenCL.
     * @param wait_events Список событий для ожидания.
     * @param event Отслеживающее событие.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool enqueueAcquireGLObject(const CLCommandQueue& queue, const CLEventList* wait_events = nullptr, CLEvent* event = nullptr);

    /**
     * @brief Освобождает объект OpenGL, на основе которого создан буфер OpenCL.
     * @param queue Очередь комманд OpenCL.
     * @param wait_events Список событий для ожидания.
     * @param event Отслеживающее событие.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool enqueueReleaseGLObject(const CLCommandQueue& queue, const CLEventList* wait_events = nullptr, CLEvent* event = nullptr);

    /**
     * @brief Получение флагов буфера OpenCL.
     * @return Флаги буфера OpenCL.
     * @throw CLException в случае ошибки.
     */
    cl_mem_flags flags() const;

    /**
     * @brief Получение размера буфера OpenCL.
     * @return Размер буфера OpenCL.
     * @throw CLException в случае ошибки.
     */
    size_t size() const;

    /**
     * @brief Получение адреса отображённого буфера OpenCL.
     * @return Адрес отображённого буфера OpenCL.
     * @throw CLException в случае ошибки.
     */
    void* hostPtr() const;

    /**
     * @brief Получение идентификатора контекста OpenCL.
     * @return Идентификатор контекста OpenCL.
     * @throw CLException в случае ошибки.
     */
    cl_context contextId() const;

    /**
     * @brief Помещение в очередь команды чтения данных из буфера OpenCL.
     * @param queue Очередь комманд OpenCL.
     * @param blocking Флаг блокировки выполнения до конца чтения буфера.
     * @param offset Смещение.
     * @param cb Размер.
     * @param ptr Указатель на память для помещения прочитанных данных.
     * @param wait_events Список событий для ожидания.
     * @param event Отслеживающее событие.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool enqueueRead(const CLCommandQueue& queue, bool blocking,
                     size_t offset, size_t cb, void* ptr,
                     const CLEventList* wait_events = nullptr, CLEvent* event = nullptr);

    /**
     * @brief Помещение в очередь команды записи данных в буфер OpenCL.
     * @param queue Очередь комманд OpenCL.
     * @param blocking Флаг блокировки выполнения до конца записи буфера.
     * @param offset Смещение.
     * @param cb Размер.
     * @param ptr Указатель на память с записываемыми данными.
     * @param wait_events Список событий для ожидания.
     * @param event Отслеживающее событие.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool enqueueWrite(const CLCommandQueue& queue, bool blocking,
                      size_t offset, size_t cb, void* ptr,
                      const CLEventList* wait_events = nullptr, CLEvent* event = nullptr);

    /**
     * @brief Помещение в очередь команды отображения в память буфера OpenCL.
     * @param queue Очередь комманд OpenCL.
     * @param blocking Флаг блокировки выполнения до конца операции отображения в память буфера.
     * @param flags Флаги.
     * @param offset Смещение.
     * @param cb Размер.
     * @param wait_events Список событий для ожидания.
     * @param event Отслеживающее событие.
     * @param err_code Код ошибки.
     * @return Указатель на отображённый буфер.
     * @throw CLException в случае ошибки.
     */
    void *enqueueMap(const CLCommandQueue& queue, bool blocking,
                     cl_map_flags flags, size_t offset, size_t cb,
                     const CLEventList* wait_events = nullptr, CLEvent* event = nullptr,
                     cl_int* err_code = nullptr);

    /**
     * @brief Помещение в очередь команды прекращения отображения в память буфера OpenCL.
     * @param queue Очередь комманд OpenCL.
     * @param ptr Указатель на отображённый буфер.
     * @param wait_events Список событий для ожидания.
     * @param event Отслеживающее событие.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool enqueueUnMap(const CLCommandQueue& queue, void* ptr,
                     const CLEventList* wait_events = nullptr, CLEvent* event = nullptr);

    /**
     * @brief Оператор присваивания.
     * @param buffer Буфер OpenCL.
     * @return Ссылка на себя.
     */
    CLBuffer& operator=(const CLBuffer& buffer);

    /**
     * @brief Оператор сравнения.
     * @param buffer Буфер OpenCL.
     * @return true, если идентификаторы буферов равны, иначе false.
     */
    bool operator==(const CLBuffer& buffer) const;

private:

    /**
     * @brief Идентификатор буфера OpenCL.
     */
    cl_mem m_id;

    /**
     * @brief Получает информацию о буфере OpenCL.
     * @param info Вид информации.
     * @return Информация о буфере OpenCL.
     */
    template<class T>
    T getInfoValue(cl_mem_info info) const;
};

#endif // CLBUFFER_H
