#ifndef NBODYWIDGET_H
#define NBODYWIDGET_H

#include <QGLWidget>

class NBodyWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit NBodyWidget(QWidget *parent = 0);
    ~NBodyWidget();
signals:

public slots:

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
};

#endif // NBODYWIDGET_H
