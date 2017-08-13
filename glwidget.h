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
#include "gridmesh.h"
#include "qcamera.h"

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

    void Reverse();
    void ChangeContraValue(float contra_value);
    void ChangeMorphKernelSize(int size);
    void ChangeGKernelSize(int size);
    void ChangeGSigma(float sigma);

public slots:
    void cleanup();
    void Morph_EroDila();
    void Morph_DilaEro();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private:
    GridMesh grid_mesh_;

    void SetupVertexAttribs();

    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO;
    QOpenGLBuffer EBO;
    QOpenGLShaderProgram *shader;
    GLuint model_mat_loc_;
    GLuint projection_mat_loc_;
    GLuint view_mat_loc_;
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

    // event params
    QPoint last_mouse_pos_;

    // state
    bool set_texture_ok_;
};

#endif // GLWIDGET_H
