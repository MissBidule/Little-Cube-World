#ifndef SHADOW_MANAGER_HPP
#define SHADOW_MANAGER_HPP

#include "LightManager.hpp"
#include "p6/p6.h"

class ShadowManager {
private:
    GLint         m_uMVPLight;
    GLint         m_uMMatrix;
    GLint         m_uLightPos;
    GLint         m_ufar_plane;
    GLint         m_uBoneTransforms;
    GLint         m_uIsMoving;
    LightManager* m_Light;

    p6::Shader m_ProgramPoint;
    p6::Shader m_ProgramDefault;

public:
    ShadowManager();
    void use();
    void SetLight(LightManager& light);
    void SendOBJtransform(const glm::mat4& OBJtransform, std::vector<glm::mat4> Transforms);
};

#endif