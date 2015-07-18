#include "nbodywidget.h"
#include "log.h"
#include "nbody.h"
#include "clplatform.h"
#include "cldevice.h"
#include "settings.h"
#include <QGLFormat>
#include <QImage>
#include <QMouseEvent>
#include <QWheelEvent>
#include <GL/glu.h>
#include <QMatrix4x4>
#include <QFile>
#include <QDataStream>
#include <QDebug>


#define LOG_WHO "NBody View"


#define VIEW_DISTANCE_DEFAULT 5000.0f
//#define VIEW_DISTANCE_DEFAULT 20000.0f



PFNGLPOINTPARAMETERFARBPROC NBodyWidget::glPointParameterfARB;
PFNGLPOINTPARAMETERFVARBPROC NBodyWidget::glPointParameterfvARB;


NBodyWidget::NBodyWidget(QWidget *parent) :
    QGLWidget(parent)
{
    QGLFormat glformat = QGLFormat::defaultFormat();

    glformat.setRgba(true);
    glformat.setRedBufferSize(8);
    glformat.setGreenBufferSize(8);
    glformat.setBlueBufferSize(8);
    glformat.setAlpha(true);
    glformat.setAlphaBufferSize(8);

    glformat.setDepth(true);
    glformat.setDepthBufferSize(24);

    glformat.setStencil(false);
    glformat.setAccum(false);
    glformat.setStereo(false);

    glformat.setDoubleBuffer(true);

    glformat.setDirectRendering(true);

    glformat.setSwapInterval(0);

    glformat.setSampleBuffers(true);
    glformat.setSamples(4);
    glformat.setProfile(QGLFormat::CompatibilityProfile);
    glformat.setVersion(2, 1);

    setFormat(glformat);

    setMouseTracking(true);

    nbody = new NBody(this);
    // Соединение сигнала завершения симуляции и слота обработки завершения симуляции.
    connect(nbody, SIGNAL(simulationFinished()), this, SLOT(on_simulationFinished()));
    connect(nbody, SIGNAL(simulationFinished()), this, SIGNAL(simulationFinished()));

    sim_run = false;

    has_point_sprite = false;
    sprite_texture = 0;

    has_point_parameters = false;

    view_position = VIEW_DISTANCE_DEFAULT;
    old_event_x = 0.0f;
    old_event_y = 0.0f;
}

NBodyWidget::~NBodyWidget()
{
    makeCurrent();
    nbody->destroy();
    if(has_point_sprite){
        deleteTexture(sprite_texture);
    }
    doneCurrent();

    delete nbody;
}

size_t NBodyWidget::bodiesCount() const
{
    return nbody->bodiesCount();
}

size_t NBodyWidget::simulatedBodiesCount() const
{
    return nbody->simulatedBodiesCount();
}

bool NBodyWidget::setSimulatedBodiesCount(size_t count)
{
    return nbody->setSimulatedBodiesCount(count);
}

bool NBodyWidget::reset()
{
    view_position = VIEW_DISTANCE_DEFAULT;
    view_rotation = QQuaternion();
    bool res = nbody->reset();
    update();
    return res;
}

bool NBodyWidget::isReady() const
{
    return nbody->isReady();
}

bool NBodyWidget::isRunning() const
{
    return nbody->isRunning();
}

bool NBodyWidget::isSimulationRunning() const
{
    return sim_run;
}

float NBodyWidget::timeStep() const
{
    return nbody->timeStep();
}

double NBodyWidget::simulationTime() const
{
    return sim_time.count();
}

void NBodyWidget::setTimeStep(float dt)
{
    nbody->setTimeStep(dt);
}

/**
 * @brief Сохранение взаимодействующих тел в файл.
 * @param filename Имя файла.
 * @return true в случае успеха, иначе false.
 */
