#include "ShadowProgram.hpp"
#include <glimac/common.hpp>
#include "glm/gtc/type_ptr.hpp"

ShadowProgram::ShadowProgram()
    : m_Program(p6::load_shader("shaders/shadow.vs.glsl", "shaders/shadow.fs.glsl"))
{
    m_uMVPLight = glGetUniformLocation(m_Program.id(), "uMVPLight");
}

void ShadowProgram::SetLight(glimac::Light& light)
{
    m_Light = &light;
}

void ShadowProgram::SendOBJtransform(const glm::mat4& OBJtransform)
{
    glUniformMatrix4fv(m_uMVPLight, 1, GL_FALSE, glm::value_ptr(m_Light->ProjMatrix * m_Light->ViewMatrix * OBJtransform));
}