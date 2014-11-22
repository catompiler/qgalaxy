#ifndef CLGLNBODY_H
#define CLGLNBODY_H

#include <QObject>
#include <QGLBuffer>


class CLContext;


class NBody : public QObject
{
    Q_OBJECT
public:
    explicit NBody(QObject *parent = 0);
    ~NBody();

    size_t bodiesCount() const;

    CLContext* clcontext();

    bool create(size_t bodies_count_);
    bool destroy();

    bool setMasses(const float* src_, size_t count_, size_t offset_);
    bool setPositions(const float* src_, size_t count_, size_t offset_);
    bool setVelocities(const float* src_, size_t count_, size_t offset_);

    QGLBuffer* massBuffer();
    QGLBuffer* posBuffer();
    QGLBuffer* velBuffer();
    QGLBuffer* indexBuffer();

signals:
    void simulationFinished();
    
public slots:

private:
    size_t _bodies_count;

    QGLBuffer* _mass_buf;
    QGLBuffer* _pos_buf;
    QGLBuffer* _vel_buf;
    QGLBuffer* _index_buf;

    CLContext* _clcxt;

    bool _initGLBuffer(QGLBuffer* buf_,
                       QGLBuffer::UsagePattern usage_,
                       size_t item_size_);
    bool _initGLBuffers();
    bool _destroyGLBuffer(QGLBuffer* buf_);
    bool _destroyGLBuffers();
};

#endif // CLGLNBODY_H
