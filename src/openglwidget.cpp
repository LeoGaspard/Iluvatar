#include "src/openglwidget.h"

openGlWidget::openGlWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    m_xRot = 0;
    m_yRot = 0;
    m_zRot = 0;
    m_scale = 1.0f;

}

void openGlWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &openGlWidget::cleanup);

    glClearColor(0.2f,0.3f,0.3f,1.0f);


    shaders.push_back(new QOpenGLShaderProgram);
    shaders[0]->addShaderFromSourceFile(QOpenGLShader::Vertex,"shaders/shader.vs");
    shaders[0]->addShaderFromSourceFile(QOpenGLShader::Fragment,"shaders/shader.fs");
    shaders[0]->bindAttributeLocation("aPos",0);
    shaders[0]->bindAttributeLocation("aNorm",1);
    shaders[0]->link();
    shaders[0]->bind();
}

void openGlWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    for(unsigned int i=0;i<spheres.size();i++)
    {
        spheres[i].get_shader_program()->bind();
        spheres[i].draw(QVector3D(1.0f,1.0f,1.0f),QVector3D(0.0f,0.0f,2.0f));
    }
    for(unsigned int i=0; i<cylinders.size();i++)
    {
        cylinders[i].get_shader_program()->bind();
        cylinders[i].draw(QVector3D(1.0f,1.0f,1.0f),QVector3D(0.0f,0.0f,2.0f));
    }
    QMatrix4x4 model;
    model.rotate(m_xRot,QVector3D(1.0f,0.0f,0.0f));
    model.rotate(m_yRot,QVector3D(0.0f,1.0f,0.0f));
    model.rotate(m_zRot,QVector3D(0.0f,0.0f,1.0f));
    model.scale(QVector3D(m_scale,m_scale,m_scale));

    QMatrix4x4 view;
    view.translate(QVector3D(0.0f,0.0f,-3.0f));

    QMatrix4x4 projection;
    projection.perspective(90.0f, 4.0f/3.0f, 0.1f, 100.0f);

    int modelLoc = shaders[0]->uniformLocation("model");
    shaders[0]->setUniformValue(modelLoc, model);
    int viewLoc = shaders[0]->uniformLocation("view");
    shaders[0]->setUniformValue(viewLoc, view);
    int projectionLoc = shaders[0]->uniformLocation("projection");
    shaders[0]->setUniformValue(projectionLoc, projection);
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

   spheres.clear();
   cylinders.clear();

   for(unsigned int i=0; i<mol->get_n_atom(); i++)
   {
       Atom a = mol->get_atom(i);
       QVector3D R = QVector3D(a.get_x(), a.get_y(), a.get_z());
       QVector3D c = QVector3D(a.get_r(), a.get_g(), a.get_b());

       Sphere at(R,a.get_ionic_radius()*0.7f,c,45,45);
       spheres.push_back(at);
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
                cylinders.push_back(bo);
           }
       }
   }
   for(unsigned int i=0; i<cylinders.size(); i++)
   {
       cylinders[i].gen_pos();
       cylinders[i].render(shaders[0]);
   }
   for(unsigned int i=0; i<spheres.size();i++)
   {
       spheres[i].gen_pos();
       spheres[i].render(shaders[0]);
   }
   paintGL();
}

void openGlWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void openGlWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if(event->buttons() & Qt::LeftButton)
    {
        setXRotation(m_xRot + 2*dy);
        setYRotation(m_yRot + 2*dx);
    }
    if(event->buttons() & Qt::RightButton)
    {
        setXRotation(m_xRot + 2*dy);
        setZRotation(m_zRot + 2*dx);
    }
    m_lastPos = event->pos();
}


void openGlWidget::qNormalizeAngle(int &angle)
{
    while (angle < 0)
    {
        angle += 360*16;
    }
    while(angle > 360*16)
    {
        angle -= 360*16;
    }
}
void openGlWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if(angle != m_xRot)
    {
        m_xRot = angle;
        update();
    }
}
void openGlWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if(angle != m_yRot)
    {
        m_yRot = angle;
        update();
    }
}
void openGlWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if(angle != m_zRot)
    {
        m_zRot = angle;
        update();
    }
}




