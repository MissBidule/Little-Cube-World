#include "TexObjectManager.hpp"
#include <glimac/common.hpp>

TexObjectManager::TexObjectManager(const std::string& vsPath, const std::string& fsPath)
    : ObjectManager(vsPath, fsPath), m_uTexKa(glGetUniformLocation(m_Program.id(), "uTexture.ka")), m_uTexKd(glGetUniformLocation(m_Program.id(), "uTexture.kd")), m_uTexKs(glGetUniformLocation(m_Program.id(), "uTexture.ks")), m_uShininess(glGetUniformLocation(m_Program.id(), "uTexture.shininess")), m_uOpacity(glGetUniformLocation(m_Program.id(), "uTexture.opacity"))
{}

std::vector<glm::mat4> TexObjectManager::getBoneTransforms([[maybe_unused]] int LOD)
{
    return {};
}

void TexObjectManager::addManualTexMesh(const std::vector<glimac::ShapeVertex>& shape, const glimac::Texture& texture)
{
    m_shapes.emplace_back(shape);
    m_textures.emplace_back(texture);
    m_LODsNB++;
}

void TexObjectManager::uniformRender(const std::vector<LightManager>& AllLights, int LOD, const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix)
{
    ObjectManager::uniformRender(AllLights, LOD, ViewMatrix, ProjMatrix);

    glUniform1i(m_uTexKa, static_cast<int>(MAXTAB * 2 + 0));
    glUniform1i(m_uTexKd, static_cast<int>(MAXTAB * 2 + 1));
    glUniform1i(m_uTexKs, static_cast<int>(MAXTAB * 2 + 2));
    glUniform1f(m_uShininess, m_textures[LOD].shininess);
    glUniform1f(m_uOpacity, m_textures[LOD].opacity);
}

void TexObjectManager::render(const std::vector<LightManager>& AllLights, const int LOD)
{
    prerender(AllLights);

    glActiveTexture(GL_TEXTURE0 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, m_textures[LOD].ka);
    glActiveTexture(GL_TEXTURE1 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, m_textures[LOD].kd);
    glActiveTexture(GL_TEXTURE2 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, m_textures[LOD].ks);

    TexObjectManager::shadowRender(LOD);

    glActiveTexture(GL_TEXTURE0 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, 0);

    postrender();
}

void TexObjectManager::shadowRender(int LOD)
{
    glBindVertexArray(m_VAO[LOD]);

    glDrawArrays(GL_TRIANGLES, 0, m_shapes[LOD].size());

    glBindVertexArray(0);
}