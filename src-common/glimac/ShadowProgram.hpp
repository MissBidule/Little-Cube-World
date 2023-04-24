#ifndef SHADOW_PROGRAM_HPP
#define SHADOW_PROGRAM_HPP

#include "Light.hpp"
#include "p6/p6.h"

class ShadowProgram {
private:
    GLint  m_uMVPLight;
    Light* m_Light;

public:
    p6::Shader m_Program;

    ShadowProgram();
    void SetLight(Light& light);
    void SendOBJtransform(const glm::mat4& OBJtransform);
};

#endif