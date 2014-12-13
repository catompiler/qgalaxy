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
#define KERNEL_MAIN_ARG_POS_CACHE 7
#define KERNEL_MAIN_ARG_MASS_CACHE 8
#define KERNEL_MAIN_ARG_CACHE_SIZE 9



NBody::NBody(QObject *parent) :
    QObject(parent)
{
    bodies_count = 0;
    simulated_bodies_count = 0;

    time_step = 0.0f;

    is_ready = false;

    current_in = 0;
    current_out = 1;

    gl_index_buf = new NBodyGLBuffer(NBodyGLBuffer::IndexBuffer);
    gl_mass_buf = new NBodyGLBuffer(NBodyGLBuffer::VertexBuffer);
    cl_mass_buf = new CLBuffer();
    for(size_t i = 0; i < switch_buffers_count; i ++){
        gl_pos_buf[i] = new NBodyGLBuffer(NBodyGLBuffer::VertexBuffer);
        gl_vel_buf[i] = new NBodyGLBuffer(NBodyGLBuffer::VertexBuffer);
        cl_pos_buf[i] = new CLBuffer();
        cl_vel_buf[i] = new CLBuffer();
    }

    global_dims[0] = 0;
    local_dims[0] = 0;
#if NDRANGE_DIMENSIONS > 1
    global_dims[1] = 0;
    local_dims[1] = 0;
#endif

    clcxt = new CLContext();
    clqueue = new CLCommandQueue();
    clprogram = new CLProgram();
    clkernel = new CLKernel();
    clevent = new CLEvent();

    connect(clevent, SIGNAL(completed(int)), this, SIGNAL(simulationFinished()));
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

size_t NBody::simulatedBodiesCount() const
{
    return simulated_bodies_count;
}

bool NBody::setSimulatedBodiesCount(size_t count)
{
    if(count > bodies_count) return false;
    simulated_bodies_count = count;
    return true;
}

float NBody::timeStep() const
{
    return time_step;
}

void NBody::setTimeStep(float dt)
{
    time_step = dt;
}

CLContext *NBody::clcontext()
{
    return clcxt;
}

/**
 * @brief Инициализация систему NBody.
 * @param platform Платформа OpenCL.
 * @param device Устройство OpenCL.
 * @param bodies Число тел.
 * @return true в случае успеха, иначе false.
 */
bool NBody::create(const CLPlatform& platform, const CLDevice& device, size_t bodies)
{
    // Нет смысла инициализировать систему из 0 тел.
    if(bodies == 0) return false;

    try{
        // Получим максимальную доступную нам память устрйоства.
        size_t mem_size = device.maxMemAllocSize();
        // Если нам нужно больше.
        if(mem_size < bodies * sizeof(float) * (3 * 2 /*pos,vel_in*/ + 3 * 2 /*pos,vel_out*/ + 1 /*masses*/)){
            // Сообщим об этом.
            log(Log::ERROR, LOG_WHO, tr("Out of memory!"));
            // Возврат.
            return false;
        }
    }// Если произошка ошибка.
    catch(CLException& e){
        // Сообщим об этом и считаем что памяти достаточно.
        log(Log::WARNING, LOG_WHO, e.what());
    }

    // Если система уже была создана - уничтожим её.
    if(is_ready) destroy();

    // Теперь система не создана.
    is_ready = false;
    // Установим новое число тел.
    bodies_count = bodies;
    // Установим моделируемое число тел.
    simulated_bodies_count = bodies;

    // Если не удалось создать буфера OpenGL.
    if(!createGLBuffers()){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, tr("Error creating OpenGL buffers!"));
        // Возврат.
        return false;
    }

    // Если не удалось проинииализировать OpenCL.
    if(!initOpenCL(platform, device)){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, tr("Error initializing OpenCL!"));
        // Уничтожим буферы OpenGL.
        destroyGLBuffers();
        // Возврат.
        return false;
    }

    // Установим флаг готовности к симуляции.
    is_ready = true;

    // Возврат успеха.
    return true;
}