bool NBodyWidget::saveNBody(const QString &filename)
{
    // Если инициализация была неудачной, либо идёт симуляция - возврат.
    if(!nbody->isReady() || nbody->isRunning()) return false;

    // Сообщим что сохраняем данные.
    log(Log::INFO, LOG_WHO, tr("Saving file: %1").arg(filename));

    // Файл.
    QFile file(filename);
    // Есом не удалось открыть файл.
    if(!file.open(QIODevice::WriteOnly)){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, tr("Error open file!"));
        // Возврат.
        return false;
    }

    // Получим число тел.
    size_t count = nbody->simulatedBodiesCount();

    // Массив для временного хранения масс.
    QVector<float> mass;
    // Массивы для временного хранения позиций и векторов скоростей.
    QVector<Point3f> pos, vel;

    // Если не удалось получить данные.
    if(!getBodies(0, count, mass, pos, vel)){
        // Сообщим об этом.
        log(Log::WARNING, LOG_WHO, tr("Error getting bodies!"));
        // Закроем файл.
        file.close();
        // Возврат.
        return false;
    }

    // Поток данных.
    QDataStream ds(&file);
    // Установим версию,
    // Это необходимо для корректной сериализации/десериализации.
    ds.setVersion(QDataStream::Qt_4_8);

    // Запишем в файл подпись и версию формата, число тел.
    ds << data_file_magic << data_file_version << static_cast<quint32>(count);

    // Если не удалось записать.
    if(ds.status() != QDataStream::Ok){
        // Сообщим об этом.
        log(Log::WARNING, LOG_WHO, tr("Error writing data!"));
        // Закроем файл.
        file.close();
        // Возврат.
        return false;
    }
    // Если неудалось записать массы, позиции или векторы скоростей.
    if(ds.writeRawData(reinterpret_cast<char*>(mass.data()), mass.size() * sizeof(float)) == -1 ||
       ds.writeRawData(reinterpret_cast<char*>(pos.data()), pos.size() * sizeof(Point3f)) == -1 ||
       ds.writeRawData(reinterpret_cast<char*>(vel.data()), vel.size() * sizeof(Point3f)) == -1){

        // Сообщим об этом.
        log(Log::WARNING, LOG_WHO, tr("Error writing data!"));
        // Закроем файл.
        file.close();
        // Возврат.
        return false;
    }

    // Закроем файл.
    file.close();

    // Сообщим об успешном сохранении данных.
    log(Log::INFO, LOG_WHO, tr("File saved!"));

    // Возврат успешного результата.
    return true;
}

/**
 * @brief Загрузка взаимодействующих тел из файла.
 * @param filename Имя файла.
 * @return true в случае успеха, иначе false.
 */
bool NBodyWidget::openNBody(const QString &filename)
{
    // Если инициализация была неудачной, либо идёт симуляция - возврат.
    if(!nbody->isReady() || nbody->isRunning()) return false;

    // Сообщим что загружаем данные.
    log(Log::INFO, LOG_WHO, tr("Opening file: %1").arg(filename));

    // Файл.
    QFile file(filename);
    // Есом не удалось открыть файл.
    if(!file.open(QIODevice::ReadOnly)){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, tr("Error open file!"));
        // Возврат.
        return false;
    }

    // Массив для временного хранения масс.
    QVector<float> mass;
    // Массивы для временного хранения позиций и векторов скоростей.
    QVector<Point3f> pos, vel;

    // Подпись и версия формата файла.
    quint32 magic, version;
    // Число тел.
    unsigned int count;

    // Поток данных.
    QDataStream ds(&file);
    // Установим версию,
    // Это необходимо для корректной сериализации/десериализации.
    ds.setVersion(QDataStream::Qt_4_8);

    // Считаем подпись формата.
    ds >> magic;

    // Если формат некорректен.
    if(magic != data_file_magic){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, tr("Invalid file magic!"));
        // Закроем файл.
        file.close();
        // Возврат.
        return false;
    }

    // Считаем версию формата.
    ds >> version;

    // Если версия некорректна.
    if(version != data_file_version){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, tr("Invalid file version!"));
        // Закроем файл.
        file.close();
        // Возврат.
        return false;
    }

    // Считаем число тел.
    ds >> count;

    // Если система симуляции не имеет в распоряжении такое количество.
    if(nbody->bodiesCount() < count){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, tr("Insufficient number of bodies. Required: %1.").arg(count));
        // Закроем файл.
        file.close();
        // Возврат.
        return false;
    }

    // Сообщим число тел в файле.
    log(Log::INFO, LOG_WHO, tr("Bodies in file: %1").arg(count));

    // Изменим размеры массивов под нужное число тел.
    mass.resize(count);
    pos.resize(count);
    vel.resize(count);

    // Если неудалось считать массы, позиции или векторы скоростей.
    if(ds.readRawData(reinterpret_cast<char*>(mass.data()), mass.size() * sizeof(float)) == -1 ||
       ds.readRawData(reinterpret_cast<char*>(pos.data()), pos.size() * sizeof(Point3f)) == -1 ||
       ds.readRawData(reinterpret_cast<char*>(vel.data()), vel.size() * sizeof(Point3f)) == -1){

        // Сообщим об этом.
        log(Log::WARNING, LOG_WHO, tr("Error reading data!"));
        // Закроем файл.
        file.close();
        // Возврат.
        return false;
    }

    // Закроем файл.
    file.close();

    // Если не удалось установить новые тела.
    if(!setBodies(0, mass, pos, vel)){
        // Сообщим об этом.
        log(Log::WARNING, LOG_WHO, tr("Error setting bodies!"));
        // Возврат.
        return false;
    }

    // Установим число взаимодействующих тел.
    nbody->setSimulatedBodiesCount(count);

    // Назначим обновление визуализации.
    update();

    // Сообщим об успешной загрузке данных.
    log(Log::INFO, LOG_WHO, tr("File loaded!"));

    // Возврат успешного результата.
    return true;
}

