#include "SkinnedObjProgram.hpp"
#include <glimac/ObjProgram.hpp>
#include <glimac/SkinnedMesh.hpp>
#include <memory>
#include "glm/gtc/type_ptr.hpp"

SkinnedObjProgram::SkinnedObjProgram(const std::string& vsPath, const std::string& fsPath, p6::Context& ctx)
    : ObjProgram(vsPath, fsPath), m_uKa(glGetUniformLocation(m_Program.id(), "uColor.ka")), m_uKd(glGetUniformLocation(m_Program.id(), "uColor.kd")), m_uKs(glGetUniformLocation(m_Program.id(), "uColor.ks")), m_uShininess(glGetUniformLocation(m_Program.id(), "uColor.shininess")), m_uOpacity(glGetUniformLocation(m_Program.id(), "uColor.opacity")), m_uBoneTransforms(glGetUniformLocation(m_Program.id(), "uBoneTransforms[0]")), m_ctx(&ctx)
{}

void SkinnedObjProgram::addSkinnedMesh(const std::string& objectPath)
{
    m_meshes.emplace_back(std::make_unique<SkinnedMesh>());
    m_meshes[m_LODsNB++]->LoadMesh(objectPath);
}

void SkinnedObjProgram::uniformRender(const std::vector<Light>& AllLights, const int LOD)
{
    ObjProgram::uniformRender(AllLights, LOD);
}

void SkinnedObjProgram::render(const std::vector<Light>& AllLights, const int LOD)
{
    for (size_t i = 0; i < AllLights.size() && i < MAXTAB; i++)
    {
        if (AllLights[i].getType() == glimac::LightType::Point)
            AllLights[i].bindRShadowMap(GL_TEXTURE0 + i * 2 + 1);
        else
            AllLights[i].bindRShadowMap(GL_TEXTURE0 + i * 2);
    }

    m_meshes[LOD]->render(m_ctx->time(), m_uBoneTransforms, m_uKa, m_uKd, m_uKs, m_uShininess, m_uOpacity);

    for (size_t i = 0; i < MAXTAB; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i * 2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0 + i * 2 + 1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}

void SkinnedObjProgram::shadowRender(const int LOD)
{
    m_meshes[LOD]->render();
}
