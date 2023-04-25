#include "SkinnedObjProgram.hpp"
#include <glimac/ObjProgram.hpp>
#include <glimac/SkinnedMesh.hpp>
#include <memory>
#include "glm/gtc/type_ptr.hpp"

SkinnedObjProgram::SkinnedObjProgram(const std::string& vsPath, const std::string& fsPath, p6::Context& ctx)
    : ObjProgram(vsPath, fsPath), m_uKa(glGetUniformLocation(m_Program.id(), "uColor.ka")), m_uKd(glGetUniformLocation(m_Program.id(), "uColor.kd")), m_uKs(glGetUniformLocation(m_Program.id(), "uColor.ks")), m_uShininess(glGetUniformLocation(m_Program.id(), "uColor.shininess")), m_uOpacity(glGetUniformLocation(m_Program.id(), "uColor.opacity")), m_uBoneTransforms(glGetUniformLocation(m_Program.id(), "uBoneTransforms[0]")), m_ctx(&ctx)
{}

std::vector<glm::mat4> SkinnedObjProgram::getBoneTransforms(int LOD)
{
    std::vector<glm::mat4> Transforms;
    m_meshes[LOD]->GetBoneTransforms(m_ctx->time(), Transforms);
    return Transforms;
}

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
    prerender(AllLights);

    m_meshes[LOD]->render(m_ctx->time(), m_uBoneTransforms, m_uKa, m_uKd, m_uKs, m_uShininess, m_uOpacity);

    postrender();
}

void SkinnedObjProgram::shadowRender(const int LOD)
{
    m_meshes[LOD]->render();
}
