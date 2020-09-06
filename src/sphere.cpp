#include "src/sphere.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QtMath>
#include <QMatrix4x4>
#include <iostream>

Sphere::Sphere(QVector3D R,float radius, QVector3D color, unsigned int stack,unsigned int slice)
{
    center = R;
    RGB = color;
    r = radius;
    stacks = stack;
    slices = slice;
}

Sphere::Sphere(const Sphere &sp)
{
    center = sp.center;
    RGB = sp.RGB;
    r = sp.r;
    stacks = sp.stacks;
    slices = sp.slices;
}


// Creates the Vertex Array Object, Vertex Buffer Object, Element Buffer Object
// Links them to the correct attributes
void Sphere::render(QOpenGLShaderProgram* program)
{
    m_program = program;
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_posvbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_posvbo.create();
    m_posvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_normvbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_normvbo.create();
    m_normvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_ebo.create();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);

    m_posvbo.bind();
    m_posvbo.allocate(pos.data(),static_cast<int>(pos.size()*3*sizeof(float)));
    f->glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),nullptr);

    m_normvbo.bind();
    m_normvbo.allocate(normal.data(),static_cast<int>(normal.size()*3*sizeof(float)));
    f->glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, sizeof(QVector3D),nullptr);

    m_ebo.bind();
    m_ebo.allocate(&indices[0],static_cast<int>(indices.size()*sizeof(unsigned int)));

    m_vao.release();

}

//Draws the elements on the openGL current window
void Sphere::draw(QVector3D lightColor, QVector3D lightPos)
{
    int coLoc = m_program->uniformLocation("objectColor");
    m_program->setUniformValue(coLoc, RGB[0],RGB[1],RGB[2]);
    int lightCPos = m_program->uniformLocation("lightColor");
    m_program->setUniformValue(lightCPos,lightColor[0],lightColor[1],lightColor[2]);
    int lightPPos = m_program->uniformLocation("lightPos");
    m_program->setUniformValue(lightPPos,lightPos[0],lightPos[1],lightPos[2]);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glDrawElements(GL_TRIANGLES,static_cast<int>(indices.size()),GL_UNSIGNED_INT, nullptr);
}

//Creates the x y and z coordinates for the sphere vertices
// x = r sin Phi cos Theta
// y = r sin Phi sin Theta
// z = r cod Theta
//
// Phi is divided in stacks and Theta in slices
//
// Creates the indexed list for the EBO
//
// Creates the normal to each vertex as a mean of the normal of
// adjacent triangles
void Sphere::gen_pos()
{
    pos.clear();
    indices.clear();
    normal.clear();
    double pi=3.1415926;
    double ststep=pi/stacks;
    double slstep=2*pi/slices;
    double stangle, slangle;
    double px,py,pz;

    unsigned int k1, k2;

    for(unsigned int i=0; i<=stacks; i++)
    {
        stangle = pi/2 - i*ststep;
        pz = static_cast<double>(r)*sin(stangle);

        for(unsigned int j=0; j<=slices; j++)
        {
            QVector3D v;
            slangle = j*slstep;
            px = static_cast<double>(r)*cos(stangle)*cos(slangle);
            py = static_cast<double>(r)*cos(stangle)*sin(slangle);
            v[0] = static_cast<float>(px)+center[0];
            v[1] = static_cast<float>(py)+center[1];
            v[2] = static_cast<float>(pz)+center[2];
            pos.push_back(v);
        }
    }

    for(unsigned int i=0; i<stacks;++i)
    {
        k1 = i*(slices+1);
        k2 = k1 + slices + 1;
        for(unsigned int j=0; j<slices; ++j, ++k1, ++k2)
        {
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1+1);
            }
            if(i != (stacks-1))
            {
                indices.push_back(k1+1);
                indices.push_back(k2);
                indices.push_back(k2+1);
            }
        }

    }

    normal.resize(pos.size());
    for(unsigned long i=0; i<indices.size();i+=3)
    {
        QVector3D v1 = pos[indices[i]];
        QVector3D v2 = pos[indices[i+1]];
        QVector3D v3 = pos[indices[i+2]];
        QVector3D e1 = v2-v1;
        QVector3D e2 = v3-v1;
        normal[indices[i]] += QVector3D::normal(e1,e2);
        normal[indices[i+1]] += QVector3D::normal(e1,e2);
        normal[indices[i+2]] += QVector3D::normal(e1,e2);
    }

}

void Sphere::change_radius(float dradius)
{
    r += dradius;
    if(r<0.01f)
    {
        r=0.01f;
    }
    this->gen_pos();
    this->render(m_program);
}