bool NBody::destroy()
{
    if(!is_ready) return false;

    try{
        clqueue->finish();
    }catch(CLException& e){
        log(Log::WARNING, LOG_WHO, e.what());
    }
    is_ready = false;

    termOpenCL();
    destroyGLBuffers();

    return true;
}

bool NBody::reset()
{
    if(!isReady() || isRunning()) return false;

    simulated_bodies_count = bodies_count;

    QVector<Point3f> data(bodies_count);

    if(!setGLBufferData(gl_mass_buf, *reinterpret_cast<QVector<float>*>(&data))) return false;
    for(size_t i = 0; i < switch_buffers_count; i ++){
        if(!setGLBufferData(gl_pos_buf[i], data)) return false;
        if(!setGLBufferData(gl_vel_buf[i], data)) return false;
    }

    return true;
}

bool NBody::isReady() const
{
    return is_ready;
}

bool NBody::isRunning() const
{
    if(!clevent->isValid()) return false;

    bool res = false;
    try{
        res = clevent->isRunning();
    }catch(CLException& e){
        log(Log::WARNING, LOG_WHO, e.what());
    }
    return res;
}

bool NBody::wait() const
{
    if(!isReady() || !isRunning()) return false;

    try{
        return clevent->wait();
    }catch(CLException& e){
        log(Log::WARNING, LOG_WHO, e.what());
    }
    return false;
}

bool NBody::setMasses(const QVector<qreal> &data, size_t offset)
{
    if(!isReady() || isRunning()) return false;
    return setGLBufferData(gl_mass_buf, data, offset);
}

bool NBody::setMasses(const QVector<float> &data, size_t offset)
{
    if(!isReady() || isRunning()) return false;
    return setGLBufferData(gl_mass_buf, data, offset);
}

bool NBody::setPositions(const QVector<QVector3D> &data, size_t offset)
{
    if(!isReady() || isRunning()) return false;
    return setGLBufferData(gl_pos_buf[current_in], data, offset);
}

bool NBody::setPositions(const QVector<Point3f> &data, size_t offset)
{
    if(!isReady() || isRunning()) return false;
    return setGLBufferData(gl_pos_buf[current_in], data, offset);
}

bool NBody::setVelocities(const QVector<QVector3D> &data, size_t offset)
{
    if(!isReady() || isRunning()) return false;
    return setGLBufferData(gl_vel_buf[current_in], data, offset);
}

bool NBody::setVelocities(const QVector<Point3f> &data, size_t offset)
{
    if(!isReady() || isRunning()) return false;
    return setGLBufferData(gl_vel_buf[current_in], data, offset);
}

bool NBody::getMasses(QVector<qreal> &data, size_t offset, size_t count) const
{
    if(!isReady() || isRunning()) return false;
    return getGLBufferData(gl_mass_buf, data, offset, count);
}

bool NBody::getMasses(QVector<float> &data, size_t offset, size_t count) const
{
    if(!isReady() || isRunning()) return false;
    return getGLBufferData(gl_mass_buf, data, offset, count);
}

bool NBody::getPositions(QVector<QVector3D> &data, size_t offset, size_t count) const
{
    if(!isReady() || isRunning()) return false;
    return getGLBufferData(gl_pos_buf[current_in], data, offset, count);
}

bool NBody::getPositions(QVector<Point3f> &data, size_t offset, size_t count) const
{
    if(!isReady() || isRunning()) return false;
    return getGLBufferData(gl_pos_buf[current_in], data, offset, count);
}

bool NBody::getVelocities(QVector<QVector3D> &data, size_t offset, size_t count) const
{
    if(!isReady() || isRunning()) return false;
    return getGLBufferData(gl_vel_buf[current_in], data, offset, count);
}

bool NBody::getVelocities(QVector<Point3f> &data, size_t offset, size_t count) const
{
    if(!isReady() || isRunning()) return false;
    return getGLBufferData(gl_vel_buf[current_in], data, offset, count);
}

NBodyGLBuffer *NBody::indexBuffer()
{
    return gl_index_buf;
}

