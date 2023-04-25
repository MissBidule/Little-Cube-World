#ifndef SKINNED_OBJ_PROGRAM_HPP
#define SKINNED_OBJ_PROGRAM_HPP

#include <glimac/SkinnedMesh.hpp>
#include "ObjProgram.hpp"

class SkinnedObjProgram : public ObjProgram {
private:
    GLint m_uKa;
    GLint m_uKd;
    GLint m_uKs;
    GLint m_uShininess;
    GLint m_uOpacity;
    GLint m_uBoneTransforms;

    p6::Context* m_ctx;

    std::vector<std::unique_ptr<SkinnedMesh>> m_meshes;

public:
    SkinnedObjProgram(const std::string& vsPath, const std::string& fsPath, p6::Context& ctx);

    void                   addSkinnedMesh(const std::string& objectPath);
    void                   initVaoVbo() override{};
    std::vector<glm::mat4> getBoneTransforms(int LOD) override;

    void uniformRender(const std::vector<Light>& AllLights, int LOD) override;
    void render(const std::vector<Light>& AllLights, int LOD) override;
    void shadowRender(int LOD) override;
};

#endif