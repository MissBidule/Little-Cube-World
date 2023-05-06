#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glimac/FreeflyCamera.hpp>
#include <glimac/ShadowCubeMap.hpp>
#include <glimac/ShadowMap.hpp>
#include <glimac/TrackballCamera.hpp>
#include <glimac/common.hpp>
#include "p6/p6.h"

class LightManager {
public:
    glm::vec3 m_color;

    explicit LightManager(glimac::LightType type);
    void setPosition(glm::vec3 position);
    void setDirection(glm::vec3 direction);
    void refreshPosition();
    void moveFront(float t);
    void moveLeft(float t);
    void rotateUp(float degrees);
    void rotateLeft(float degrees);
    void setPhi(float degrees);

    glm::mat4         getMMatrix() const;
    glm::mat4         getVMatrix() const;
    glm::mat4         getProjMatrix() const;
    glm::vec3         getPosition() const;
    glimac::LightType getType() const;
    float             getFarPlane() const;

    void initShadowMap(unsigned int WindowSize);
    void bindWShadowMap(unsigned int CubeMap = 0);
    void bindRShadowMap(GLenum TextureUnit) const;

private:
    float             m_far_plane = 15.f;
    glimac::LightType m_type;
    float             m_frontT;
    float             m_leftT;
    float             m_upR;
    float             m_leftR;
    glm::vec3         m_position;
    glm::mat4         m_ProjMatrix;
    FreeflyCamera     m_ViewMatrixSpotPointLight;
    TrackballCamera   m_ViewMatrixDirLight;
    ShadowMap         m_shadowMapSpotDirLight;
    ShadowCubeMap     m_shadowMapPointLight;
};

#endif