NBodyGLBuffer *NBody::massBuffer()
{
    return gl_mass_buf;
}

NBodyGLBuffer *NBody::posBuffer()
{
    return gl_pos_buf[current_in];
}

NBodyGLBuffer *NBody::velBuffer()
{
    return gl_vel_buf[current_in];
}

/**
 * @brief Запускает расчёт симуляции очередного шага.
 * Использует установленное время шага.
 * @return true в случае успеха, иначе false.
 */
bool NBody::simulate()
{
    // Запустить симуляцию с ранее установленным временем шага.
    return simulate(time_step);
}

/**
 * @brief Запускает расчёт симуляции очередного шага.
 * @param dt Время шага.
 * @return true в случае успеха, иначе false.
 */
bool NBody::simulate(float dt)
{
    // Если не готовы, либо симуляция уже просчитывается - возврат.
    if(!isReady() || isRunning()) return false;

    // Результат.
    bool res = true;

    // Подождём завершения операций OpenGL.
    glFinish();

    // Если событие OpenCL создано.
    if(clevent->isValid()){
        // Уничтожим его.
        try{ clevent->release(); }catch(CLException& e){ log(Log::WARNING, LOG_WHO, e.what()); }
    }

    try{
        // Захватим буфера OpenGL.
        cl_mass_buf->enqueueAcquireGLObject(*clqueue);
        for(size_t i = 0; i < switch_buffers_count; i ++){
            cl_pos_buf[i]->enqueueAcquireGLObject(*clqueue);
            cl_vel_buf[i]->enqueueAcquireGLObject(*clqueue);
        }

        // Установим аргументы ядра OpenCL.
        clkernel->setArg<float>(KERNEL_MAIN_ARG_DT, dt);
        clkernel->setArg<cl_mem>(KERNEL_MAIN_ARG_POSITIONS_IN,  cl_pos_buf[current_in ]->id());
        clkernel->setArg<cl_mem>(KERNEL_MAIN_ARG_POSITIONS_OUT, cl_pos_buf[current_out]->id());
        clkernel->setArg<cl_mem>(KERNEL_MAIN_ARG_VELOCITIES_IN,  cl_vel_buf[current_in ]->id());
        clkernel->setArg<cl_mem>(KERNEL_MAIN_ARG_VELOCITIES_OUT, cl_vel_buf[current_out]->id());
        clkernel->setArg<unsigned int>(KERNEL_MAIN_ARG_COUNT, simulated_bodies_count);//bodies_count

        // Запустим программу OpenCL.
        clkernel->execute(*clqueue, NDRANGE_DIMENSIONS, global_dims, local_dims);

    }// Если произошла ошибка.
    catch(CLException& e){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, e.what());
        // Результат - ошибка.
        res = false;
    }

    // Освободим буферы OpenGL.
    try{cl_mass_buf->enqueueReleaseGLObject(*clqueue); }catch(CLException& e){ log(Log::WARNING, LOG_WHO, e.what()); }
    for(size_t i = 0; i < switch_buffers_count; i ++){
        try{ cl_pos_buf[i]->enqueueReleaseGLObject(*clqueue); }catch(CLException& e){ log(Log::WARNING, LOG_WHO, e.what()); }
        try{ cl_vel_buf[i]->enqueueReleaseGLObject(*clqueue); }catch(CLException& e){ log(Log::WARNING, LOG_WHO, e.what()); }
    }

    // Если всё прошло успешно.
    if(res){

        // Переключим буферы для чтения и записи.
        switchCurrentBuffers();

        // Флаг установки маркера в очередь OpenCL.
        bool add_marker_res = false;
        try{
            // Установим маркер в очередь OpenCL.
            add_marker_res = clqueue->marker(clevent);
            // Отправим все команды на устройство.
            //clqueue->flush();
        }// Если произошла ошибка.
        catch(CLException& e){
            // Сообщим об этом.
            log(Log::WARNING, LOG_WHO, e.what());
        }

        // Если маркер не установлен.
        if(!add_marker_res){
            // Сообщим об этом.
            log(Log::WARNING, LOG_WHO, tr("Error enqueueing marker, waiting"));
            try{
                // Подождём завершения вычислений.
                res = clqueue->finish();
            }// Если произошла ошибка.
            catch(CLException& e){
                // Сообщим об этом.
                log(Log::WARNING, LOG_WHO, e.what());
            }
            // Пошлём сообщение окончания расчётов.
            emit simulationFinished();
        }
    }

    // Возврат результата.
    return res;
}

