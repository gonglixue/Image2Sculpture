#include "qcamera.h"
#include <QtMath>
QCamera::QCamera(QVector3D position, QVector3D up, GLfloat yaw, GLfloat pitch):
    Front(QVector3D(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY),
    Zoom(ZOOM)
{
    this->Position = position;
    this->WorldUp = up;
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
}

void QCamera::ProcessKeyboard(Camera_MOVEMENT direction, GLfloat deltaTime)
{
    qDebug() << "process key in camera";
    GLfloat velocity = this->MovementSpeed * deltaTime;
    if(direction == FORWARD)
        this->Position += this->Front * velocity;
    if(direction == BACKWARD)
        this->Position -= this->Front * velocity;
    if(direction == LEFT)
        this->Position -= this->Right * velocity;
    if(direction == RIGHT)
        this->Position += this->Right * velocity;
}

void QCamera::updateCameraVectors()
{
    QVector3D front;
    front.setX( qCos(qDegreesToRadians(this->Yaw)) );
    front.setZ( qSin(qDegreesToRadians(this->Yaw)) * qCos(qDegreesToRadians(this->Pitch)) );
    front.setY( qSin(qDegreesToRadians(this->Pitch)) );
    front.normalize();
    this->Front = front;

    QVector3D temp1 = QVector3D::crossProduct(this->Front, this->WorldUp);
    temp1.normalize();
    this->Right = temp1;
    temp1 = QVector3D::crossProduct(this->Right, this->Front);
    temp1.normalize();
    this->Up = temp1;
}
