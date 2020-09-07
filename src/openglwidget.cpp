#include "src/openglwidget.h"

openGlWidget::openGlWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    m_scale = 1.0f;

}

void openGlWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &openGlWidget::cleanup);

    glClearColor(0.2f,0.3f,0.3f,1.0f);


    m_shaders.push_back(new QOpenGLShaderProgram);
    m_shaders[0]->addShaderFromSourceFile(QOpenGLShader::Vertex,"shaders/shader.vs");
    m_shaders[0]->addShaderFromSourceFile(QOpenGLShader::Fragment,"shaders/shader.fs");
    m_shaders[0]->bindAttributeLocation("aPos",0);
    m_shaders[0]->bindAttributeLocation("aNorm",1);
    m_shaders[0]->link();
    m_shaders[0]->bind();
}

void openGlWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    for(unsigned int i=0;i<m_spheres.size();i++)
    {
        m_spheres[i].get_shader_program()->bind();
        m_spheres[i].draw(QVector3D(1.0f,1.0f,1.0f),QVector3D(0.0f,0.0f,2.0f));
    }
    for(unsigned int i=0; i<m_cylinders.size();i++)
    {
        m_cylinders[i].get_shader_program()->bind();
        m_cylinders[i].draw(QVector3D(1.0f,1.0f,1.0f),QVector3D(0.0f,0.0f,2.0f));
    }
    QMatrix4x4 model;
    model.rotate(m_rotation);
    model.scale(QVector3D(m_scale,m_scale,m_scale));

    QMatrix4x4 view;
    view.translate(QVector3D(0.0f,0.0f,-3.0f));

    QMatrix4x4 projection;
    projection.perspective(90.0f, 4.0f/3.0f, 0.1f, 100.0f);

    int modelLoc = m_shaders[0]->uniformLocation("model");
    m_shaders[0]->setUniformValue(modelLoc, model);
    int viewLoc = m_shaders[0]->uniformLocation("view");
    m_shaders[0]->setUniformValue(viewLoc, view);
    int projectionLoc = m_shaders[0]->uniformLocation("projection");
    m_shaders[0]->setUniformValue(projectionLoc, projection);
}

void openGlWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScalef(height *1.f/width, 1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
}

void openGlWidget::cleanup()
{
    makeCurrent();
    doneCurrent();
}

void openGlWidget::wheelEvent(QWheelEvent *event)
{
    m_scale += event->angleDelta().y()*0.0005f;
    if(m_scale < 0.001f)
    {
        m_scale = 0.001f;
    }
    update();
}

void openGlWidget::set_background_color(double r, double g, double b)
{
    makeCurrent();
    glClearColor(static_cast<float>(r),static_cast<float>(g),static_cast<float>(b),1.0f);
    update();
}

void openGlWidget::draw_molecule(Molecule *mol)
{
   makeCurrent();
   mol->compute_adjacency_matrix();
   mol->center();

   m_spheres.clear();
   m_cylinders.clear();

   for(unsigned int i=0; i<mol->get_n_atom(); i++)
   {
       Atom a = mol->get_atom(i);
       QVector3D R = QVector3D(a.get_x(), a.get_y(), a.get_z());
       QVector3D c = QVector3D(a.get_r(), a.get_g(), a.get_b());

       Sphere at(R,a.get_ionic_radius()*0.7f,c,45,45);
       m_spheres.push_back(at);
   }
   for(unsigned int i=0;i<mol->get_n_atom()-1;i++)
   {
       for(unsigned int j=i+1; j<mol->get_n_atom();j++)
       {
           if(mol->get_adjacency(i,j)==1)
           {
                Atom a = mol->get_atom(i);
                Atom b = mol->get_atom(j);

                QVector3D R1 = QVector3D(a.get_x(), a.get_y(), a.get_z());
                QVector3D R2 = QVector3D(b.get_x(), b.get_y(), b.get_z());

                Cylinder bo(R1,R2,0.3f,QVector3D(0.3f,0.3f,0.3f),15);
                m_cylinders.push_back(bo);
           }
       }
   }
   for(unsigned int i=0; i<m_cylinders.size(); i++)
   {
       m_cylinders[i].gen_pos();
       m_cylinders[i].render(m_shaders[0]);
   }
   for(unsigned int i=0; i<m_spheres.size();i++)
   {
       m_spheres[i].gen_pos();
       m_spheres[i].render(m_shaders[0]);
   }
   paintGL();
}

void openGlWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = QVector2D(event->localPos());
}

void openGlWidget::mouseMoveEvent(QMouseEvent *event)
{
    QVector2D diff = QVector2D(event->localPos()) - m_lastPos;
    QVector3D n = QVector3D(diff.y(),diff.x(),0.0f).normalized();
    float acc = diff.length()/2.0f;
    QVector3D rotationAxis = n;
    m_rotation = QQuaternion::fromAxisAndAngle(rotationAxis,acc)*m_rotation;
    m_lastPos = QVector2D(event->localPos());
    update();
}
