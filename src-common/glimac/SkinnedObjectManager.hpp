#ifndef SKINNED_OBJ_PROGRAM_HPP
#define SKINNED_OBJ_PROGRAM_HPP

#include "ObjectManager.hpp"
#include "SkinnedMesh.hpp"

class SkinnedObjectManager : public ObjectManager {
private:
    GLint m_uKa;
    GLint m_uKd;
    GLint m_uKs;
    GLint m_uShininess;
    GLint m_uOpacity;
    GLint m_uBoneTransforms;
    GLint m_uMovement;

    p6::Context* m_ctx;

    std::vector<std::unique_ptr<SkinnedMesh>> m_meshes;

public:
    SkinnedObjectManager(const std::string& vsPath, const std::string& fsPath, p6::Context& ctx);

    void                   addSkinnedMesh(const std::string& objectPath);
    void                   initVaoVbo() override{};
    std::vector<glm::mat4> getBoneTransforms(int LOD) override;

    void uniformRender(const std::vector<LightManager>& AllLights, int LOD, const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix) override;
    void render(const std::vector<LightManager>& AllLights, int LOD) override;
    void shadowRender(int LOD) override;
};

#endif