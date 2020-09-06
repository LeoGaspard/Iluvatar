#ifndef sphere_h_included
#define sphere_h_included

#include <vector>
#include <iostream>
#include <QVector3D>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class Sphere
{
    public:
        Sphere(QVector3D R,float radius, QVector3D color, unsigned int stack,unsigned int slice);
        Sphere(const Sphere &sp);
        void render(QOpenGLShaderProgram*);
        void draw(QVector3D lightColor, QVector3D lightPos);
        void gen_pos();
        void change_radius(float dradius);
        QOpenGLShaderProgram* get_shader_program() {return m_program;}
    private:
        QVector3D center, RGB;
        unsigned int VAO,posBuffer, normBuffer, EBO;
        float r;
        std::vector<QVector3D> pos, normal;
        std::vector<unsigned int> indices;
        unsigned int stacks, slices;
        QOpenGLShaderProgram *m_program;
        QOpenGLVertexArrayObject m_vao;
        QOpenGLBuffer m_posvbo,m_normvbo,m_ebo;
};

#endif
