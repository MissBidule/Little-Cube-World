#include "ShadowManager.hpp"
#include <glimac/common.hpp>
#include "glm/gtc/type_ptr.hpp"

ShadowManager::ShadowManager()
    : m_Light(nullptr), m_ProgramPoint(p6::load_shader("shaders/shadow.vs.glsl", "shaders/shadowPoint.fs.glsl")), m_ProgramDefault(p6::load_shader("shaders/shadow.vs.glsl", "shaders/shadowDefault.fs.glsl"))
{}

void ShadowManager::SetLight(LightManager& light)
{
    m_Light = &light;

    if (m_Light->getType() == glimac::LightType::Point)
    {
        m_uMVPLight       = glGetUniformLocation(m_ProgramPoint.id(), "uMVPLight");
        m_uMMatrix        = glGetUniformLocation(m_ProgramPoint.id(), "uMMatrix");
        m_uLightPos       = glGetUniformLocation(m_ProgramPoint.id(), "uLightPos");
        m_ufar_plane      = glGetUniformLocation(m_ProgramPoint.id(), "ufar_plane");
        m_uBoneTransforms = glGetUniformLocation(m_ProgramPoint.id(), "uBoneTransforms[0]");
        m_uIsMoving       = glGetUniformLocation(m_ProgramPoint.id(), "uIsMoving");
    }
    else
    {
        m_uMVPLight       = glGetUniformLocation(m_ProgramDefault.id(), "uMVPLight");
        m_uMMatrix        = glGetUniformLocation(m_ProgramDefault.id(), "uMMatrix");
        m_uLightPos       = glGetUniformLocation(m_ProgramDefault.id(), "uLightPos");
        m_ufar_plane      = glGetUniformLocation(m_ProgramDefault.id(), "ufar_plane");
        m_uBoneTransforms = glGetUniformLocation(m_ProgramDefault.id(), "uBoneTransforms[0]");
        m_uIsMoving       = glGetUniformLocation(m_ProgramDefault.id(), "uIsMoving");
    }
}

void ShadowManager::use()
{
    if (m_Light == nullptr)
        throw std::runtime_error("No light set in shadow program.");
    if (m_Light->getType() == glimac::LightType::Point)
    {
        m_ProgramPoint.use();
    }
    else
    {
        m_ProgramDefault.use();
    }
}

void ShadowManager::SendOBJtransform(const glm::mat4& OBJtransform, std::vector<glm::mat4> Transforms)
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
    glUniform1f(m_ufar_plane, m_Light->getFarPlane());
}