#ifndef CLEVENT_H
#define CLEVENT_H

#include <QObject>
#include <QList>
#include <CL/opencl.h>
#include "clexception.h"
#include "utils.h"
#include "clcommandqueue.h"


class CLContext;

/**
 * @class CLEvent.
 * @brief Класс события OpenCL.
 */
class CLEvent : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор.
     * @param parent Объект-родитель.
     */
    explicit CLEvent(QObject *parent = 0);

    /**
     * @brief Конструктор.
     * @param event_id Идентификатор объекта события OpenCL.
     * @param parent Объект-родитель.
     */
    explicit CLEvent(const cl_event& event_id, QObject* parent = 0);

    /**
     * @brief Конструктор копирования.
     * Создаёт копию объекта, но не события.
     * @param event Копируемый объект.
     */
    CLEvent(const CLEvent &event);

    /**
     * @brief Деструктор.
     */
    ~CLEvent();

    /**
     * @brief Получение идентификатор объекта события OpenCL.
     * @return Идентификатор объекта события OpenCL.
     */
    cl_event id() const;

    /**
     * @brief Установка идентификатора объекта события OpenCL.
     * @param event_id Идентификатор объекта события OpenCL.
     */
    void setId(const cl_event& event_id);

    /**
     * @brief Получение флага валидности события OpenCL.
     * @return Флаг валидности события OpenCL.
     */
    bool isValid() const;

    /**
     * @brief Создание события OpenCL.
     * @param cxt Контекст OpenCL.
     * @param err_code Код ошибки.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool create(const CLContext& cxt, cl_int* err_code = nullptr);

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
     * @brief Получение статус события OpenCL.
     * @return Статус события OpenCL.
     * @throw CLException в случае ошибки.
     */
    cl_int status() const;

    /**
     * @brief Установка статуса события OpenCL.
     * @param Статус события OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool setStatus(cl_int status);

    /**
     * @brief Получение флага успешного завершения задания события.
     * @return Флаг успешного завершения задания события.
     * @throw CLException в случае ошибки.
     */
    bool isCompleted() const;

    /**
     * @brief Получение флага выполнения задания события.
     * @return Флаг выполнения задания события.
     * @throw CLException в случае ошибки.
     */
    bool isRunning() const;

    /**
     * @brief Ожидание события OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool wait() const;

    /**
     * @brief Ожидание списка событий OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    static bool waitForEvents(const QList<CLEvent>& events);

    /**
     * @brief Получение идентификатора очереди команд OpenCL.
     * @return Идентификатор очереди команд OpenCL.
     * @throw CLException в случае ошибки.
     */
    cl_command_queue commandQueueId() const;

    /**
     * @brief Получение идентификатора контекста OpenCL.
     * @return Идентификатор контекста OpenCL.
     * @throw CLException в случае ошибки.
     */
    cl_context contextId() const;

    /**
     * @brief Получение типа команды события.
     * @return Тип команды события.
     * @throw CLException в случае ошибки.
     */
    cl_command_type commandType() const;

    /**
     * @brief Оператор присваивания.
     * @param event Событие OpenCL.
     * @return Ссылка на себя.
     */
    CLEvent& operator=(const CLEvent& event);

    /**
     * @brief Оператор сравнения.
     * @param event Событие OpenCL.
     * @return true, если идентификаторы событий равны, иначе false.
     */
    bool operator==(const CLEvent& event) const;
    
signals:
    /**
     * @brief Сигнал события OpenCL.
     * @param exec_status Статус выполнения операции.
     */
    void completed(int exec_status);
private:

    /**
     * @brief Идентификатор события OpenCL.
     */
    cl_event m_id;

    /**
     * @brief Устанавливает идентификатор события OpenCL и функцию обратного вызова.
     * @param ev_id Идентификатор события OpenCL.
     * @throw CLException в случае ошибки.
     */
    inline void m_setId(cl_event ev_id);

    /**
     * @brief Получает информацию о событии OpenCL.
     * @param info Вид информации.
     * @return Информация о событии OpenCL.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    T getInfoValue(cl_event_info info) const;

    /**
     * @brief Функция обратного вызова для событий OpenCL.
     * @param event Событие OpenCL.
     * @param exec_status Статус выполнения операции.
     * @param user_data Пользовательские данные события.
     */
    static void CL_CALLBACK event_notify(cl_event event, cl_int exec_status, void* user_data);
};


typedef QList<CLEvent> CLEventList;

#endif // CLEVENT_H
