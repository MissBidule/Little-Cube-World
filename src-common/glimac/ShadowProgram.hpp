#ifndef SHADOW_PROGRAM_HPP
#define SHADOW_PROGRAM_HPP

#include <glimac/common.hpp>
#include "p6/p6.h"

class ShadowProgram {
private:
    GLint          m_uMVPLight;
    glimac::Light* m_Light;

public:
    p6::Shader m_Program;

    ShadowProgram();
    void SetLight(glimac::Light& light);
    void SendOBJtransform(const glm::mat4& OBJtransform);
};

#endif