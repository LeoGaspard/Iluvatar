#include "src/cylinder.h"

Cylinder::Cylinder(QVector3D pos1, QVector3D pos2, float r, QVector3D color, int slice)
{
    posA = pos1;
    posB = pos2;
    RGB = color;
    radius = r;
    slices = slice;
    height = pos1.distanceToPoint(pos2);
}

Cylinder::Cylinder(const Cylinder &cy)
{
    posA = cy.posA;
    posB = cy.posB;
    RGB = cy.RGB;
    slices = cy.slices;
    radius = cy.radius;
    height = cy.height;
}

// Creates the unit circel that defines the cylinder
//  x = r cos Theta
//  y = r sin Theta
//
//  Theta divided into slices
void Cylinder::unit_circle()
{
    unit.clear();
    float pi=3.1415926f;
    float step=2*pi/slices;
    float angle;

    for(int i=0; i<slices; i++)
    {
        QVector3D v;
        angle = i*step;
        v[0] = radius*static_cast<float>(cos(static_cast<double>(angle)));
        v[1] = radius*static_cast<float>(sin(static_cast<double>(angle)));
        v[2] = 0;
        unit.push_back(v);
    }
}

//Creates the x y and z coordinates for the cylinder vertices
// x and y from unit_circle
// z = 0 or height
//
// Rotates and translate the cylinder to match the center of each face
//
// Create the indexed list for the EBO
//
// Creates the normal to each vertex as a mean of the normal of
// adjacent triangles
void Cylinder::gen_pos()
{
    pos.clear();
    indices.clear();
    unit.clear();
    normal.clear();

    this->unit_circle();

    //Builds the coordinates of the center of the base face
    QVector3D v;
    v = {0.0f,0.0f,0.0f};
    pos.push_back(v);

    //Builds the coordinates of the center of the top face
    v = {0.0f,0.0f,height};
    pos.push_back(v);

    //Builds the coordinates of the side faces
    for(unsigned long i=0; i<static_cast<unsigned long>(slices); i++)
    {
        v = {unit[i].x()*radius,unit[i].y()*radius,0.0f};
        pos.push_back(v);
        v = {unit[i].x()*radius,unit[i].y()*radius, height};
        pos.push_back(v);
    }


    //Translation matrix to put the base of the cylinder at point A
    QMatrix4x4 trsmat;
    trsmat.translate(posA);

    QVector3D cylH = pos[0]-pos[1]; //The vector of the height of the cylinder
    cylH.normalize();
    QVector3D AB = posA-posB; //The vector between points A and B
    AB.normalize();


    QVector3D rotVec = QVector3D::crossProduct(AB,cylH); // A vector normal to both cylH and AB
    rotVec.normalize();

    float angle = -static_cast<float>(acos(static_cast<double>(QVector3D::dotProduct(AB,cylH))));

    QMatrix4x4 rotmat;
    rotmat.rotate(qRadiansToDegrees(angle),rotVec);
    for(unsigned long i=0;i<pos.size();i++)
    {
        pos[i] = QVector3D(trsmat*rotmat*QVector4D(pos[i],1.0f));
    }


    //Triangles on the sides
    for(unsigned int i=2;i<2*static_cast<unsigned int>(slices);i+=2)
    {
        indices.push_back(i);
        indices.push_back(i+1);
        indices.push_back(i+2);
        indices.push_back(i+1);
        indices.push_back(i+2);
        indices.push_back(i+3);
    }
    //Closing the cylinder
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(static_cast<unsigned int>(pos.size()) -2 );
    indices.push_back(static_cast<unsigned int>(pos.size()) -2 );
    indices.push_back(static_cast<unsigned int>(pos.size()) -1);
    indices.push_back(3);

    //Triangle at the bottom and top
    for(unsigned int i=2;i<2*static_cast<unsigned int>(slices);i+=2)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i+2);
        indices.push_back(1);
        indices.push_back(i+1);
        indices.push_back(i+3);
    }
    //Closing the faces
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(static_cast<unsigned int>(pos.size()) -2);
    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(static_cast<unsigned int>(pos.size()) -1);



    std::vector<QVector3D> trnorm;
    normal.resize(pos.size());

    for(unsigned long i=0; i<indices.size(); i+=3)
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

// Creates the Vertex Array Object, Vertex Buffer Object, Element Buffer Object
// Links them to the correct attributes
void Cylinder::render(QOpenGLShaderProgram* program)
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
void Cylinder::draw(QVector3D lightColor, QVector3D lightPos)
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
