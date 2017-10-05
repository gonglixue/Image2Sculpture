#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <math.h>

GLWidget::GLWidget(QWidget *parent):QOpenGLWidget(parent),
    shader(0),
    VBO(QOpenGLBuffer::VertexBuffer),
    EBO(QOpenGLBuffer::IndexBuffer),
    rot_angle_(0,0,0),
    light_rot_angle_(0, 0, 0),
    set_texture_ok_(false),
    texture_yx_ratio_(1),
    active_texture_(false)
{
    grid_mesh_ = GridMesh(QVector2D(-1.0, -1.0), QVector2D(1.0, 1.0), 512);
    vertex_shader_fn_ = QDir::currentPath() + "/default.vert";
    frag_shader_fn_ = QDir::currentPath() + "/default.frag";
    light_pos_ = QVector3D(0, 0, 2);
    texture = 0;
    camera = QCamera(QVector3D(0, 0, 5));
    model.setToIdentity();

    image_widget_ = new CVImageWidget();
    qDebug()<< "shader path:" << vertex_shader_fn_;
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(800,800);
}

QSize GLWidget::sizeHint() const
{
    return QSize(800, 800);
}

void GLWidget::SetTexture(QString texture_fn)
{
    QImage img = QImage(texture_fn);
    img = img.convertToFormat(QImage::Format_RGBA8888, Qt::AutoColor);
    texture = new QOpenGLTexture(img);

    cv::Mat grey_image = cv::imread(texture_fn.toStdString(), cv::IMREAD_GRAYSCALE);
    this->grid_mesh_.InitImage(grey_image);
    //cv::imshow("origin",grey_image);

    this->SetupVertexAttribs();  // 载入纹理后需要更新VBO

    texture_yx_ratio_ = 1.0 * grey_image.rows / grey_image.cols;
    set_texture_ok_ = true;
    GetDensity();
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
    if(event->buttons() & Qt::LeftButton)
        last_mouse_pos_ = event->pos();
    else if(event->buttons() & Qt::RightButton)
        last_right_mouse_pos_ = event->pos();
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
        last_mouse_pos_ = event->pos();
    }
    else if(event->buttons() & Qt::RightButton)
    {
        QPoint dis = event->pos() - last_right_mouse_pos_;
        QMatrix4x4 mouse_matrix;
        mouse_matrix.setToIdentity();
        mouse_matrix.rotate(light_rot_angle_.x(), 1,0,0);
        mouse_matrix.rotate(light_rot_angle_.y(), 0,1,0);
        mouse_matrix.rotate(light_rot_angle_.z(), 0,0,1);
        QVector4D x_vector(1,0,0,0);
        QVector4D y_vector(0,1,0,0);
        QVector4D z_vector(0,0,1,0);
        x_vector = mouse_matrix * x_vector;
        y_vector = mouse_matrix * y_vector;
        z_vector = mouse_matrix * z_vector;

        light_rot_angle_.setY(
                    light_rot_angle_.y() + (y_vector.y() / abs(y_vector.y())) * dis.x() / 20.0
                    );
        light_rot_angle_.setX(
                    light_rot_angle_.x() + dis.y()/20.0
                    );
//        float dis_length = dis.manhattanLength();
//        light_pos_ -= QVector3D(dis.x()/dis_length, dis.y()/dis_length,0);
//        if(light_pos_.x() > 1.3)
//            light_pos_.setX(1.3);
//        else if(light_pos_.x()<-1.3)
//            light_pos_.setX(-1.3);

//        if(light_pos_.y() > 1.3)
//            light_pos_.setY(1.3);
//        else if(light_pos_.y() < -1.3)
//            light_pos_.setY(-1.3);
        qDebug() << "change light dir";
        last_right_mouse_pos_ = event->pos();
    }


    update();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    float forward_offset = event->delta()/100.0f;
    this->camera.MoveForward(forward_offset);
    qDebug() << "wheel offset:" << forward_offset;
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

    shader->bind();
    this->model_mat_loc_ = shader->uniformLocation("model");
    this->view_mat_loc_ = shader->uniformLocation("view");
    this->projection_mat_loc_ = shader->uniformLocation("projection");
    this->active_texture_loc_ = shader->uniformLocation("active_texture");
    this->light_pos_loc_ = shader->uniformLocation("lightPos");
    this->camera_pos_loc_ = shader->uniformLocation("viewPos");
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
    shader->setUniformValue(active_texture_loc_, active_texture_);

    QMatrix4x4 light_rotate_mat;
    light_rotate_mat.setToIdentity();
    light_rotate_mat.rotate(light_rot_angle_.x(), 1, 0, 0);
    light_rotate_mat.rotate(light_rot_angle_.y(), 0, 1, 0);
    light_rotate_mat.rotate(light_rot_angle_.z(), 0, 0, 1);
    QVector3D temp_light_pos_ = light_rotate_mat * light_pos_;
    shader->setUniformValue(light_pos_loc_, temp_light_pos_);

    if(set_texture_ok_)
        texture->bind();

    glDrawElements(GL_TRIANGLES, this->grid_mesh_.GetIndexNum(), GL_UNSIGNED_INT, 0);
    shader->release();

    // draw line
