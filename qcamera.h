#ifndef QCAMERA_H
#define QCAMERA_H

#include <QOpenGLFunctions>
#include <QVector3D>
#include <QMatrix4x4>
#include <iostream>

enum Camera_MOVEMENT{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVITY = 0.25f;
const GLfloat ZOOM = 45.0f;

class QCamera
{
public:
    QVector3D Position;
    QVector3D Front;  // target - eye
    QVector3D Up;
    QVector3D Right;
    QVector3D WorldUp;
    // Euler angle
    GLfloat Yaw;
    GLfloat Pitch;
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;

    QCamera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f),
            QVector3D up = QVector3D(0.0f, 1.0f, 0.0f),
            GLfloat yaw = YAW,
            GLfloat pitch = PITCH);

    QMatrix4x4 GetViewMatrix(){
        QMatrix4x4 view;
        view.setToIdentity();
        view.lookAt(this->Position, this->Position+this->Front, this->Up);
        return view;
    }

    void ProcessKeyboard(Camera_MOVEMENT direction, GLfloat deltaTime);
private:
    void updateCameraVectors();
};

#endif // QCAMERA_H