bool NBodyWidget::setBodies(size_t offset, const QVector<qreal> &masses, const QVector<QVector3D> &positions, const QVector<QVector3D> &velocities)
{
    if(!nbody->isReady()) return false;
    if(nbody->isRunning()) return false;

    bool has_glcontext = QGLContext::currentContext() != nullptr;

    if(!has_glcontext) makeCurrent();

    bool res = nbody->setMasses(masses, offset) &&
               nbody->setPositions(positions, offset) &&
               nbody->setVelocities(velocities, offset);

    if(!has_glcontext) doneCurrent();

    update();

    return res;
}

bool NBodyWidget::setBodies(size_t offset, const QVector<float> &masses, const QVector<Point3f> &positions, const QVector<Point3f> &velocities)
{
    if(!nbody->isReady()) return false;
    if(nbody->isRunning()) return false;

    bool has_glcontext = QGLContext::currentContext() != nullptr;

    if(!has_glcontext) makeCurrent();

    bool res = nbody->setMasses(masses, offset) &&
               nbody->setPositions(positions, offset) &&
               nbody->setVelocities(velocities, offset);

    if(!has_glcontext) doneCurrent();

    update();

    return res;
}

bool NBodyWidget::getBodies(size_t offset, size_t count, QVector<qreal> &masses, QVector<QVector3D> &positions, QVector<QVector3D> &velocities)
{
    if(!nbody->isReady()) return false;
    if(nbody->isRunning()) return false;

    bool has_glcontext = QGLContext::currentContext() != nullptr;

    if(!has_glcontext) makeCurrent();

    bool res = nbody->getMasses(masses, offset, count) &&
               nbody->getPositions(positions, offset, count) &&
               nbody->getVelocities(velocities, offset, count);

    if(!has_glcontext) doneCurrent();

    return res;
}

bool NBodyWidget::getBodies(size_t offset, size_t count, QVector<float> &masses, QVector<Point3f> &positions, QVector<Point3f> &velocities)
{
    if(!nbody->isReady()) return false;
    if(nbody->isRunning()) return false;

    bool has_glcontext = QGLContext::currentContext() != nullptr;

    if(!has_glcontext) makeCurrent();

    bool res = nbody->getMasses(masses, offset, count) &&
               nbody->getPositions(positions, offset, count) &&
               nbody->getVelocities(velocities, offset, count);

    if(!has_glcontext) doneCurrent();

    return res;
}

void NBodyWidget::setSimulationRunning(bool running)
{
    if(running){
        startSimulation();
    }else{
        stopSimulation();
    }
}

void NBodyWidget::startSimulation()
{
    if(sim_run == false){
        sim_run = nbody->simulate();
    }
}

void NBodyWidget::stopSimulation()
{
    sim_run = false;
    nbody->wait();
}

/**
 * @brief Пересоздание системы симуляции.
 * @return true в случае успеха, иначе false.
 */
