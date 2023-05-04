#include "LightManager.hpp"
#include <glimac/ShadowCubeMap.hpp>
#include <glimac/common.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/trigonometric.hpp"

LightManager::LightManager(glimac::LightType type)
    : m_color(glm::vec3(0)), m_type(type), m_frontT(0), m_leftT(0), m_upR(0), m_leftR(0), m_position(glm::vec3(0)), m_ProjMatrix(glm::mat4(0))
{
    float valueOrtho = 45.f;

    switch (type)
    {
    case glimac::LightType::Directional:
        m_ProjMatrix = glm::ortho(-valueOrtho, valueOrtho, -valueOrtho, valueOrtho, -valueOrtho, valueOrtho);
        break;

    case glimac::LightType::Spot:
        m_ProjMatrix = glm::perspective(glm::radians(135.f), 4096 / static_cast<float>(4096), 0.1f, m_far_plane);
        break;

    case glimac::LightType::Point:
        m_ProjMatrix = glm::perspective(glm::radians(90.f), 4096 / static_cast<float>(4096), 0.1f, m_far_plane);
        break;
    }
}

void LightManager::setPosition(glm::vec3 position)
{
    m_position = position;

    m_ViewMatrixSpotPointLight.setPos(m_position);
    if (m_type == glimac::LightType::Point)
        return;

    float up = glm::degrees(glm::orientedAngle(glm::vec2(1, 0), glm::normalize(glm::vec2(m_position.x, m_position.y))));
    if (!(up >= -360))
        up = 0;

    float left = glm::degrees(glm::orientedAngle(glm::vec2(0, 1), glm::normalize(glm::vec2(m_position.x, m_position.z))));
    if (!(left >= -360))
        left = 0;

    if (m_type == glimac::LightType::Directional)
    {
        m_ViewMatrixDirLight.rotateUp(up);
        m_ViewMatrixDirLight.rotateLeft(glm::degrees(left) + 45);
        return;
    }
    m_ViewMatrixSpotPointLight.rotateUp(-up);
    m_ViewMatrixSpotPointLight.rotateLeft(-(glm::degrees(left)));
}

void LightManager::setDirection(glm::vec3 direction)
{
    if (m_type != glimac::LightType::Spot)
        return;

    float up = glm::degrees(glm::orientedAngle(glm::vec2(1, 0), glm::normalize(glm::vec2(direction.x, direction.y))));
    if (!(up >= -360))
        up = 0;

    float left = glm::degrees(glm::orientedAngle(glm::vec2(0, 1), glm::normalize(glm::vec2(direction.x, direction.z))));
    if (!(left >= -360))
        left = 0;

    m_ViewMatrixSpotPointLight.setPhi(left);
    m_ViewMatrixSpotPointLight.setTheta(up);
}

void LightManager::refreshPosition()
{
    if (m_type == glimac::LightType::Directional)
        return;
    glm::mat4 MMatrix     = glm::rotate(glm::mat4(1), -glm::radians(m_leftR), glm::vec3(0, 1, 0));
    MMatrix               = glm::rotate(MMatrix, -glm::radians(m_upR), glm::vec3(1, 0, 0));
    glm::vec3 newPosition = glm::vec3(MMatrix * glm::vec4(m_position, 1));
    m_ViewMatrixSpotPointLight.setPos(newPosition);
}

void LightManager::moveFront(float t)
{
    if (m_type == glimac::LightType::Directional)
    {
        m_ViewMatrixDirLight.moveFront(t);
        return;
    }
    m_ViewMatrixSpotPointLight.moveFront(t);
    m_frontT += t;
}

void LightManager::moveLeft(float t)
{
    if (m_type == glimac::LightType::Directional)
    {
        return;
    }
    m_ViewMatrixSpotPointLight.moveLeft(t);
    m_leftT += t;
}

void LightManager::rotateUp(float degrees)
{
    if (m_type == glimac::LightType::Directional)
    {
        m_ViewMatrixDirLight.rotateUp(degrees);
    }
    else
    {
        m_ViewMatrixSpotPointLight.rotateUp(-degrees);
    }
    m_upR += degrees;
}

void LightManager::rotateLeft(float degrees)
{
    if (m_type == glimac::LightType::Directional)
    {
        m_ViewMatrixDirLight.rotateLeft(degrees);
    }
    else
    {
        m_ViewMatrixSpotPointLight.rotateLeft(-degrees);
    }
    m_leftR += degrees;
}

glm::mat4 LightManager::getMMatrix() const
{
    glm::mat4 MMatrix = glm::translate(glm::mat4(1), glm::vec3(-m_leftT, 0, m_frontT));
    MMatrix           = glm::rotate(MMatrix, -glm::radians(m_leftR), glm::vec3(0, 1, 0));
    MMatrix           = glm::rotate(MMatrix, -glm::radians(m_upR), glm::vec3(1, 0, 0));

    return MMatrix;
}

glm::mat4 LightManager::getVMatrix() const
{
    if (m_type == glimac::LightType::Directional)
    {
        return m_ViewMatrixDirLight.getViewMatrix();
    }

    return m_ViewMatrixSpotPointLight.getViewMatrix();
}

glm::mat4 LightManager::getProjMatrix() const
{
    return m_ProjMatrix;
}

glm::vec3 LightManager::getPosition() const
{
    return m_position;
}

glimac::LightType LightManager::getType() const
{
    return (m_type);
}

float LightManager::getFarPlane() const
{
    return (m_far_plane);
}

void LightManager::initShadowMap(unsigned int WindowSize)
{
    if (m_type == glimac::LightType::Point)
    {
        m_shadowMapPointLight.Init(WindowSize);
    }
    else
    {
        m_shadowMapSpotDirLight.Init(WindowSize);
    }
}

void LightManager::bindWShadowMap(unsigned int CubeMap)
{
    if (m_type == glimac::LightType::Point)
    {
        m_shadowMapPointLight.BindForWriting(CubeMap);
        m_ViewMatrixSpotPointLight.setTheta(m_shadowMapPointLight.getTheta(CubeMap));
        m_ViewMatrixSpotPointLight.setPhi(m_shadowMapPointLight.getPhi(CubeMap));
    }
    else
    {
        m_shadowMapSpotDirLight.BindForWriting();
    }
}

void LightManager::bindRShadowMap(GLenum TextureUnit) const
{
    if (m_type == glimac::LightType::Point)
    {
        m_shadowMapPointLight.BindForReading(TextureUnit);
    }
    else
    {
        m_shadowMapSpotDirLight.BindForReading(TextureUnit);
    }
}