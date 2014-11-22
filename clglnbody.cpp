#include "clglnbody.h"
#include <QGLBuffer>
#include "clcontext.h"



CLGLNBody::CLGLNBody(CLContext* clcxt_, QObject *parent) :
    QObject(parent)
{
    _clcxt = clcxt_;
    _bodies_count = 0;
    _mass_buf = new QGLBuffer(QGLBuffer::VertexBuffer);
    _pos_buf = new QGLBuffer(QGLBuffer::VertexBuffer);
    _vel_buf = new QGLBuffer(QGLBuffer::VertexBuffer);
    _index_buf = new QGLBuffer(QGLBuffer::IndexBuffer);
}

CLGLNBody::~CLGLNBody()
{
    if(_clcxt){
        _clcxt->release();
        delete _clcxt;
    }
    delete _mass_buf;
    delete _pos_buf;
    delete _vel_buf;
    delete _index_buf;
}

size_t CLGLNBody::bodiesCount() const
{
    return _bodies_count;
}

CLContext *CLGLNBody::clcontext()
{
    return _clcxt;
}

bool CLGLNBody::create(size_t bodies_count_)
{
    destroy();

    _bodies_count = bodies_count_;

    if(!_initGLBuffers()) return false;

    return true;
}

bool CLGLNBody::destroy()
{
    bool res;

    res = _destroyGLBuffers();

    return res;
}

bool CLGLNBody::setMasses(const float *src_, size_t count_, size_t offset_)
{
    if(!_mass_buf->isCreated()) return false;
    if(!_mass_buf->bind()) return false;
    _mass_buf->write(offset_ * sizeof(float), src_, count_ * sizeof(float));
    _mass_buf->release();
    return true;
}

bool CLGLNBody::setPositions(const float *src_, size_t count_, size_t offset_)
{
    if(!_pos_buf->isCreated()) return false;
    if(!_pos_buf->bind()) return false;
    _pos_buf->write(offset_ * sizeof(float) * 3, src_, count_ * sizeof(float) * 3);
    _pos_buf->release();
    return true;
}

bool CLGLNBody::setVelocities(const float *src_, size_t count_, size_t offset_)
{
    if(!_vel_buf->isCreated()) return false;
    if(!_vel_buf->bind()) return false;
    _vel_buf->write(offset_ * sizeof(float) * 3, src_, count_ * sizeof(float) * 3);
    _vel_buf->release();
    return true;
}

QGLBuffer *CLGLNBody::massBuffer()
{
    return _mass_buf;
}

QGLBuffer *CLGLNBody::posBuffer()
{
    return _pos_buf;
}

QGLBuffer *CLGLNBody::velBuffer()
{
    return _vel_buf;
}

QGLBuffer *CLGLNBody::indexBuffer()
{
    return _index_buf;
}

bool CLGLNBody::_initGLBuffer(QGLBuffer *buf_,
                              QGLBuffer::UsagePattern usage_,
                              size_t item_size_)
{
    buf_->setUsagePattern(usage_);
    if(!buf_->create()) return false;
    if(!buf_->bind()) return false;
    buf_->allocate(item_size_ * _bodies_count);
    return true;
}

bool CLGLNBody::_initGLBuffers()
{
    bool res;

    res = _initGLBuffer(_mass_buf, QGLBuffer::StaticDraw,sizeof(float)) &&
          _initGLBuffer(_pos_buf, QGLBuffer::StaticDraw, sizeof(float) * 3) &&
          _initGLBuffer(_vel_buf, QGLBuffer::StaticDraw, sizeof(float) * 3) &&
          _initGLBuffer(_index_buf, QGLBuffer::StaticDraw, sizeof(unsigned int));
    if(!res){
        _destroyGLBuffers();
        return false;
    }

    _index_buf->bind();
    unsigned int* ptr = static_cast<unsigned int*>(
                            _index_buf->map(QGLBuffer::WriteOnly)
                        );
    if(ptr == nullptr){
        _destroyGLBuffers();
        return false;
    }
    for(unsigned int i = 0; i < _bodies_count; i ++){
        ptr[i] = i;
    }
    _index_buf->unmap();
    _index_buf->release();

    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    return true;
}

bool CLGLNBody::_destroyGLBuffer(QGLBuffer *buf_)
{
    if(buf_ && buf_->isCreated()){
        buf_->destroy();
        return true;
    }
    return false;
}

bool CLGLNBody::_destroyGLBuffers()
{
    _destroyGLBuffer(_mass_buf);
    _destroyGLBuffer(_pos_buf);
    _destroyGLBuffer(_vel_buf);
    _destroyGLBuffer(_index_buf);
    return true;
}
