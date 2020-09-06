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
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void qNormalizeAngle(int &angle);
    void set_background_color(double r, double g, double b);

signals:

private:
    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_vao;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    int m_projMatrixLoc;
    int m_mvMatrixLoc;
    int normalMatrixLoc;
    int m_lightPosLoc;

    int m_xRot, m_yRot, m_zRot;
    float m_scale;

    QPoint m_lastPos;

    std::vector<QOpenGLShaderProgram*> shaders;
    std::vector<Sphere> spheres;
    std::vector<Cylinder> cylinders;
public slots:
};

#endif // OPENGLWIDGET_H
