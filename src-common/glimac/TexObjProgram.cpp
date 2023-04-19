#include "TexObjProgram.hpp"

TexObjProgram::TexObjProgram(const std::string& vsPath, const std::string& fsPath)
    : ObjProgram(vsPath, fsPath), m_uTexKa(glGetUniformLocation(m_Program.id(), "uTexture.ka")), m_uTexKd(glGetUniformLocation(m_Program.id(), "uTexture.kd")), m_uTexKs(glGetUniformLocation(m_Program.id(), "uTexture.ks")), m_uShininess(glGetUniformLocation(m_Program.id(), "uTexture.shininess")), m_uOpacity(glGetUniformLocation(m_Program.id(), "uTexture.opacity"))
{}

void TexObjProgram::addManualTexMesh(const std::vector<glimac::ShapeVertex>& shape, const glimac::Texture& texture)
{
    m_shapes.emplace_back(shape);
    m_textures.emplace_back(texture);
    m_LODsNB++;
}

void TexObjProgram::uniformRender(const std::vector<glimac::Light>& AllLights, int LOD)
{
    ObjProgram::uniformRender(AllLights, LOD);

    glUniform1i(m_uTexKa, static_cast<int>(MAXTAB) + 0);
    glUniform1i(m_uTexKd, static_cast<int>(MAXTAB) + 1);
    glUniform1i(m_uTexKs, static_cast<int>(MAXTAB) + 0);
    glUniform1f(m_uShininess, m_textures[LOD].shininess);
    glUniform1f(m_uOpacity, m_textures[LOD].opacity);
}

void TexObjProgram::render(const std::vector<glimac::Light>& AllLights, const int LOD)
{
    glActiveTexture(GL_TEXTURE0 + MAXTAB);
    glBindTexture(GL_TEXTURE_2D, m_textures[LOD].ka);
    glActiveTexture(GL_TEXTURE1 + MAXTAB);
    glBindTexture(GL_TEXTURE_2D, m_textures[LOD].kd);
    glActiveTexture(GL_TEXTURE3 + MAXTAB);
    glBindTexture(GL_TEXTURE_2D, m_textures[LOD].ks);

    for (size_t i = 0; i < AllLights.size() && i < MAXTAB; i++)
    {
        AllLights[i].shadowMap.BindForReading(GL_TEXTURE0 + i);
    }

    TexObjProgram::shadowRender(LOD);

    for (size_t i = 0; i < AllLights.size() && i < MAXTAB; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glActiveTexture(GL_TEXTURE0 + MAXTAB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1 + MAXTAB);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3 + MAXTAB);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TexObjProgram::shadowRender(int LOD)
{
    glBindVertexArray(m_VAO[LOD]);

    glDrawArrays(GL_TRIANGLES, 0, m_shapes[LOD].size());

    glBindVertexArray(0);
}