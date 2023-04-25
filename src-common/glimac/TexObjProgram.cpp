#include "TexObjProgram.hpp"
#include <glimac/common.hpp>

TexObjProgram::TexObjProgram(const std::string& vsPath, const std::string& fsPath)
    : ObjProgram(vsPath, fsPath), m_uTexKa(glGetUniformLocation(m_Program.id(), "uTexture.ka")), m_uTexKd(glGetUniformLocation(m_Program.id(), "uTexture.kd")), m_uTexKs(glGetUniformLocation(m_Program.id(), "uTexture.ks")), m_uShininess(glGetUniformLocation(m_Program.id(), "uTexture.shininess")), m_uOpacity(glGetUniformLocation(m_Program.id(), "uTexture.opacity"))
{}

std::vector<glm::mat4> TexObjProgram::getBoneTransforms([[maybe_unused]] int LOD)
{
    return {};
}

void TexObjProgram::addManualTexMesh(const std::vector<glimac::ShapeVertex>& shape, const glimac::Texture& texture)
{
    m_shapes.emplace_back(shape);
    m_textures.emplace_back(texture);
    m_LODsNB++;
}

void TexObjProgram::uniformRender(const std::vector<Light>& AllLights, int LOD)
{
    ObjProgram::uniformRender(AllLights, LOD);

    glUniform1i(m_uTexKa, static_cast<int>(MAXTAB * 2 + 0));
    glUniform1i(m_uTexKd, static_cast<int>(MAXTAB * 2 + 1));
    glUniform1i(m_uTexKs, static_cast<int>(MAXTAB * 2 + 2));
    glUniform1f(m_uShininess, m_textures[LOD].shininess);
    glUniform1f(m_uOpacity, m_textures[LOD].opacity);
}

void TexObjProgram::render(const std::vector<Light>& AllLights, const int LOD)
{
    for (size_t i = 0; i < AllLights.size() && i < MAXTAB; i++)
    {
        if (AllLights[i].getType() == glimac::LightType::Point)
            AllLights[i].bindRShadowMap(GL_TEXTURE0 + i * 2 + 1);
        else
            AllLights[i].bindRShadowMap(GL_TEXTURE0 + i * 2);
    }

    glActiveTexture(GL_TEXTURE0 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, m_textures[LOD].ka);
    glActiveTexture(GL_TEXTURE1 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, m_textures[LOD].kd);
    glActiveTexture(GL_TEXTURE3 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, m_textures[LOD].ks);

    TexObjProgram::shadowRender(LOD);

    for (size_t i = 0; i < MAXTAB; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i * 2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0 + i * 2 + 1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    glActiveTexture(GL_TEXTURE0 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3 + MAXTAB * 2);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TexObjProgram::shadowRender(int LOD)
{
    glBindVertexArray(m_VAO[LOD]);

    glDrawArrays(GL_TRIANGLES, 0, m_shapes[LOD].size());

    glBindVertexArray(0);
}