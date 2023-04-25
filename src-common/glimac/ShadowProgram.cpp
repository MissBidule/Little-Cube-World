#include "ShadowProgram.hpp"
#include <glimac/common.hpp>
#include "glm/gtc/type_ptr.hpp"

ShadowProgram::ShadowProgram()
    : m_Light(nullptr), m_Program(p6::load_shader("shaders/shadow.vs.glsl", "shaders/shadow.fs.glsl"))
{
    m_uMVPLight       = glGetUniformLocation(m_Program.id(), "uMVPLight");
    m_uMMatrix        = glGetUniformLocation(m_Program.id(), "uMMatrix");
    m_uLightPos       = glGetUniformLocation(m_Program.id(), "uLightPos");
    m_ufar_plane      = glGetUniformLocation(m_Program.id(), "ufar_plane");
    m_uLightType      = glGetUniformLocation(m_Program.id(), "uLightType");
    m_uBoneTransforms = glGetUniformLocation(m_Program.id(), "uBoneTransforms[0]");
    m_uIsMoving       = glGetUniformLocation(m_Program.id(), "uIsMoving");
}

void ShadowProgram::SetLight(Light& light)
{
    m_Light = &light;
}

void ShadowProgram::SendOBJtransform(const glm::mat4& OBJtransform, std::vector<glm::mat4> Transforms)
{
    for (int i = 0; i < static_cast<int>(Transforms.size()); i++)
    {
        glUniformMatrix4fv(m_uBoneTransforms + i * 4, 1, GL_FALSE, glm::value_ptr(Transforms[i]));
    }

    if (Transforms.size() == 0)
    {
        glUniform1i(m_uIsMoving, 0);
    }
    else
    {
        glUniform1i(m_uIsMoving, 1);
    }

    glUniformMatrix4fv(m_uMVPLight, 1, GL_FALSE, glm::value_ptr(m_Light->getProjMatrix() * m_Light->getVMatrix() * OBJtransform));
    glUniformMatrix4fv(m_uMMatrix, 1, GL_FALSE, glm::value_ptr(OBJtransform));

    // calcul of lightPos depending of its movement
    glm::vec3 lightPosition = glm::vec3((m_Light->getMMatrix()) * glm::vec4(m_Light->getPosition(), (static_cast<int>(m_Light->getType()) > static_cast<int>(glimac::LightType::Directional))));

    glUniform3fv(m_uLightPos, 1, glm::value_ptr(lightPosition));
    glUniform1i(m_uLightType, static_cast<int>(m_Light->getType()));
    glUniform1f(m_ufar_plane, m_Light->getFarPlane());
}