#include "SimpleObjectManager.hpp"
#include <glimac/ObjectManager.hpp>
#include <vector>
#include "ShadowMap.hpp"
#include "glm/gtc/type_ptr.hpp"

SimpleObjectManager::SimpleObjectManager(const std::string& vsPath, const std::string& fsPath)
    : ObjectManager(vsPath, fsPath), m_uKa(glGetUniformLocation(m_Program.id(), "uColor.ka")), m_uKd(glGetUniformLocation(m_Program.id(), "uColor.kd")), m_uKs(glGetUniformLocation(m_Program.id(), "uColor.ks")), m_uShininess(glGetUniformLocation(m_Program.id(), "uColor.shininess")), m_uOpacity(glGetUniformLocation(m_Program.id(), "uColor.opacity"))
{}

std::vector<glm::mat4> SimpleObjectManager::getBoneTransforms([[maybe_unused]] int LOD)
{
    return {};
}

void SimpleObjectManager::addManualMesh(const std::vector<glimac::ShapeVertex>& shape, const glimac::Color color)
{
    m_shapes.emplace_back(shape);
    m_colors.emplace_back(color);
    m_LODsNB++;
}

void SimpleObjectManager::uniformRender(const std::vector<LightManager>& AllLights, const int LOD, const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix)
{
    ObjectManager::uniformRender(AllLights, LOD, ViewMatrix, ProjMatrix);

    glUniform3fv(m_uKa, 1, glm::value_ptr(m_colors[LOD].ka));
    glUniform3fv(m_uKd, 1, glm::value_ptr(m_colors[LOD].kd));
    glUniform3fv(m_uKs, 1, glm::value_ptr(m_colors[LOD].ks));
    glUniform1f(m_uShininess, m_colors[LOD].shininess);
    glUniform1f(m_uOpacity, m_colors[LOD].opacity);
}

void SimpleObjectManager::render(const std::vector<LightManager>& AllLights, const int LOD)
{
    prerender(AllLights);

    SimpleObjectManager::shadowRender(LOD);

    postrender();
}

void SimpleObjectManager::shadowRender(const int LOD)
{
    glBindVertexArray(m_VAO[LOD]);
    glDrawArrays(GL_TRIANGLES, 0, m_shapes[LOD].size());
    glBindVertexArray(0);
}

void SimpleObjectManager::updateColor(const glimac::Color& newColor, int LOD)
{
    m_colors[LOD] = newColor;
}