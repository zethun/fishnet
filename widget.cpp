#include "widget.h"

Widget::Widget(QGLWidget *parent)
    : QGLWidget(parent),
      activeNode(NULL), x(-5), y(-5), rate(0)
{
    setWindowTitle(QString("Fishnet"));
    setGeometry(100, 100, 800, 600);
}

void Widget::initializeGL()
{
    glClearColor(0,0,0,1);
    glEnable(GL_LINE_SMOOTH);

    connect(&timeUpdate, SIGNAL(timeout()), SLOT(update()));
    connect(&rateTimer, SIGNAL(timeout()), SLOT(logoutRefreshRate()));
    timeUpdate.setInterval(10);
    timeUpdate.start();
    rateTimer.setInterval(1000);
    rateTimer.start();
}

void Widget::paintGL()
{
    rate += 1;
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    net.paint();
    glLoadIdentity();

    glFlush();
}

void Widget::resizeGL(int w, int h)
{
    aspect = w / (double)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if ( aspect <= 1 ) {
        glOrtho(-200.0, 200.0, -200/aspect, 200/aspect, 1.0, -1.0);
        scale = (double)400 / w;
    } else {
        glOrtho(-200*aspect, 200*aspect, -200.0, 200.0, 1.0, -1.0);
        scale = (double)400 / h;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Widget::mousePressEvent(QMouseEvent* event)
{
    Point point = convertWidgetCoordinates(event->x(), event->y());
    activeNode = net.getNodeAtPoint(point.x, point.y);
//    timeUpdate.stop();
    if (activeNode != NULL) activeNode->setXY(point.x, point.y);
//    timeUpdate.start();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (activeNode != NULL)
    {
        timer.restart();
        lastPoint = convertWidgetCoordinates(event->x(), event->y());
        activeNode->setXY(lastPoint.x, lastPoint.y);

        updateGL();
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    int timeElapsed = timer.elapsed();
    Vector speed((event->x()-lastPoint.x)/(timeElapsed*20),
                 (event->y()-lastPoint.y)/(timeElapsed*20));
    if (activeNode != NULL) activeNode->release(speed);
    activeNode = NULL;
}

Point Widget::convertWidgetCoordinates(double x, double y)
{
    Point point;

    if (aspect > 1) {
        point.x = x * scale - 200 * aspect;
        point.y = 200 - y * scale;
    } else {
        point.x = x * scale - 200;
        point.y = 200 * aspect - y * scale;
    }

    return point;
}

void Widget::logoutRefreshRate()
{
    rateTimer.stop();
    qDebug() << rate;
    rate = 0;
    rateTimer.start();
}

