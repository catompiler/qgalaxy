#ifndef CLPROGRAM_H
#define CLPROGRAM_H

#include <CL/opencl.h>
#include <QVector>
#include <QString>
#include "cldevice.h"

class CLContext;

/**
 * @class CLProgram.
 * @brief Ксласс программы OpenCL.
 */
class CLProgram
{
public:
    /**
     * @brief Конструктор.
     */
    CLProgram();

    /**
     * @brief Конструктор.
     * @param prog_id Идентификатор объекта программы OpenCL.
     */
    explicit CLProgram(const cl_program& prog_id);

    /**
     * @brief Конструктор копирования.
     * Создаёт копию объекта, но не программы.
     * @param prog Копируемый объект.
     */
    CLProgram(const CLProgram& prog);

    /**
     * @brief Деструктор.
     */
    ~CLProgram();

    /**
     * @brief Получение идентификатор объекта программы OpenCL.
     * @return Идентификатор объекта программы OpenCL.
     */
    cl_program id() const;

    /**
     * @brief Установка идентификатора объекта программы OpenCL.
     * @param prog_id Идентификатор объекта программы OpenCL.
     */
    void setId(const cl_program& prog_id);

    /**
     * @brief Получение флага валидности программы OpenCL.
     * @return Флаг валидности программы OpenCL.
     */
    bool isValid() const;

    /**
     * @brief Создание программы OpenCL.
     * @param clcxt Контекст OpenCL.
     * @param device Устройство OpenCL.
     * @param err_code Код ошибки.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool create(const CLContext& clcxt, const QString& source, cl_int *err_code = nullptr);

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
     * @brief Производит сборку программы OpenCL.
     * @param devices Список устройств OpenCL.
     * @param options Опции компиляции.
     * @param err_code Код ошибки OpenCL.
     * @return true в случае успеха, иначе false.
     * @throw CLException в случае ошибки.
     */
    bool build(const CLDeviceList& devices, const QStringList& options, cl_int *err_code = nullptr);

    /**
     * @brief Получение статуса сборки программы OpenCL.
     * @param device Устройство OpenCL.
     * @return Статус сборки программы OpenCL.
     * @throw CLException в случае ошибки.
     */
    cl_build_status buildStatus(const CLDevice& device) const;

    /**
     * @brief Получение лога сборки программы OpenCL.
     * @param device Устройство OpenCL.
     * @return Лог сборки программы OpenCL.
     * @throw CLException в случае ошибки.
     */
    QString buildLog(const CLDevice& device) const;

    /**
     * @brief Получение идентификатора контекста OpenCL.
     * @return Идентификатор контекста OpenCL.
     * @throw CLException в случае ошибки.
     */
    cl_context contextId() const;

    /**
     * @brief Получение числа устройств программы OpenCL.
     * @return Число устройств программы OpenCL.
     * @throw CLException в случае ошибки.
     */
    size_t devicesCount() const;

    /**
     * @brief Получение списка устройств программы OpenCL.
     * @return Список устройств программы OpenCL.
     * @throw CLException в случае ошибки.
     */
    CLDeviceList devices() const;

    /**
     * @brief Оператор присваивания.
     * @param prog Программа OpenCL.
     * @return Ссылка на себя.
     */
    CLProgram& operator=(const CLProgram& prog);

    /**
     * @brief Оператор сравнения.
     * @param prog Программа OpenCL.
     * @return true, если Идентификаторы буферов равны, иначе false.
     */
    bool operator==(const CLProgram& prog) const;

private:

    /**
     * @brief Идентификатор программы OpenCL.
     */
    cl_program m_id;

    /**
     * @brief Получает текстовую информацию о программе OpenCL.
     * @param info Вид информации.
     * @return Текстовая информация о программе OpenCL.
     * @throw CLException в случае ошибки.
     */
    QString getInfoValueStr(cl_program_info info) const;

    /**
     * @brief Получает информацию о программе OpenCL.
     * @param info Вид информации.
     * @return Информация о программе OpenCL.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    T getInfoValue(cl_program_info info) const;

    /**
     * @brief Получает информацию о программе OpenCL в виде вектора.
     * @param info Вид информации.
     * @return Информация о программе OpenCL в виде вектора.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    QVector<T> getInfoValuev(cl_program_info info) const;

    /**
     * @brief Получает текстовую информацию о сборке программы OpenCL.
     * @param info Вид информации.
     * @return Текстовая информация о сборке программы OpenCL.
     * @throw CLException в случае ошибки.
     */
    QString getBuildInfoValueStr(const CLDevice& device, cl_program_build_info info) const;

    /**
     * @brief Получает информацию о сборке программы OpenCL.
     * @param info Вид информации.
     * @return Информация о сборке программы OpenCL.
     * @throw CLException в случае ошибки.
     */
    template<class T>
    T getBuildInfoValue(const CLDevice& device, cl_program_build_info info) const;
};

#endif // CLPROGRAM_H
