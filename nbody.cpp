#include "nbody.h"
#include "log.h"
#include "clcontext.h"
#include "clbuffer.h"
#include "clcommandqueue.h"
#include "clprogram.h"
#include "clkernel.h"
#include "clexception.h"
#include "clevent.h"
#include <QString>
#include <QFile>
#include <math.h>


#define LOG_WHO "NBody"

/**
 * @brief Имя файла программы OpenCL.
 */
static const char* clprogram_file_name = "nbody.cl";

/**
 * @brief Имя функции - ядра программы OpenCL.
 */
static const char* clprogram_kernel_name = "kernel_main";

/*
 * Константы - индексы аргументов ядра OpenCL.
 */
#define KERNEL_MAIN_ARG_COUNT 0
#define KERNEL_MAIN_ARG_POSITIONS_IN 1
#define KERNEL_MAIN_ARG_POSITIONS_OUT 2
#define KERNEL_MAIN_ARG_VELOCITIES_IN 3
#define KERNEL_MAIN_ARG_VELOCITIES_OUT 4
#define KERNEL_MAIN_ARG_MASSES 5
#define KERNEL_MAIN_ARG_DT 6


NBody::NBody(QObject *parent) :
    QObject(parent)
{
    bodies_count = 0;

    is_ready = false;

    current_in = 0;
    current_out = 1;

    gl_index_buf = new QGLBuffer(QGLBuffer::IndexBuffer);
    gl_mass_buf = new QGLBuffer(QGLBuffer::VertexBuffer);
    cl_mass_buf = new CLBuffer();
    for(size_t i = 0; i < switch_buffers_count; i ++){
        gl_pos_buf[i] = new QGLBuffer(QGLBuffer::VertexBuffer);
        gl_vel_buf[i] = new QGLBuffer(QGLBuffer::VertexBuffer);
        cl_pos_buf[i] = new CLBuffer();
        cl_vel_buf[i] = new CLBuffer();
    }

    global_dims[0] = 0;
    global_dims[1] = 0;
    local_dims[0] = 0;
    local_dims[1] = 0;

    clcxt = new CLContext();
    clqueue = new CLCommandQueue();
    clprogram = new CLProgram();
    clkernel = new CLKernel();
    clevent = new CLEvent();
}

NBody::~NBody()
{
    delete clevent;
    delete clkernel;
    delete clprogram;
    delete clqueue;
    delete clcxt;

    delete gl_index_buf;
    delete cl_mass_buf;
    delete gl_mass_buf;
    for(size_t i = 0; i < switch_buffers_count; i ++){
        delete cl_pos_buf[i];
        delete cl_vel_buf[i];
        delete gl_pos_buf[i];
        delete gl_vel_buf[i];
    }
}

size_t NBody::bodiesCount() const
{
    return bodies_count;
}

CLContext *NBody::clcontext()
{
    return clcxt;
}

bool NBody::create(const CLPlatform& platform, const CLDevice& device, size_t bodies)
{
    if(is_ready) destroy();

    if(bodies == 0) return false;

    bodies_count = bodies;

    if(!createGLBuffers()) return false;

    if(!initOpenCL(platform, device)){
        destroyGLBuffers();
        return false;
    }

    is_ready = true;

    return true;
}

bool NBody::destroy()
{
    if(!is_ready) return false;

    clqueue->finish();
    is_ready = false;

    termOpenCL();
    destroyGLBuffers();

    return true;
}

bool NBody::setMasses(const QVector<qreal> &data, size_t offset)
{
    return setGLBufferData(gl_mass_buf, data, offset);
}

bool NBody::setPositions(const QVector<QVector3D> &data, size_t offset)
{
    return setGLBufferData(gl_pos_buf[current_in], data, offset);
}

bool NBody::setVelocities(const QVector<QVector3D> &data, size_t offset)
{
    return setGLBufferData(gl_vel_buf[current_in], data, offset);
}

QGLBuffer *NBody::indexBuffer()
{
    return gl_index_buf;
}

QGLBuffer *NBody::massBuffer()
{
    return gl_mass_buf;
}

QGLBuffer *NBody::posBuffer()
{
    return gl_pos_buf[current_in];
}

QGLBuffer *NBody::velBuffer()
{
    return gl_vel_buf[current_in];
}

bool NBody::initOpenCL(const CLPlatform &platform, const CLDevice &device)
{
    try{
        log(Log::INFO, LOG_WHO, tr("Using OpenCL device: %1").arg(device.name()));

        if(!clcxt->create(platform, CLDeviceList() << device, true)) return false;

        if(!clqueue->create(*clcxt, device)){
            termOpenCL();
            return false;
        }

        if(!createCLBuffers()){
            termOpenCL();
            return false;
        }

        if(!createCLProgram()){
            termOpenCL();
            return false;
        }

        if(!calculateDimsSizes()){
            termOpenCL();
            return false;
        }

    }catch(CLException& e){
        log(Log::ERROR, LOG_WHO, e.what());
        termOpenCL();
        return false;
    }

    return true;
}