//    glLoadIdentity();
//    glColor3f(1, 1, 0);
//    glBegin(GL_LINE);
//        glVertex3f(light_pos_.x(), light_pos_.y(), light_pos_.z());
//        glVertex3f(0, 0, 0);
//    glEnd();
}

bool GLWidget::Empty()
{
    qDebug() << "mesh vertex num:" << grid_mesh_.GetVertexNum();
    if(grid_mesh_.origin_.data == NULL)
        return true;
    return false;
}

void GLWidget::Reverse()
{
    this->grid_mesh_.Reverse();
    SetupVertexAttribs();
    update();
}

void GLWidget::ChangeContraValue(float contra_value)
{
    this->grid_mesh_.ChangeContraValue(contra_value);
    SetupVertexAttribs();
    update();
}

void GLWidget::ChangeMorphKernelSize(int size)
{
    this->grid_mesh_.ChangeMorphKernelSize(size);
}

void GLWidget::Morph_EroDila()
{
    if(Empty()){
        qDebug() << "GLWidget::Morph_EroDila:empty image";
        return;
    }
    qDebug() << "erode and dilate";
    this->grid_mesh_.ErodeAndDilate();
    SetupVertexAttribs();
    update();
    this->ShowInterImage(DENOISE_IMAGE);
}
void GLWidget::Morph_DilaEro()
{
    if(Empty()){
        qDebug() << "GLWidget::Morph_DilaEro:empty image";
        return;
    }
    qDebug() << "dilate and erode";
    this->grid_mesh_.DilateAndErode();
    SetupVertexAttribs();
    update();
    this->ShowInterImage(DENOISE_IMAGE);
}

void GLWidget::Morph_DilaEro2()
{
    if(Empty()){
        qDebug() << "GLWidget::Morph_DilaEro:empty image";
        return;
    }
    qDebug() << "dilate and erode";
    this->grid_mesh_.DilateAndErode2();
    SetupVertexAttribs();
    update();
    this->ShowInterImage(DENOISE_IMAGE);
}

void GLWidget::ChangeGKernelSize(int size)
{
    if(Empty())
        return;
    this->grid_mesh_.ChangeGKernelSize(size);
    SetupVertexAttribs();
    update();
}

void GLWidget::ChangeGSigma(float sigma)
{
    if(Empty())
        return;
    this->grid_mesh_.ChangeGSigma(sigma);
    SetupVertexAttribs();
    update();
}

void GLWidget::ChangeBlendA(float a)
{
    if(Empty())
        return;
    this->grid_mesh_.ChangeBlend_a(a);
    SetupVertexAttribs();
    update();
}

void GLWidget::ChangeBlendB(float b)
{
    if(Empty())
        return;
    this->grid_mesh_.ChangeBlend_b(b);
    SetupVertexAttribs();
    update();
}

void GLWidget::ChangeZFactor(float z_factor)
{
    this->grid_mesh_.AdjustZfactor(z_factor);
    SetupVertexAttribs();
    update();
}

void GLWidget::ChangeRenderMode(int mode)
{
    if(mode == 0){
        // texture
        active_texture_ = true;
    }
    else
        active_texture_ = false;

    update();
}

void GLWidget::ChangeZMapMode(int mode)
{
    grid_mesh_.ChangeZMapMode(mode);
    SetupVertexAttribs();
    update();
}

QString GLWidget::GetDensity()
{
    int densityx = this->grid_mesh_.GetDensityX();
    int densityy = this->grid_mesh_.GetDensityY();
    char density_str[100];
    sprintf(density_str, "%d X %d", densityx, densityy);
    QString density_info(density_str);
    emit GLWidgetDensityChanged(densityx, densityy);
    return density_info;
}

void GLWidget::ShowInterImage(InterImageType inter)
{
    QString title;
    switch(inter)
    {
    case CONTRA_IMAGE:
        // 显示对比度图片
        title = QString::fromLocal8Bit("调整对比度");
        image_widget_->setWindowTitle(title);
        image_widget_->showImage(grid_mesh_.contra_image_);
        break;
    case DENOISE_IMAGE:
        // 显示降噪后的图片
        title = QString::fromLocal8Bit("降噪");
        image_widget_->setWindowTitle(title);
        image_widget_->showImage(grid_mesh_.denoise_image_);
        break;
    case BLUR_IMAGE:
        // 显示降噪并高斯模糊后的图片
        title = QString::fromLocal8Bit("高斯模糊");
        image_widget_->setWindowTitle(title);
        image_widget_->showImage(grid_mesh_.blur_image_);
        break;
    case FINAL_BLEND_IAMGE:
        // 显示最终混合的图片
        title = QString::fromLocal8Bit("最终混合图片");
        image_widget_->setWindowTitle(title);
        image_widget_->showImage(grid_mesh_.final_blend_);
        break;
    default:
        title = QString::fromLocal8Bit("最终混合图片");
        image_widget_->setWindowTitle(title.toLocal8Bit());
        image_widget_->showImage(grid_mesh_.final_blend_);
        break;
    }


    image_widget_->show();
}

