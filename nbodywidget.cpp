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



#define LOG_WHO "NBody View"


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
    connect(nbody, SIGNAL(simulationFinished()), this, SLOT(simulationFinished()));

    sim_run = false;

    has_point_sprite = false;
    sprite_texture = 0;

    view_distance = 5000.0f;
    rot_x = 0.0f;
    rot_y = 0.0f;
    old_rot_x = 0.0f;
    old_rot_y = 0.0f;
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

bool NBodyWidget::setBodies(size_t offset, const QVector<float> &masses, const QVector<Point3f> &positions, const QVector<Point3f> &velocities)
{
    if(!nbody->isReady()) return false;
    if(nbody->isRunning()) return false;

    bool has_glcontext = QGLContext::currentContext() != nullptr;

    bool res = nbody->setMasses(masses, offset) &&
               nbody->setPositions(positions, offset) &&
               nbody->setVelocities(velocities, offset);

    if(!has_glcontext) doneCurrent();

    update();

    return res;
}

void NBodyWidget::setSimulationRunning(bool running)
{
    sim_run = running;
    if(running) update();
}

bool NBodyWidget::recreateNBody()
{
    bool res = true;
    sim_run = false;

    bool has_glcontext = QGLContext::currentContext() != nullptr;

    if(!has_glcontext) makeCurrent();

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

    return res;
}

void NBodyWidget::simulationFinished()
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
    gluPerspective(45, static_cast<float>(width)/height, 1, 100000);
    glMatrixMode(GL_MODELVIEW);
}

void NBodyWidget::paintGL()
{
    if(!nbody->isReady()) return;
    if(nbody->isRunning()) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, view_distance,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    glDepthMask(GL_FALSE);

    if(has_point_sprite){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sprite_texture);
        glEnable(GL_POINT_SPRITE);
    }

    glPushMatrix();
    //glTranslatef(0.0, 0.0, -view_distance);
    //glRotatef(rot_x, 1.0, 0.0, 0.0);
    //glRotatef(rot_y, 0.0, 1.0, 0.0);
    QMatrix4x4 rot_mat;
    rot_mat.rotate(rot_quat);
    glMultMatrixd(rot_mat.data());

    nbody->posBuffer()->bind();
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    nbody->indexBuffer()->bind();
    glDrawElements(GL_POINTS, nbody->bodiesCount(), GL_UNSIGNED_INT, nullptr);

    nbody->indexBuffer()->release();

    glDisableClientState(GL_VERTEX_ARRAY);
    nbody->posBuffer()->release();

    glPopMatrix();

    if(has_point_sprite){
        glDisable(GL_POINT_SPRITE);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }

    glDepthMask(GL_TRUE);

    if(sim_run) nbody->simulate(5e5);//1e-3);
}

void NBodyWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() != Qt::NoButton){
        rot_x = event->y() - old_rot_x;
        rot_y = event->x() - old_rot_y;

        QQuaternion q_rot_x = QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, rot_x);
        QQuaternion q_rot_y = QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, rot_y);

        rot_quat = q_rot_x * q_rot_y * rot_quat;

        if(!nbody->isRunning()) update();
    }
    old_rot_x = event->y();
    old_rot_y = event->x();
}

void NBodyWidget::wheelEvent(QWheelEvent *event)
{
    view_distance -= event->delta() / 4;

    if(view_distance < 1.0f) view_distance = 1.0f;

    if(!nbody->isRunning()) update();
}