bool NBodyWidget::recreateNBody()
{
    // Результат.
    bool res = true;

    // Установим симуляцию как не выполняющуюся.
    sim_run = false;

    // Доступность контекста OpenGL.
    bool has_glcontext = QGLContext::currentContext() != nullptr;

    // Если нет - сделаем текущим контекст, созданный QGLWidget.
    if(!has_glcontext) makeCurrent();

    // Установим шаг симуляции.
    nbody->setTimeStep(Settings::get().timeStep());

    try{
        // Получаем платформу и устройство OpenCL
        // из настроек.
        CLPlatform platform = CLPlatform::byName(Settings::get().clPlatformName());
        CLDevice device = platform.deviceByName(Settings::get().clDeviceName());

        // Уничтожаем возможно созданную ранее систему симуляции.
        nbody->destroy();

        // Если не удалось создать систему симуляции.
        if(!nbody->create(platform, device, Settings::get().bodiesCount())){
            // Сообщим об этом.
            log(Log::ERROR, LOG_WHO, tr("Error initializing NBody system"));
            // Результат - отрицательный.
            res = false;
        }
    }//Если где-то произошла ошибка.
    catch(CLException& e){
        // Сообщим об этом.
        log(Log::ERROR, LOG_WHO, e.what());
        // Результат - отрицательный.
        res = false;
    }

    // Если в начале функции небыло контекста OpenGL - уберём текущий контекст.
    if(!has_glcontext) doneCurrent();

    // Обозначим необходимость обновить визуализацию.
    update();

    // Оповестим всех об окончании визуализации, успешной или нет.
    emit nbodyStatusChanged();

    // Возвратим результат.
    return res;
}

/**
 * @brief Слот завершения симуляции.
 */
void NBodyWidget::on_simulationFinished()
{
    sim_time = std::chrono::duration_cast<std::chrono::duration<double>>(
                    std::chrono::high_resolution_clock::now() - sim_time_start);
    //qDebug() << "Simulation time:" << sim_time.count() * 1000.0 << "ms";
    // Поместить сообщение необходимости перерисовки области
    // просмотра в очередь событий приложения.
    update();
}

/**
 * @brief Функция настройки OpenGL.
 */
void NBodyWidget::initializeGL()
{
    // Имя расширения GL_ARB_point_sprite.
    static const char* gl_point_sprite_ext = "GL_ARB_point_sprite";
    // Имя расширения GL_ARB_point_parameters.
    static const char* gl_point_parameters_ext = "GL_ARB_point_parameters";

    // Сообщим об инициализации.
    log(Log::INFO, LOG_WHO, tr("Initializing OpenGL"));

    // Цвет очистки фона.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Запрет теста глубины.
    glDisable(GL_DEPTH_TEST);

    // Получим список расширений.
    QString glexts = QString::fromAscii(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));

    // Получим адреса нужных функций.
    init_gl_functions();

    // Если в списке расширений имеется
    // расширение GL_ARB_point_sprite.
    if(glexts.contains(gl_point_sprite_ext)){

        // Попытаемся загрузить текстуру звезды.
        sprite_texture = bindTexture(QImage(":/images/star.png"), GL_TEXTURE_2D, GL_RGBA);

        // Если удачно.
        if(sprite_texture != 0){
            // Обозначим наличие текстуры для звезды.
            has_point_sprite = true;
            // Освободим двумерную текстуру.
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }else{
        // Иначе сообщим о невозможности отрисовывать спрайты.
        log(Log::WARNING, LOG_WHO, tr("GL_ARB_point_sprite is not supported!"));
    }

    if(glexts.contains(gl_point_parameters_ext)){
        has_point_parameters = (glPointParameterfARB != nullptr) &&
                               (glPointParameterfvARB != nullptr);
    }else{
        // Иначе сообщим о невозможности отрисовывать спрайты.
        log(Log::WARNING, LOG_WHO, tr("GL_ARB_point_parameters is not supported!"));
    }

    // Если возможно рисовать спрайты.
    if(has_point_sprite){
        // Разрешим генерацию текстурных координат для точек.
        glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
        // Установим размер точек.
        glPointSize(7.5f);//7.5
        //glPointSize(10.0f);//7.5
    }else{
        // Иначе будем рисовать точки поменьше и без текстуры.
        glPointSize(2.5f);//2.5
        //glPointSize(3.5f);//2.5
    }

    if(has_point_parameters){

        float atten[3]={0.0f, 2e-3f, 1e-10f};

        float fade = 0.1f;

        float point_size_min, point_size_max;

        glGetFloatv(GL_POINT_SIZE_MIN_ARB, &point_size_min);
        glGetFloatv(GL_POINT_SIZE_MAX_ARB, &point_size_max);

        glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, fade < point_size_min ? point_size_min : fade);
        glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, point_size_max);
        glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, fade);
        glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, atten);

        log(Log::INFO, LOG_WHO, tr("Point size: %1 ... %2").arg(point_size_min).arg(point_size_max));
    }

    // Разрешение сглаживания точек.
    glEnable(GL_POINT_SMOOTH);

    // Переинициализируем систему симуляции.
    recreateNBody();
}

