#include "SimpleObjProgram.hpp"
#include <glimac/ShadowMapFBO.hpp>
#include "glm/gtc/type_ptr.hpp"

SimpleObjProgram::SimpleObjProgram(const std::string& vsPath, const std::string& fsPath)
    : ObjProgram(vsPath, fsPath), m_uKa(glGetUniformLocation(m_Program.id(), "uColor.ka")), m_uKd(glGetUniformLocation(m_Program.id(), "uColor.kd")), m_uKs(glGetUniformLocation(m_Program.id(), "uColor.ks")), m_uShininess(glGetUniformLocation(m_Program.id(), "uColor.shininess")), m_uOpacity(glGetUniformLocation(m_Program.id(), "uColor.opacity"))
{}

void SimpleObjProgram::addManualMesh(const std::vector<glimac::ShapeVertex>& shape, const glimac::Color color)
{
    m_shapes.emplace_back(shape);
    m_colors.emplace_back(color);
    m_LODsNB++;
}

void SimpleObjProgram::uniformRender(const std::vector<glimac::Light>& AllLights, const int LOD)
{
    ObjProgram::uniformRender(AllLights, LOD);

    glUniform3fv(m_uKa, 1, glm::value_ptr(m_colors[LOD].ka));
    glUniform3fv(m_uKd, 1, glm::value_ptr(m_colors[LOD].kd));
    glUniform3fv(m_uKs, 1, glm::value_ptr(m_colors[LOD].ks));
    glUniform1f(m_uShininess, m_colors[LOD].shininess);
    glUniform1f(m_uOpacity, m_colors[LOD].opacity);
}

void SimpleObjProgram::render(const std::vector<glimac::Light>& AllLights, const int LOD)
{
    for (size_t i = 0; i < AllLights.size() && i < MAXTAB; i++)
    {
        AllLights[i].shadowMap.BindForReading(GL_TEXTURE0 + i);
    }

    SimpleObjProgram::shadowRender(LOD);

    for (size_t i = 0; i < AllLights.size() && i < MAXTAB; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void SimpleObjProgram::shadowRender(const int LOD)
{
    glBindVertexArray(m_VAO[LOD]);
    glDrawArrays(GL_TRIANGLES, 0, m_shapes[LOD].size());
    glBindVertexArray(0);
}