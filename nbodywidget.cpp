#include "nbodywidget.h"
#include "log.h"
#include <QGLFormat>

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
}

NBodyWidget::~NBodyWidget()
{
}

void NBodyWidget::initializeGL()
{
    log(Log::INFO, LOG_WHO, tr("Initializing OpenGL"));
}

void NBodyWidget::resizeGL(int width, int height)
{
    log(Log::INFO, LOG_WHO, tr("Resizing: %1x%2").arg(width).arg(height));
}

void NBodyWidget::paintGL()
{

}