/**
 * @brief Инициализирует OpenCL.
 * @param platform Платформа OpenCL.
 * @param device Устройство OpenCL.
 * @return true в случае успеха, иначе false.
 */
bool NBody::initOpenCL(const CLPlatform &platform, const CLDevice &device)
{
    try{
        // Сообщим используемое устройство OpenCL.
        log(Log::INFO, LOG_WHO, tr("Using OpenCL device: %1").arg(device.name()));

        // Если не удалось сосздать контекст OpenCL.
        if(!clcxt->create(platform, CLDeviceList() << device, true)){
            // Возврат.
            return false;
        }

        // Если не удалось создать очередь команд OpenCL.
        if(!clqueue->create(*clcxt, device)){
            // Уничтожим OpenCL.
            termOpenCL();
            // Возврат.
            return false;
        }

        // Если не удалось создать буферы OpenCL.
        if(!createCLBuffers()){
            // Уничтожим OpenCL.
            termOpenCL();
            // Возврат.
            return false;
        }

        // Если не удалось создать программу OpenCL.
        if(!createCLProgram()){
            // Уничтожим OpenCL.
            termOpenCL();
            // Возврат.
            return false;
        }

        // Если не удалось вычислить размер NDRange OpenCL.
        if(!calculateDimsSizes()){
            // Уничтожим OpenCL.
            termOpenCL();
            // Возврат.
            return false;
        }
    }// Если произошла ошибка.
    catch(CLException& e){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, e.what());
        // Уничтожим OpenCL.
        termOpenCL();
        // Возврат.
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

/*bool NBody::calculateDimsSizes()
{
    try{
        CLDevice device = clcxt->devices().first();

        if(device.maxWorkItemDimensions() < 2){
            log(Log::ERROR, LOG_WHO, tr("Two or more dimensions is not supported"));
            return false;
        }

        size_t dev_work_grp_size = device.maxWorkGroupSize();
        size_t knl_work_grp_size = clkernel->workGroupSize(device);
        size_t dev_compute_units = device.maxComputeUnits();

        log(Log::INFO, LOG_WHO, tr("Device max compute units: %1").arg(dev_compute_units));
        log(Log::INFO, LOG_WHO, tr("Device max work group size: %1").arg(dev_work_grp_size));
        log(Log::INFO, LOG_WHO, tr("Kernel max work group size: %1").arg(knl_work_grp_size));

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
}*/

/**
 * @brief Вычисляет размеры измерений X,Y.
 * @return true в случае успеха, иначе false.
 */
bool NBody::calculateDimsSizes()
{
    try{
        // Получим устройство OpenCL.
        CLDevice device = clcxt->devices().first();

#if NDRANGE_DIMENSIONS > 1
        // Если устройство не поддерживает больше одного измерения.
        if(device.maxWorkItemDimensions() < 2){
            // Сообщим об этом.
            log(Log::ERROR, LOG_WHO, tr("Two or more dimensions is not supported"));
            // Возврат.
            return false;
        }
#endif

        // Получим размер рабочей группы устройства.
        size_t dev_work_grp_size = device.maxWorkGroupSize();
        // Получим размер рабочей группы ядра.
        size_t knl_work_grp_size = clkernel->workGroupSize(device);
        // Получим число вычислительных элементов.
        size_t dev_compute_units = device.maxComputeUnits();

        // Сообщим полученные числа.
        log(Log::INFO, LOG_WHO, tr("Device max compute units: %1").arg(dev_compute_units));
        log(Log::INFO, LOG_WHO, tr("Device max work group size: %1").arg(dev_work_grp_size));
        log(Log::INFO, LOG_WHO, tr("Kernel max work group size: %1").arg(knl_work_grp_size));

        // Выберем допустимый размер рабочей группы.
        size_t local_work_size = std::min(dev_work_grp_size,
                                          knl_work_grp_size);

        // Установим размер локальных измерений.
        local_dims[0] = local_work_size;
#if NDRANGE_DIMENSIONS > 1
        local_dims[1] = 1;
#endif
        // Установим размер глобальных измерений.
        global_dims[0] = static_cast<size_t>(ceil(static_cast<float>(bodies_count) / local_work_size)) * local_work_size;
#if NDRANGE_DIMENSIONS > 1
        global_dims[1] = 1;
#endif

        // Сообщим рекомендованное число звёзд.
        log(Log::INFO, LOG_WHO, tr("Recommended maximum number of stars: %1").arg(local_work_size * dev_compute_units));

        // Сообщим используемый размер локальных измерений.
        log(Log::INFO, LOG_WHO, tr("Using local work size %1x%2").arg(local_dims[0])
#if NDRANGE_DIMENSIONS > 1
                .arg(local_dims[1]));
#else
                .arg(1));
#endif
        // Сообщим используемый размер глобальных измерений.
        log(Log::INFO, LOG_WHO, tr("Using global work size %1x%2").arg(global_dims[0])
#if NDRANGE_DIMENSIONS > 1
                .arg(global_dims[1]));
#else
                .arg(1));
#endif

    }// Если произошла ошибка.
    catch(CLException& e){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, e.what());
        // Возврат.
        return false;
    }

    // Возврат успеха.
    return true;
}