bool NBody::termOpenCL()
{
    destroyCLObject(clkernel);
    destroyCLObject(clprogram);
    destroyCLBuffers();
    destroyCLObject(clqueue);
    destroyCLObject(clcxt);
    return true;
}

bool NBody::calculateDimsSizes()
{
    try{
        CLDevice device = clcxt->devices().first();

        if(device.maxWorkItemDimensions() < 2){
            log(Log::ERROR, LOG_WHO, tr("Two or more dimensions is not supported"));
            return false;
        }

        size_t dev_work_grp_size = device.maxWorkGroupSize();
        size_t knl_work_grp_size = clkernel->workGroupSize(device);

        size_t local_work_size =
                static_cast<size_t>(pow(2.0,
                    floor(
                        log(
                            sqrt(
                                std::min(dev_work_grp_size,
                                         knl_work_grp_size)//Наименьшее число рабочих элементов.
                            )
                        ) / log(2.0)
                    )
                ));

        local_dims[0] = local_work_size;
        local_dims[1] = local_work_size;

        size_t dim_size = static_cast<size_t>(ceil(sqrt(bodies_count)));
        if(dim_size < local_work_size) dim_size = local_work_size;

        if(dim_size % local_work_size == 0){
            global_dims[0] = dim_size;
            global_dims[1] = dim_size;
        }else{
            global_dims[0] = (dim_size / local_work_size + 1) * local_work_size;

            dim_size = bodies_count / global_dims[0];
            if(dim_size < local_work_size) dim_size = local_work_size;

            if(dim_size % local_work_size == 0){
                global_dims[1] = dim_size;
            }else{
                global_dims[1] = (dim_size / local_work_size + 1) * local_work_size;
            }
        }
        log(Log::INFO, LOG_WHO, tr("Using local work size %1x%1").arg(local_dims[0]));
        log(Log::INFO, LOG_WHO, tr("Using global work size %1x%2").arg(global_dims[0]).arg(global_dims[1]));

    }catch(CLException& e){
        log(Log::ERROR, LOG_WHO, e.what());
        return false;
    }

    return true;
}

bool NBody::createCLProgram()
{
    QFile file(clprogram_file_name);
    if(!file.open(QIODevice::ReadOnly)){
        log(Log::ERROR, LOG_WHO, tr("Error open OpenCL program file"));
        return false;
    }

    QString source = file.readAll();

    file.close();

    if(source.isEmpty()){
        log(Log::ERROR, LOG_WHO, tr("Invalid OpenCL program from file"));
        return false;
    }

    try{
        clprogram->create(*clcxt, source);
    }catch(CLException& e){
        log(Log::ERROR, LOG_WHO, e.what());
        return false;
    }

    try{
        clprogram->build(clcxt->devices(), QStringList() << "-cl-mad-enable" << "-cl-fast-relaxed-math");
    }catch(CLException& e){
        log(Log::ERROR, LOG_WHO, e.what());

        try{
            QString buildLog = clprogram->buildLog(clcxt->devices().first());
            log(Log::ERROR, LOG_WHO, tr("Build log:\n") + buildLog);
        }catch(CLException& e){
            log(Log::ERROR, LOG_WHO, e.what());
        }
        CLProgram::unloadCompiler();
        return false;
    }

    CLProgram::unloadCompiler();

    try{
        clkernel->create(*clprogram, clprogram_kernel_name);
        clkernel->setArg<unsigned int>(KERNEL_MAIN_ARG_COUNT, bodies_count);
        clkernel->setArg<cl_mem>(KERNEL_MAIN_ARG_MASSES, cl_mass_buf->id());
    }catch(CLException& e){
        log(Log::ERROR, LOG_WHO, e.what());

        return false;
    }

    return true;
}

template <typename T>
bool NBody::destroyCLObject(T *clobj)
{
    try{
        if(clobj->isValid()) return clobj->release();
    }catch(CLException& e){
        log(Log::ERROR, LOG_WHO, e.what());
    }
    return false;
}

void NBody::switchCurrentBuffers()
{
    if(++ current_out >= switch_buffers_count) current_out = 0;
    if(++ current_in >= switch_buffers_count) current_in = 0;
}

