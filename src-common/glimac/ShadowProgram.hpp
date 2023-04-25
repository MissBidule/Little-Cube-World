#ifndef SHADOW_PROGRAM_HPP
#define SHADOW_PROGRAM_HPP

#include "Light.hpp"
#include "p6/p6.h"

class ShadowProgram {
private:
    GLint  m_uMVPLight;
    GLint  m_uMMatrix;
    GLint  m_uLightPos;
    GLint  m_ufar_plane;
    GLint  m_uBoneTransforms;
    GLint  m_uIsMoving;
    Light* m_Light;

    p6::Shader m_ProgramPoint;
    p6::Shader m_ProgramDefault;

public:
    ShadowProgram();
    void use();
    void SetLight(Light& light);
    void SendOBJtransform(const glm::mat4& OBJtransform, std::vector<glm::mat4> Transforms);
};

#endif