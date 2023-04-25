#include "Light.hpp"
#include <glimac/ShadowCubeMap.hpp>
#include <glimac/common.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/trigonometric.hpp"

Light::Light(glimac::LightType type)
    : m_color(glm::vec3(0)), m_type(type), m_frontT(0), m_leftT(0), m_upR(0), m_leftR(0), m_position(glm::vec3(0)), m_ProjMatrix(glm::mat4(0))
{
    float valueOrtho = 35.f;

    switch (type)
    {
    case glimac::LightType::Directional:
        m_ProjMatrix = glm::ortho(-valueOrtho, valueOrtho, -valueOrtho, valueOrtho, -valueOrtho, valueOrtho);
        break;

    case glimac::LightType::Spot:

    case glimac::LightType::Point:
        m_ProjMatrix = glm::perspective(glm::radians(90.f), 4096 / static_cast<float>(4096), 0.1f, m_far_plane);
        break;
    }
}

void Light::setPosition(glm::vec3 position)
{
    m_position = position;
    if (m_type == glimac::LightType::Directional)
    {
        ViewMatrixDirLight.rotateUp(glm::degrees(glm::orientedAngle(glm::normalize(m_position), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0))));
        return;
    }
    ViewMatrixSpotPointLight.rotateUp(glm::degrees(glm::orientedAngle(glm::normalize(m_position), glm::vec3(0, 0, -1), glm::vec3(1, 0, 0))));
}

void Light::refreshPosition()
{
    if (m_type == glimac::LightType::Directional)
        return;
    glm::mat4 MMatrix = glm::rotate(glm::mat4(1), -glm::radians(m_leftR + 45), glm::vec3(0, 1, 0));
    MMatrix           = glm::rotate(MMatrix, -glm::radians(m_upR), glm::vec3(1, 0, 0));
    ViewMatrixSpotPointLight.setPos(glm::vec3(MMatrix * glm::vec4(m_position, 1)));
}

void Light::moveFront(float t)
{
    if (m_type == glimac::LightType::Directional)
    {
        ViewMatrixDirLight.moveFront(t);
        return;
    }
    ViewMatrixSpotPointLight.moveFront(t);
    m_frontT += t;
}

void Light::moveLeft(float t)
{
    if (m_type == glimac::LightType::Directional)
    {
        return;
    }
    ViewMatrixSpotPointLight.moveLeft(t);
    m_leftT += t;
}

void Light::rotateUp(float degrees)
{
    if (m_type == glimac::LightType::Directional)
    {
        ViewMatrixDirLight.rotateUp(degrees);
    }
    else
    {
        ViewMatrixSpotPointLight.rotateUp(-degrees);
    }
    m_upR += degrees;
}

void Light::rotateLeft(float degrees)
{
    if (m_type == glimac::LightType::Directional)
    {
        ViewMatrixDirLight.rotateLeft(degrees);
    }
    else
    {
        ViewMatrixSpotPointLight.rotateLeft(-degrees);
    }
    m_leftR += degrees;
}

glm::mat4 Light::getMMatrix() const
{
    glm::mat4 MMatrix = glm::translate(glm::mat4(1), glm::vec3(-m_leftT, 0, m_frontT));
    MMatrix           = glm::rotate(MMatrix, -glm::radians(m_leftR), glm::vec3(0, 1, 0));
    MMatrix           = glm::rotate(MMatrix, -glm::radians(m_upR), glm::vec3(1, 0, 0));

    return MMatrix;
}

glm::mat4 Light::getVMatrix() const
{
    if (m_type == glimac::LightType::Directional)
    {
        return ViewMatrixDirLight.getViewMatrix();
    }

    return ViewMatrixSpotPointLight.getViewMatrix();
}

glm::mat4 Light::getProjMatrix() const
{
    return m_ProjMatrix;
}

glm::vec3 Light::getPosition() const
{
    return m_position;
}

glimac::LightType Light::getType() const
{
    return (m_type);
}

float Light::getFarPlane() const
{
    return (m_far_plane);
}

void Light::initShadowMap(unsigned int WindowSize)
{
    if (m_type == glimac::LightType::Point)
    {
        shadowMapPointLight.Init(WindowSize);
    }
    else
    {
        shadowMapSpotDir.Init(WindowSize);
    }
}

void Light::bindWShadowMap(unsigned int CubeMap)
{
    if (m_type == glimac::LightType::Point)
    {
        shadowMapPointLight.BindForWriting(CubeMap);
        ViewMatrixSpotPointLight.setTheta(shadowMapPointLight.getTheta(CubeMap));
        ViewMatrixSpotPointLight.setPhi(shadowMapPointLight.getPhi(CubeMap));
    }
    else
    {
        shadowMapSpotDir.BindForWriting();
    }
}

void Light::bindRShadowMap(GLenum TextureUnit) const
{
    if (m_type == glimac::LightType::Point)
    {
        shadowMapPointLight.BindForReading(TextureUnit);
    }
    else
    {
        shadowMapSpotDir.BindForReading(TextureUnit);
    }
}