#ifndef cylinder_h_included
#define cylinder_h_included

#include <vector>
#include <iostream>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QtMath>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class Cylinder
{
    public:
        Cylinder(QVector3D pos1, QVector3D pos2, float r, QVector3D color, int slice);
        Cylinder(const Cylinder &cy);
        void unit_circle();
        void render(QOpenGLShaderProgram*);
        void draw(QVector3D lightColor, QVector3D lightPos);
//		void change_radius(float dradius);
//		void change_height(float dheight);
        void gen_pos();
        QOpenGLShaderProgram* get_shader_program() {return m_program;}
    private:
        QVector3D posA, posB, RGB;
        unsigned int VAO,posBuffer,normBuffer, EBO;
        std::vector<QVector3D> pos, unit, normal;
        std::vector<unsigned int> indices;
        int slices;
        float height, radius;

        QOpenGLShaderProgram *m_program;
        QOpenGLVertexArrayObject m_vao;
        QOpenGLBuffer m_posvbo, m_normvbo, m_ebo;

};

#endif