void NBodyWidget::resizeGL(int width, int height)
{
    log(Log::INFO, LOG_WHO, tr("Resizing: %1x%2").arg(width).arg(height));

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, static_cast<float>(width)/height, 1.0, 1e8);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Функция перерисовки.
 */
void NBodyWidget::paintGL()
{
    // Если нет данных для визуализации, либо невозможно её выполнить - возврат.
    if(!nbody->isReady() || nbody->isRunning()) return;

    // Очистим экран.
    glClear(GL_COLOR_BUFFER_BIT);

    // Установим матрицу модели.
    glMatrixMode(GL_MODELVIEW);
    // Сбросим матрицу.
    glLoadIdentity();
    // Установим позицию наблюдателя.
    gluLookAt(0.0f, 0.0f, view_position,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    // Матрица вращения.
    QMatrix4x4 rot_mat;
    // Получим матрицу вращения из кватерниона вращения.
    rot_mat.rotate(view_rotation);
    // Применим матрицу вращения.
    glMultMatrixd(rot_mat.data());

    // Если возможно текстурировать звёзды.
    if(has_point_sprite){
        // Разрешим смешивание.
        glEnable(GL_BLEND);
        // Параметры функции смешивания.
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Свяжем идентификатор текстуры с текущей двумерной текстурой.
        glBindTexture(GL_TEXTURE_2D, sprite_texture);
        // Разрешим двумерную текстуру.
        glEnable(GL_TEXTURE_2D);
        // Разрешим текстурирование точек.
        glEnable(GL_POINT_SPRITE);
    }

    // Установим буфер позиций.
    nbody->posBuffer()->bind();
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    // Установим индексный буфер.
    nbody->indexBuffer()->bind();

    // Отрисуем звзёды.
    glDrawElements(GL_POINTS, nbody->simulatedBodiesCount(), GL_UNSIGNED_INT, nullptr);

    // Сбросим установки индексного буфера.
    nbody->indexBuffer()->release();

    // Сбросим установки буфера позиций.
    glDisableClientState(GL_VERTEX_ARRAY);
    nbody->posBuffer()->release();

    // Если возможно текстурировать звёзды.
    if(has_point_sprite){
        // Запретим текстурирование точек.
        glDisable(GL_POINT_SPRITE);
        // Запретим двумерную текстуру.
        glDisable(GL_TEXTURE_2D);
        // Сбросим свяязь текущей двумерной текстуры.
        glBindTexture(GL_TEXTURE_2D, 0);
        // Запретим смешивание.
        glDisable(GL_BLEND);
    }

    // Если запущена непрерывная симуляция.
    if(sim_run){
        // Запустим вычисления.
        sim_run = nbody->simulate();
        // Если ошибка - пошлём сообщение.
        if(!sim_run) emit nbodyStatusChanged();
        // Время.
        sim_time_start = std::chrono::high_resolution_clock::now();
    }
}

qreal NBodyWidget::calcNewValueExp(qreal old_value, qreal step, qreal scale)
{
    qreal delta = step * scale;

    if(delta == 1.0 || delta == 0.0) return old_value;

    if(step > 0.0){
        return old_value * (1.0 + delta);
    }else{
        return old_value / (1.0 - delta);
    }
}

void NBodyWidget::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) != Qt::NoButton){
        qreal rot_x = event->y() - old_event_y;
        qreal rot_y = event->x() - old_event_x;

        QQuaternion q_rot_x = QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, rot_x);
        QQuaternion q_rot_y = QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, rot_y);

        view_rotation = q_rot_x * q_rot_y * view_rotation;

        if(!nbody->isRunning()) update();
    }

    old_event_x = event->x();
    old_event_y = event->y();
}

void NBodyWidget::wheelEvent(QWheelEvent *event)
{
    view_position = calcNewValueExp(view_position, - event->delta() / 8, 0.01);

    if(view_position < 1.0f) view_position = 1.0;

    //qDebug() << "z:" << view_position;

    if(!nbody->isRunning()) update();
}

bool NBodyWidget::init_gl_functions()
{
    static bool initialized = false;

    if(initialized) return true;

    const QGLContext* cxt = QGLContext::currentContext();

    if(cxt == nullptr) return false;

    glPointParameterfARB = reinterpret_cast<PFNGLPOINTPARAMETERFARBPROC>(cxt->getProcAddress("glPointParameterfARB"));
    glPointParameterfvARB = reinterpret_cast<PFNGLPOINTPARAMETERFVARBPROC>(cxt->getProcAddress("glPointParameterfvARB"));

    initialized = true;

    return true;
}