bool NBody::createGLBuffers()
{
    bool res = false;

    res = createGLBuffer(gl_mass_buf,  QGLBuffer::StaticDraw, sizeof(float)) &&
          createGLBuffer(gl_index_buf, QGLBuffer::StaticDraw, sizeof(unsigned int));
    if(!res){
        destroyGLBuffers();
        return false;
    }


    for(size_t i = 0; i < switch_buffers_count; i ++){
        res = createGLBuffer(gl_pos_buf[i], QGLBuffer::StaticDraw, sizeof(float) * 3) &&
              createGLBuffer(gl_vel_buf[i], QGLBuffer::StaticDraw, sizeof(float) * 3);
        if(!res){
            destroyGLBuffers();
            return false;
        }
    }

    gl_index_buf->bind();
        unsigned int* ptr = static_cast<unsigned int*>(gl_index_buf->map(QGLBuffer::WriteOnly));

        if(ptr == nullptr){
            destroyGLBuffers();
            return false;
        }

        for(unsigned int i = 0; i < bodies_count; i ++){
            ptr[i] = i;
        }
    gl_index_buf->unmap();
    gl_index_buf->release();

    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    return true;
}

bool NBody::destroyGLBuffers()
{
    glFinish();

    destroyGLBuffer(gl_index_buf);
    destroyGLBuffer(gl_mass_buf);
    for(size_t i = 0; i < switch_buffers_count; i ++){
        destroyGLBuffer(gl_pos_buf[i]);
        destroyGLBuffer(gl_vel_buf[i]);
    }
    return true;
}

bool NBody::createGLBuffer(QGLBuffer *buf, QGLBuffer::UsagePattern usage, size_t item_size_bytes)
{
    if(!buf->create()) return false;
    if(!buf->bind()) return false;
    buf->setUsagePattern(usage);
    buf->allocate(item_size_bytes * bodies_count);
    return true;
}

bool NBody::destroyGLBuffer(QGLBuffer *buf)
{
    if(buf && buf->isCreated()){
        buf->destroy();
        return true;
    }
    return false;
}

bool NBody::setGLBufferData(QGLBuffer *buf, const QVector<QVector3D> &data, size_t offset)
{
    if(!buf->isCreated()) return false;
    if((offset + static_cast<size_t>(data.size())) * 3 > static_cast<size_t>(buf->size())) return false;

    buf->bind();
        float* ptr = static_cast<float*>(buf->map(QGLBuffer::WriteOnly));

        if(ptr == nullptr) return false;

        ptr += offset * 3;

        for(size_t i = 0; i < static_cast<size_t>(data.size()); i ++){
            ptr[0] = data.at(i).x();
            ptr[1] = data.at(i).y();
            ptr[2] = data.at(i).z();
            ptr += 3;
        }
    buf->unmap();
    buf->release();

    return true;
}

bool NBody::setGLBufferData(QGLBuffer *buf, const QVector<qreal> &data, size_t offset)
{
    if(!buf->isCreated()) return false;
    if((offset + static_cast<size_t>(data.size())) > static_cast<size_t>(buf->size())) return false;

    buf->bind();
        float* ptr = static_cast<float*>(buf->map(QGLBuffer::WriteOnly));

        if(ptr == nullptr) return false;

        ptr += offset;

        for(size_t i = 0; i < static_cast<size_t>(data.size()); i ++){
            ptr[i] = data.at(i);
        }
    buf->unmap();
    buf->release();

    return true;
}

bool NBody::createCLBuffers()
{
    bool res = false;

    res = createCLBuffer(cl_mass_buf, CL_MEM_READ_WRITE, gl_mass_buf);
    if(!res) return false;

    for(size_t i = 0; i < switch_buffers_count; i ++){
        res = createCLBuffer(cl_pos_buf[i], CL_MEM_READ_WRITE, gl_pos_buf[i]) &&
              createCLBuffer(cl_vel_buf[i], CL_MEM_READ_WRITE, gl_vel_buf[i]);
        if(!res){
            destroyCLBuffers();
            return false;
        }
    }
    return true;
}

bool NBody::destroyCLBuffers()
{
    destroyCLBuffer(cl_mass_buf);
    for(size_t i = 0; i < switch_buffers_count; i ++){
        destroyCLBuffer(cl_pos_buf[i]);
        destroyCLBuffer(cl_vel_buf[i]);
    }
    return true;
}

bool NBody::createCLBuffer(CLBuffer *clbuf, cl_mem_flags flags, QGLBuffer *glbuf)
{
    try{
        return clbuf->createFromGLBuffer(*clcxt, flags, glbuf->bufferId());
    }catch(CLException& e){
        log(Log::ERROR, LOG_WHO, e.what());
    }
    return false;
}

bool NBody::destroyCLBuffer(CLBuffer *buf)
{
    return destroyCLObject(buf);
}
