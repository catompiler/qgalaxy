#ifndef NBODYWIDGET_H
#define NBODYWIDGET_H

#include <QGLWidget>

class NBody;



class NBodyWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit NBodyWidget(QWidget *parent = 0);
    ~NBodyWidget();
signals:

public slots:
    bool recreateNBody();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

private:
    NBody* nbody;
};

#endif // NBODYWIDGET_H
