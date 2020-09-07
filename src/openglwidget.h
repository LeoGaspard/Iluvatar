#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QtMath>
#include <QMouseEvent>
#include "src/sphere.h"
#include "src/cylinder.h"
#include "src/molecule.h"

class openGlWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit openGlWidget(QWidget *parent = nullptr);
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void cleanup();
    void draw_molecule(Molecule *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void set_background_color(double r, double g, double b);

signals:

private:
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_vao;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;

    float m_scale;

    QVector2D m_lastPos;
    QQuaternion m_rotation;
    std::vector<QOpenGLShaderProgram*> m_shaders;
    std::vector<Sphere> m_spheres;
    std::vector<Cylinder> m_cylinders;
public slots:
};

#endif // OPENGLWIDGET_H