/**
 * @brief Создаёт, считывает и компилирует программу OpenCL.
 * @return true в случае успеха, иначе false.
 */
bool NBody::createCLProgram()
{
    // Файл программы.
    QFile file(clprogram_file_name);
    // Если не удалось открыть файл.
    if(!file.open(QIODevice::ReadOnly)){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, tr("Error open OpenCL program file"));
        // Возврат.
        return false;
    }

    // Считаем код программы из файла.
    QString source = file.readAll();

    // Закроем файл.
    file.close();

    // Если файл пуст, либо произошла ошибка чтения.
    if(source.isEmpty()){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, tr("Invalid OpenCL program from file"));
        // Возврат.
        return false;
    }

    try{
        // Попытаемся создать программу со считанным исходным кодом.
        clprogram->create(*clcxt, source);
    }// Если произошла ошибка.
    catch(CLException& e){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, e.what());
        // Возврат.
        return false;
    }

    try{
        // Попытаемся скомпилировать программу.
        clprogram->build(clcxt->devices(), QStringList() << "-cl-mad-enable" << "-cl-fast-relaxed-math");
    }// Если произошла ошибка.
    catch(CLException& e){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, e.what());

        try{
            // Получим лог компиляции.
            QString buildLog = clprogram->buildLog(clcxt->devices().first());
            // Выведем его в лог.
            log(Log::ERROR, LOG_WHO, tr("Build log:\n") + buildLog);
        }// Если произошла ошибка.
        catch(CLException& e){
            // Сообщим об этом.
            log(Log::ERROR, LOG_WHO, e.what());
        }
        // Выгрузим компилятор OpenCL.
        CLProgram::unloadCompiler();
        // Возврат.
        return false;
    }

    // Выгрузим компилятор OpenCL.
    CLProgram::unloadCompiler();

    try{
        // Создадим ядро программы OpenCL.
        clkernel->create(*clprogram, clprogram_kernel_name);
    }// Если произошла ошибка.
    catch(CLException& e){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, e.what());
        // Возврат.
        return false;
    }

    // Размер кэша по-умолчанию -
    // минимальный объём памяти
    // для работы.
    size_t cache_count = 1;

    try{
        // Устройство OpenCL.
        CLDevice device = clcxt->devices().first();

        // Объём локальной памяти.
        size_t local_mem = device.localMemSize() - clkernel->localMemSize(device);

        // Сообщим.
        log(Log::INFO, LOG_WHO, tr("Available local memory size: %1 byte(s)").arg(local_mem));

        // Если памяти недостаточно.
        if(local_mem < (sizeof(unsigned int) + sizeof(float) * 3)){
            // Сообщим об этом.
            log(Log::ERROR, LOG_WHO, tr("Local memory is too small!"));
            // Возврат.
            return false;
        }
        // Вычислим число кэшированных элементов.
        cache_count = local_mem / (sizeof(unsigned int) + sizeof(float) * 3);

        // Число рабочих элементов.
        size_t work_items = clkernel->workGroupSize(device);

        // Нет смысла в кэше больше чем число рабочих элементов.
        if(cache_count > work_items) cache_count = work_items;

    }// Если произошла ошибка.
    catch(CLException& e){
        // Сообщим об этом.
        log(Log::WARNING, LOG_WHO, e.what());
    }

    log(Log::INFO, LOG_WHO, tr("Number of cached data: %1").arg(cache_count));

    try{
        // Установим неизменяемые аргументы.
        // Буфер масс.
        clkernel->setArg<cl_mem>(KERNEL_MAIN_ARG_MASSES, cl_mass_buf->id());
        clkernel->setLocalArgSize(KERNEL_MAIN_ARG_POS_CACHE, cache_count * sizeof(float) * 3);
        clkernel->setLocalArgSize(KERNEL_MAIN_ARG_MASS_CACHE, cache_count * sizeof(float));
        clkernel->setArg<unsigned int>(KERNEL_MAIN_ARG_CACHE_SIZE, cache_count);
    }// Если произошла ошибка.
    catch(CLException& e){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, e.what());
        // Возврат.
        return false;
    }

    // Возврат успеха.
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

    QVector<float> init_data(bodies_count * 3);

    res = createGLBuffer(gl_mass_buf,  NBodyGLBuffer::StaticDraw, sizeof(float), init_data.data()) &&
          createGLBuffer(gl_index_buf, NBodyGLBuffer::StaticDraw, sizeof(unsigned int));
    if(!res){
        destroyGLBuffers();
        return false;
    }

    for(size_t i = 0; i < switch_buffers_count; i ++){
        res = createGLBuffer(gl_pos_buf[i], NBodyGLBuffer::StaticDraw, sizeof(float) * 3, init_data.data()) &&
              createGLBuffer(gl_vel_buf[i], NBodyGLBuffer::StaticDraw, sizeof(float) * 3, init_data.data());
        if(!res){
            destroyGLBuffers();
            return false;
        }
    }

    if(!gl_index_buf->bind()){
        destroyGLBuffers();
        return false;
    }
        unsigned int* ptr = static_cast<unsigned int*>(gl_index_buf->map(NBodyGLBuffer::WriteOnly));

        if(ptr == nullptr){
            destroyGLBuffers();
            return false;
        }

        for(unsigned int i = 0; i < bodies_count; i ++){
            ptr[i] = i;
        }
    gl_index_buf->unmap();
    gl_index_buf->release();

    NBodyGLBuffer::release(NBodyGLBuffer::VertexBuffer);
    NBodyGLBuffer::release(NBodyGLBuffer::IndexBuffer);

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

