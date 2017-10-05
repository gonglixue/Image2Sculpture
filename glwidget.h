#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QDir>
#include <QString>
#include <QDebug>
#include <QPoint>
#include <qevent.h>
#include <QVector4D>
#include <QImage>
#include <QMessageBox>
//#include <QtOpenGL/QtOpenGL>
//#include <QtOpenGL/qgl.h>
//#include <QGLWidget>
#include "gridmesh.h"
#include "qcamera.h"
#include "cvimagewidget.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget:public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GLWidget(QWidget *parent=0);
    ~GLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void SetTexture(QString texture_fn);
    void SaveMesh(QString mesh_fn);
    bool Empty();
    QString GetDensity();

    void Reverse();
    void ChangeContraValue(float contra_value);
    void ChangeMorphKernelSize(int size);
    void ChangeGKernelSize(int size);
    void ChangeGSigma(float sigma);
    void ChangeBlendA(float a);
    void ChangeBlendB(float b);
    void ChangeZFactor(float z_factor);
    void ChangeRenderMode(int mode);
    void ChangeZMapMode(int mode);
    void ChangeThickness(float thickness){
        grid_mesh_.ChangeThickness(thickness);
        SetupVertexAttribs();
        update();
    }

    void ShowInterImage(InterImageType inter);
public slots:
    void cleanup();
    void Morph_EroDila();
    void Morph_DilaEro();
    void Morph_DilaEro2();
    void ChangeGridMeshDensity(int dx)
    {
        grid_mesh_.ChangeDensity(dx);
        SetupVertexAttribs();
        update();
    }
    void ChangeDistFieldRange(int r)
    {
        grid_mesh_.ChangeDistRange(r);
        SetupVertexAttribs();
        update();
    }

signals:
    void GLWidgetDensityChanged(int density_x, int density_y);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    GridMesh grid_mesh_;
    CVImageWidget* image_widget_;

    void SetupVertexAttribs();

    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;
    QOpenGLBuffer EBO;
    QOpenGLShaderProgram *shader;  // texture
    GLuint model_mat_loc_;
    GLuint projection_mat_loc_;
    GLuint view_mat_loc_;
    GLuint active_texture_loc_;
    GLuint light_pos_loc_;
    GLuint camera_pos_loc_;
    QOpenGLTexture *texture;
    QImage texture_QImage_;

    QString vertex_shader_fn_;
    QString frag_shader_fn_;

    QCamera camera;
    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;
    float texture_yx_ratio_;
    QVector3D rot_angle_;
    QVector3D light_rot_angle_;
    QVector3D light_pos_;

    // event params
    QPoint last_mouse_pos_;
    QPoint last_right_mouse_pos_;

    // state
    bool set_texture_ok_;
    bool active_texture_;
};

#endif // GLWIDGET_H
