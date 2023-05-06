#include "TrackballCamera.hpp"
#include <iostream>
#include "glm/gtx/transform.hpp"
#include "glm/trigonometric.hpp"

TrackballCamera::TrackballCamera(glm::vec3 position, float Zoom, float AngleX, float AngleY)
    : m_position(-position), m_fZoom(Zoom), m_fAngleX(AngleX), m_fAngleY(AngleY) {}

void TrackballCamera::moveFront(float delta)
{
    float x = 0.f;
    float z = 0.f;

    int angle = static_cast<int>(m_fAngleY);
    if (angle < 0)
    {
        angle = 360 - (-angle) % 360;
    }
    else
    {
        angle = angle % 360;
    }

    if (angle < 90)
    {
        z = ((90 - angle) / 90.f) * delta;
        x = -((angle) / 90.f) * delta;
    }
    if (angle >= 90 && angle < 180)
    {
        z = -((angle - 90) / 90.f) * delta;
        x = -((180 - angle) / 90.f) * delta;
    }
    if (angle >= 180 && angle < 270)
    {
        z = -((270 - angle) / 90.f) * delta;
        x = ((angle - 180) / 90.f) * delta;
    }
    if (angle >= 270)
    {
        z = ((angle - 270) / 90.f) * delta;
        x = ((360 - angle) / 90.f) * delta;
    }

    m_position = glm::vec3(m_position.x + x, m_position.y, m_position.z + z);
}

void TrackballCamera::moveLeft(float delta)
{
    float x = 0.f;
    float z = 0.f;

    int angle = static_cast<int>(m_fAngleY - 90);
    if (angle < 0)
    {
        angle = 360 - (-angle) % 360;
    }
    else
    {
        angle = angle % 360;
    }

    if (angle < 90)
    {
        z = ((90 - angle) / 90.f) * delta;
        x = -((angle) / 90.f) * delta;
    }
    if (angle >= 90 && angle < 180)
    {
        z = -((angle - 90) / 90.f) * delta;
        x = -((180 - angle) / 90.f) * delta;
    }
    if (angle >= 180 && angle < 270)
    {
        z = -((270 - angle) / 90.f) * delta;
        x = ((angle - 180) / 90.f) * delta;
    }
    if (angle >= 270)
    {
        z = ((angle - 270) / 90.f) * delta;
        x = ((360 - angle) / 90.f) * delta;
    }

    m_position = glm::vec3(m_position.x + x, m_position.y, m_position.z + z);
}

void TrackballCamera::moveUp(float delta)
{
    m_position = glm::vec3(m_position.x, m_position.y + delta, m_position.z);
}

void TrackballCamera::setPos(glm::vec3 position)
{
    m_position = -position;
}

void TrackballCamera::rotateLeft(float degrees)
{
    m_fAngleY += degrees;
}

void TrackballCamera::rotateUp(float degrees)
{
    m_fAngleX += degrees;
}

void TrackballCamera::zoom(float delta)
{
    m_fZoom -= delta;
}

glm::vec3 TrackballCamera::getPosition() const
{
    return -m_position;
}

glm::mat4 TrackballCamera::getViewMatrix() const
{
    glm::mat4 matrix = glm::mat4(1);
    matrix           = glm::translate(matrix, glm::vec3(0, 0, -m_fZoom));
    matrix           = glm::rotate(matrix, glm::radians(m_fAngleX), glm::vec3(1, 0, 0));
    matrix           = glm::rotate(matrix, glm::radians(m_fAngleY), glm::vec3(0, 1, 0));
    matrix           = glm::translate(matrix, m_position);

    return matrix;
}

float TrackballCamera::getAngleY() const
{
    return m_fAngleY;
}

float TrackballCamera::getAngleX() const
{
    return m_fAngleX;
}