bool NBody::createGLBuffer(NBodyGLBuffer *buf, NBodyGLBuffer::UsagePattern usage, size_t item_size_bytes, const void *data)
{
    if(!buf->create()) return false;
    if(!buf->bind()) return false;
    buf->setUsagePattern(usage);
    buf->allocate(data, item_size_bytes * bodies_count);
    return true;
}

bool NBody::destroyGLBuffer(NBodyGLBuffer *buf)
{
    if(buf && buf->isCreated()){
        buf->destroy();
        return true;
    }
    return false;
}

bool NBody::setGLBufferData(NBodyGLBuffer *buf, const QVector<QVector3D> &data, size_t offset)
{
    if(!buf->isCreated()) return false;
    if((offset + static_cast<size_t>(data.size())) * 3 > static_cast<size_t>(buf->size())) return false;

    if(!buf->bind()) return false;
        float* ptr = static_cast<float*>(buf->map(NBodyGLBuffer::WriteOnly));

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

bool NBody::setGLBufferData(NBodyGLBuffer *buf, const QVector<Point3f> &data, size_t offset)
{
    if(!buf->isCreated()) return false;
    if((offset + static_cast<size_t>(data.size())) * 3 > static_cast<size_t>(buf->size())) return false;

    if(!buf->bind()) return false;
    buf->write(offset * sizeof(float) * 3, data.data(), data.size() * sizeof(Point3f));
    buf->release();

    return true;
}

bool NBody::setGLBufferData(NBodyGLBuffer *buf, const QVector<qreal> &data, size_t offset)
{
    if(!buf->isCreated()) return false;
    if((offset + static_cast<size_t>(data.size())) > static_cast<size_t>(buf->size())) return false;

    if(!buf->bind()) return false;
        float* ptr = static_cast<float*>(buf->map(NBodyGLBuffer::WriteOnly));

        if(ptr == nullptr) return false;

        ptr += offset;

        for(size_t i = 0; i < static_cast<size_t>(data.size()); i ++){
            ptr[i] = data.at(i);
        }
    buf->unmap();
    buf->release();

    return true;
}

bool NBody::setGLBufferData(NBodyGLBuffer *buf, const QVector<float> &data, size_t offset)
{
    if(!buf->isCreated()) return false;
    if((offset + static_cast<size_t>(data.size())) > static_cast<size_t>(buf->size())) return false;

    if(!buf->bind()) return false;
    buf->write(offset * sizeof(float), data.data(), data.size() * sizeof(float));
    buf->release();

    return true;
}

bool NBody::getGLBufferData(NBodyGLBuffer *buf, QVector<QVector3D> &data, size_t offset, size_t count) const
{
    if(!buf->isCreated()) return false;
    if((offset + count) * 3 > static_cast<size_t>(buf->size())) return false;

    if(!buf->bind()) return false;
        float* ptr = static_cast<float*>(buf->map(NBodyGLBuffer::WriteOnly));

        if(ptr == nullptr) return false;

        ptr += offset * 3;

        for(size_t i = 0; i < count; i ++){
            data.append(QVector3D(ptr[0], ptr[1], ptr[2]));
            ptr += 3;
        }
    buf->unmap();
    buf->release();

    return true;
}

bool NBody::getGLBufferData(NBodyGLBuffer *buf, QVector<Point3f> &data, size_t offset, size_t count) const
{
    if(!buf->isCreated()) return false;
    if((offset + count) * 3 > static_cast<size_t>(buf->size())) return false;

    if(!buf->bind()) return false;
    data.resize(data.size() + count);
    buf->read(offset * sizeof(float) * 3, data.data(), count * sizeof(Point3f));
    buf->release();

    return true;
}

bool NBody::getGLBufferData(NBodyGLBuffer *buf, QVector<qreal> &data, size_t offset, size_t count) const
{
    if(!buf->isCreated()) return false;
    if((offset + count) > static_cast<size_t>(buf->size())) return false;

    if(!buf->bind()) return false;
        float* ptr = static_cast<float*>(buf->map(NBodyGLBuffer::WriteOnly));

        if(ptr == nullptr) return false;

        ptr += offset;

        for(size_t i = 0; i < count; i ++){
            data.append(ptr[i]);
        }
    buf->unmap();
    buf->release();

    return true;
}

bool NBody::getGLBufferData(NBodyGLBuffer *buf, QVector<float> &data, size_t offset, size_t count) const
{
    if(!buf->isCreated()) return false;
    if((offset + count) > static_cast<size_t>(buf->size())) return false;

    if(!buf->bind()) return false;
    data.resize(data.size() + count);
    buf->read(offset * sizeof(float), data.data(), count * sizeof(float));
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

bool NBody::createCLBuffer(CLBuffer *clbuf, cl_mem_flags flags, NBodyGLBuffer *glbuf)
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
