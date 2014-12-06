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


#define LOG_WHO "NBody View"


#define VIEW_DISTANCE_DEFAULT 5000.0f


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
    connect(nbody, SIGNAL(simulationFinished()), this, SLOT(on_simulationFinished()));
    connect(nbody, SIGNAL(simulationFinished()), this, SIGNAL(simulationFinished()));

    sim_run = false;

    has_point_sprite = false;
    sprite_texture = 0;

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

void NBodyWidget::setTimeStep(float dt)
{
    nbody->setTimeStep(dt);
}

bool NBodyWidget::saveNBody(const QString &filename)
{
    if(!nbody->isReady()) return false;
    if(nbody->isRunning()) return false;

    log(Log::INFO, LOG_WHO, tr("Saving file: %1").arg(filename));

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)){
        log(Log::ERROR, LOG_WHO, tr("Error open file!"));
        return false;
    }

    size_t count = nbody->simulatedBodiesCount();

    QVector<float> mass;
    QVector<Point3f> pos, vel;

    if(!getBodies(0, count, mass, pos, vel)){
        log(Log::WARNING, LOG_WHO, tr("Error getting bodies!"));
        file.close();
        return false;
    }

    QDataStream ds(&file);
    ds.setVersion(QDataStream::Qt_4_8);

    ds << data_file_magic << data_file_version << static_cast<unsigned int>(count);

    if(ds.writeRawData(reinterpret_cast<char*>(mass.data()), mass.size() * sizeof(float)) == -1 ||
       ds.writeRawData(reinterpret_cast<char*>(pos.data()), pos.size() * sizeof(Point3f)) == -1 ||
       ds.writeRawData(reinterpret_cast<char*>(vel.data()), vel.size() * sizeof(Point3f)) == -1){

        log(Log::WARNING, LOG_WHO, tr("Error writing data!"));
        file.close();
        return false;
    }

    file.close();

    log(Log::INFO, LOG_WHO, tr("File saved!"));

    return true;
}

bool NBodyWidget::openNBody(const QString &filename)
{
    if(!nbody->isReady()) return false;
    if(nbody->isRunning()) return false;

    log(Log::INFO, LOG_WHO, tr("Opening file: %1").arg(filename));

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        log(Log::ERROR, LOG_WHO, tr("Error open file!"));
        return false;
    }

    QVector<float> mass;
    QVector<Point3f> pos, vel;

    quint32 magic, version;
    unsigned int count;

    QDataStream ds(&file);
    ds.setVersion(QDataStream::Qt_4_8);

    ds >> magic;

    if(magic != data_file_magic){
        log(Log::ERROR, LOG_WHO, tr("Invalid file magic!"));
        file.close();
        return false;
    }

    ds >> version;

    if(version != data_file_version){
        log(Log::ERROR, LOG_WHO, tr("Invalid file version!"));
        file.close();
        return false;
    }

    ds >> count;

    if(nbody->bodiesCount() < count){
        log(Log::ERROR, LOG_WHO, tr("Insufficient number of bodies. Required: %1.").arg(count));
        file.close();
        return false;
    }

    log(Log::INFO, LOG_WHO, tr("Bodies in file: %1").arg(count));

    mass.resize(count);
    pos.resize(count);
    vel.resize(count);

    if(ds.readRawData(reinterpret_cast<char*>(mass.data()), mass.size() * sizeof(float)) == -1 ||
       ds.readRawData(reinterpret_cast<char*>(pos.data()), pos.size() * sizeof(Point3f)) == -1 ||
       ds.readRawData(reinterpret_cast<char*>(vel.data()), vel.size() * sizeof(Point3f)) == -1){

        log(Log::WARNING, LOG_WHO, tr("Error reading data!"));
        file.close();
        return false;
    }

    file.close();

    if(!setBodies(0, mass, pos, vel)){
        log(Log::WARNING, LOG_WHO, tr("Error setting bodies!"));
        return false;
    }

    nbody->setSimulatedBodiesCount(count);

    update();

    log(Log::INFO, LOG_WHO, tr("File opened!"));

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

bool NBodyWidget::recreateNBody()
{
    bool res = true;
    sim_run = false;

    bool has_glcontext = QGLContext::currentContext() != nullptr;

    if(!has_glcontext) makeCurrent();

    nbody->setTimeStep(Settings::get().timeStep());

    try{
        CLPlatform platform = CLPlatform::byName(Settings::get().clPlatformName());
        CLDevice device = platform.deviceByName(Settings::get().clDeviceName());

        nbody->destroy();
        if(!nbody->create(platform, device, Settings::get().bodiesCount())){
            log(Log::ERROR, LOG_WHO, tr("Error initializing NBody system"));
            res = false;
        }
    }catch(CLException& e){
        log(Log::ERROR, LOG_WHO, e.what());
        res = false;
    }

    if(!has_glcontext) doneCurrent();

    update();

    emit nbodyInitialized();

    return res;
}

void NBodyWidget::on_simulationFinished()
{
    update();
}

void NBodyWidget::initializeGL()
{
    static const char* gl_point_sprite_ext = "GL_ARB_point_sprite";

    log(Log::INFO, LOG_WHO, tr("Initializing OpenGL"));

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepth(1.0f);

    QString glexts = QString::fromAscii(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));

    if(glexts.contains(gl_point_sprite_ext)){

        sprite_texture = bindTexture(QImage(":/images/star.png"), GL_TEXTURE_2D, GL_RGBA);

        if(sprite_texture != 0){

            has_point_sprite = true;
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }else{
        log(Log::WARNING, LOG_WHO, tr("GL_ARB_point_sprite is not supported!"));
    }

    if(has_point_sprite){
        glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
        glPointSize(7.5f);
    }else{
        glPointSize(2.5f);
    }

    glEnable(GL_POINT_SMOOTH);

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

void NBodyWidget::paintGL()
{
    if(!nbody->isReady()) return;
    if(nbody->isRunning()) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0.0f, 0.0f, view_position,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    glDepthMask(GL_FALSE);

    if(has_point_sprite){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, sprite_texture);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_POINT_SPRITE);
    }

    QMatrix4x4 rot_mat;
    rot_mat.rotate(view_rotation);
    glMultMatrixd(rot_mat.data());

    nbody->posBuffer()->bind();
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    nbody->indexBuffer()->bind();
    glDrawElements(GL_POINTS, nbody->simulatedBodiesCount(), GL_UNSIGNED_INT, nullptr);

    nbody->indexBuffer()->release();

    glDisableClientState(GL_VERTEX_ARRAY);
    nbody->posBuffer()->release();

    if(has_point_sprite){
        glDisable(GL_POINT_SPRITE);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
    }

    glDepthMask(GL_TRUE);

    if(sim_run) sim_run = nbody->simulate();
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

    if(!nbody->isRunning()) update();
}
