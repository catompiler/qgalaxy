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

class NBodyWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit NBodyWidget(QWidget *parent = 0);
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

    bool saveNBody(const QString& filename);
    bool openNBody(const QString& filename);

    bool setBodies(size_t offset, const QVector<qreal>& masses, const QVector<QVector3D>& positions, const QVector<QVector3D>& velocities);
    bool setBodies(size_t offset, const QVector<float>& masses, const QVector<Point3f>& positions, const QVector<Point3f>& velocities);
    bool getBodies(size_t offset, size_t count, QVector<qreal> &masses, QVector<QVector3D> &positions, QVector<QVector3D> &velocities);
    bool getBodies(size_t offset, size_t count, QVector<float> &masses, QVector<Point3f> &positions, QVector<Point3f> &velocities);

signals:
    void simulationFinished();
    void nbodyInitialized();

public slots:
    void setSimulationRunning(bool running);
    void startSimulation();
    void stopSimulation();
    bool recreateNBody();

private slots:
    void on_simulationFinished();

private:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    qreal calcNewValueExp(qreal old_value, qreal step, qreal scale);

    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    static const quint32 data_file_magic = 0x474c5859;
    static const quint32 data_file_version = 0x100;

    NBody* nbody;

    bool sim_run;

    bool has_point_sprite;
    GLuint sprite_texture;

    qreal view_position;
    QQuaternion view_rotation;

    float old_event_x;
    float old_event_y;
};

#endif // NBODYWIDGET_H
