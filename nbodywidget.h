#ifndef NBODYWIDGET_H
#define NBODYWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QVector>
#include <QVector3D>
#include <QQuaternion>
#include "point3f.h"


class NBody;
class QMouseEvent;
class QWheelEvent;
class QString;


/**
 * @brief Класс области просмотра,
 * осуществляющий визуализацию моделирования.
 * @class NBodyWidget.
 */
class NBodyWidget : public QGLWidget
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор.
     * @param Объект-родитель.
     */
    explicit NBodyWidget(QWidget *parent = 0);

    /**
     * Деструктор.
     */
    ~NBodyWidget();

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
     * @brief Сбрасывает параметры симуляции и тел.
     * @return  true в случае успеха, иначе false.
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
     * @brief Получение флага непрерывной симуляции.
     * @return Флаг непрерывной симуляции.
     */
    bool isSimulationRunning() const;

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
     * @brief Сохранение взаимодействующих тел в файл.
     * @param filename Имя файла.
     * @return true в случае успеха, иначе false.
     */
    bool saveNBody(const QString& filename);

    /**
     * @brief Загрузка взаимодействующих тел из файла.
     * @param filename Имя файла.
     * @return true в случае успеха, иначе false.
     */
    bool openNBody(const QString& filename);

    /**
     * @brief Установка параметров тел.
     * @param offset Смещение номера первого тела.
     * @param masses Массы тел.
     * @param positions Позиции тел.
     * @param velocities Векторы скоростей тел.
     * @return true в случае успеха, иначе false.
     */
    bool setBodies(size_t offset, const QVector<qreal>& masses, const QVector<QVector3D>& positions, const QVector<QVector3D>& velocities);

    /**
     * @brief Установка параметров тел.
     * @param offset Смещение номера первого тела.
     * @param masses Массы тел.
     * @param positions Позиции тел.
     * @param velocities Векторы скоростей тел.
     * @return true в случае успеха, иначе false.
     */
    bool setBodies(size_t offset, const QVector<float>& masses, const QVector<Point3f>& positions, const QVector<Point3f>& velocities);

    /**
     * @brief Получение параметров тел.
     * @param offset Смещение номера первого тела.
     * @param count Число тел.
     * @param masses Массы тел.
     * @param positions Позиции тел.
     * @param velocities Векторы скоростей тел.
     * @return true в случае успеха, иначе false.
     */
    bool getBodies(size_t offset, size_t count, QVector<qreal> &masses, QVector<QVector3D> &positions, QVector<QVector3D> &velocities);

    /**
     * @brief Получение параметров тел.
     * @param offset Смещение номера первого тела.
     * @param count Число тел.
     * @param masses Массы тел.
     * @param positions Позиции тел.
     * @param velocities Векторы скоростей тел.
     * @return true в случае успеха, иначе false.
     */
    bool getBodies(size_t offset, size_t count, QVector<float> &masses, QVector<Point3f> &positions, QVector<Point3f> &velocities);

signals:
    /**
     * @brief Сигнал окончания симуляции.
     */
    void simulationFinished();

    /**
     * @brief Сигнал окончания инициализации.
     */
    void nbodyInitialized();

public slots:

    /**
     * @brief Установка статуса симуляции.
     * @param running Флаг запущенности симуляции.
     */
    void setSimulationRunning(bool running);

    /**
     * @brief Запуск симуляции.
     */
    void startSimulation();

    /**
     * @brief Установка симуляции.
     */
    void stopSimulation();

    /**
     * @brief Пересоздание системы симуляции.
     * @return true в случае успеха, иначе false.
     */
    bool recreateNBody();

private slots:

    /**
     * @brief Слот завершения симуляции.
     */
    void on_simulationFinished();

private:

    /**
     * @brief Функция настройки OpenGL.
     */
    void initializeGL();

    /**
     * @brief Функция изменения парметров просмотра
     * при изменении размера области отображения.
     * @param width Ширина.
     * @param height Высота.
     */
    void resizeGL(int width, int height);

    /**
     * @brief Функция перерисовки.
     */
    void paintGL();

    /**
     * @brief Функция вычисления нового масштаба.
     * @param old_value Старый масштаб.
     * @param step Шаг.
     * @param scale Увеличение.
     * @return Новый масштаб.
     */
    qreal calcNewValueExp(qreal old_value, qreal step, qreal scale);

    /**
     * @brief Обработчик событий перемещения мыши.
     * @param event Событие.
     */
    void mouseMoveEvent(QMouseEvent* event);

    /**
     * @brief Обработчик события перемещения колёсика мыши.
     * @param event Событие.
     */
    void wheelEvent(QWheelEvent* event);

    /**
     * @brief Подпись формата файла.
     */
    static const quint32 data_file_magic = 0x474c5859;

    /**
     * @brief Версия формата файла.
     */
    static const quint32 data_file_version = 0x100;

    /**
     * @brief Система симуляции.
     */
    NBody* nbody;

    /**
     * @brief Флаг непрерывного выполнения симуляции.
     */
    bool sim_run;

    /**
     * @brief Флаг наличия расширения GL_ARB_point_sprite.
     */
    bool has_point_sprite;

    /**
     * @brief Идентификатор текстуры.
     */
    GLuint sprite_texture;

    /**
     * @brief Позиция просмотра по оси Z.
     */
    qreal view_position;

    /**
     * @brief Вращение.
     */
    QQuaternion view_rotation;

    /**
     * @brief Предыдущее положение мыши по оси X.
     */
    float old_event_x;

    /**
     * @brief Предыдущее положение мыши по оси Y.
     */
    float old_event_y;
};

#endif // NBODYWIDGET_H
