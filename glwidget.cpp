#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <math.h>

GLWidget::GLWidget(QWidget *parent):QOpenGLWidget(parent),
    shader(0),
    VBO(QOpenGLBuffer::VertexBuffer),
    EBO(QOpenGLBuffer::IndexBuffer),
    rot_angle_(0,0,0),
    set_texture_ok_(false),
    texture_yx_ratio_(1)
{
    grid_mesh_ = GridMesh(QVector2D(-1.0, -1.0), QVector2D(1.0, 1.0), 512);
    vertex_shader_fn_ = QDir::currentPath() + "/default.vert";
    frag_shader_fn_ = QDir::currentPath() + "/default.frag";

    texture = 0;
    camera = QCamera(QVector3D(0, 0, 3));
    model.setToIdentity();
    qDebug()<< "shader path:" << vertex_shader_fn_;
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(512,512);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void GLWidget::SetTexture(QString texture_fn)
{
    QImage img = QImage(texture_fn);
    img = img.convertToFormat(QImage::Format_RGBA8888, Qt::AutoColor);
    texture = new QOpenGLTexture(img);

    cv::Mat grey_image = cv::imread(texture_fn.toStdString(), cv::IMREAD_GRAYSCALE);
    this->grid_mesh_.InitImage(grey_image);
    cv::imshow("GLWidget::settexture",grey_image);

    this->SetupVertexAttribs();  // 载入纹理后需要更新VBO

    texture_yx_ratio_ = 1.0 * grey_image.rows / grey_image.cols;
    set_texture_ok_ = true;
    qDebug() << "set texture ok\n";
}

void GLWidget::SaveMesh(QString mesh_fn)
{
    grid_mesh_.SaveMeshToFile(mesh_fn);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    projection.setToIdentity();
    projection.perspective(camera.Zoom, GLfloat(w)/h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    last_mouse_pos_ = event->pos();
}
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        QPoint dis = event->pos() - last_mouse_pos_;
        QMatrix4x4 mouse_matrix;
        mouse_matrix.setToIdentity();
        mouse_matrix.rotate(rot_angle_.x(), 1,0,0);
        mouse_matrix.rotate(rot_angle_.y(), 0,1,0);
        mouse_matrix.rotate(rot_angle_.z(), 0,0,1);
        QVector4D x_vector(1,0,0,0);
        QVector4D y_vector(0,1,0,0);
        QVector4D z_vector(0,0,1,0);
        x_vector = mouse_matrix * x_vector;
        y_vector = mouse_matrix * y_vector;
        z_vector = mouse_matrix * z_vector;

        rot_angle_.setY(
                    rot_angle_.y() + (y_vector.y() / abs(y_vector.y())) * dis.x() / 5.0
                    );
        rot_angle_.setX(
                    rot_angle_.x() + dis.y()/5.0
                    );
    }
    last_mouse_pos_ = event->pos();
    update();
}

void GLWidget::cleanup()
{
    makeCurrent();
    this->VBO.destroy();
    delete this->shader;
    delete this->texture;
    this->shader = 0;
    doneCurrent();
}

void GLWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

    glEnable(GL_DEPTH_TEST);
    shader = new QOpenGLShaderProgram;
    if(!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex_shader_fn_))
    {
        std::cerr << "unable to compile vertex shader: ";
        std::cerr << vertex_shader_fn_.toStdString() << std::endl;
    }
    if(!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, frag_shader_fn_))
    {
        std::cerr <<"unable to compile fragment shader: ";
        std::cerr << frag_shader_fn_.toStdString() << std::endl;
    }

    if(!shader->link()){
        std::cerr << "unable to link shader.\n";
    }

    //SetTexture(QDir::currentPath() + "/grey3.jpg");

    shader->bind();
    this->model_mat_loc_ = shader->uniformLocation("model");
    this->view_mat_loc_ = shader->uniformLocation("view");
    this->projection_mat_loc_ = shader->uniformLocation("projection");

    shader->setUniformValue("grey_image", 0);

    SetupVertexAttribs();
}

void GLWidget::SetupVertexAttribs()
{
    this->VAO.create();
    QOpenGLVertexArrayObject::Binder VAOBinder(&this->VAO);
    this->VBO.create();
    this->EBO.create();

    VBO.bind();
    VBO.allocate(this->grid_mesh_.Data(), this->grid_mesh_.GetVertexNum()*sizeof(Vertex));
    EBO.bind();
    EBO.allocate(this->grid_mesh_.Index(), this->grid_mesh_.GetIndexNum()*sizeof(int));

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // position
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(0));
    // normal
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    // texcoord
    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, texcoord)));
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->VAO.bind();
    model.setToIdentity();
    model.scale(1, texture_yx_ratio_, 1);
    model.rotate(rot_angle_.x(), 1, 0, 0);
    model.rotate(rot_angle_.y(), 0, 1, 0);
    model.rotate(rot_angle_.z(), 0, 0, 1);
    view = camera.GetViewMatrix();

    this->shader->bind();

    shader->setUniformValue(model_mat_loc_, model);
    shader->setUniformValue(view_mat_loc_, view);
    shader->setUniformValue(projection_mat_loc_, projection);
    if(set_texture_ok_)
        texture->bind();

    glDrawElements(GL_TRIANGLES, this->grid_mesh_.GetIndexNum(), GL_UNSIGNED_INT, 0);
    shader->release();
}

bool GLWidget::Empty()
{
    qDebug() << "mesh vertex num:" << grid_mesh_.GetVertexNum();
    if(grid_mesh_.origin_.data == NULL)
        return true;
    return false;
}
