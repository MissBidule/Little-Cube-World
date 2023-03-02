#include "TrackballCamera.hpp"
#include "glm/gtx/transform.hpp"

TrackballCamera::TrackballCamera(float Distance, float AngleX, float AngleY)
    : m_fDistance(Distance), m_fAngleX(AngleX), m_fAngleY(AngleY) {}

void TrackballCamera::moveFront(float delta)
{
    m_fDistance -= delta;
}

void TrackballCamera::rotateLeft(float degrees)
{
    m_fAngleY += degrees;
}
void TrackballCamera::rotateUp(float degrees)
{
    m_fAngleX += degrees;
}

glm::mat4 TrackballCamera::getViewMatrix() const
{
    glm::mat4 matrix = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -m_fDistance));
    matrix           = glm::rotate(matrix, m_fAngleX, glm::vec3(1, 0, 0));
    matrix           = glm::rotate(matrix, m_fAngleY, glm::vec3(0, 1, 0));

    return matrix;
}
