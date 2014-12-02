#ifndef NBODYWIDGET_H
#define NBODYWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QVector>
#include <QQuaternion>
#include "point3f.h"


class NBody;
class QMouseEvent;
class QWheelEvent;


class NBodyWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit NBodyWidget(QWidget *parent = 0);
    ~NBodyWidget();

    size_t bodiesCount() const;

    bool setBodies(size_t offset, const QVector<qreal>& masses, const QVector<QVector3D>& positions, const QVector<QVector3D>& velocities);
    bool setBodies(size_t offset, const QVector<float>& masses, const QVector<Point3f>& positions, const QVector<Point3f>& velocities);
    bool getBodies(size_t offset, size_t count, QVector<qreal> &masses, QVector<QVector3D> &positions, QVector<QVector3D> &velocities);
    bool getBodies(size_t offset, size_t count, QVector<float> &masses, QVector<Point3f> &positions, QVector<Point3f> &velocities);

signals:

public slots:
    void setSimulationRunning(bool running);
    bool recreateNBody();

private slots:
    void simulationFinished();

private:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    NBody* nbody;

    bool sim_run;

    bool has_point_sprite;
    GLuint sprite_texture;

    float view_distance;
    float rot_x;
    float rot_y;
    float old_rot_x;
    float old_rot_y;

    QQuaternion rot_quat;
};

#endif // NBODYWIDGET_H
