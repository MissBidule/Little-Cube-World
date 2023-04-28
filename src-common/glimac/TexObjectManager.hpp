#ifndef TEX_OBJ_MANAGER_HPP
#define TEX_OBJ_MANAGER_HPP

#include "ObjectManager.hpp"

class TexObjectManager : public ObjectManager {
private:
    GLint m_uTexKd;
    GLint m_uTexKs;
    GLint m_uShininess;
    GLint m_uOpacity;

    std::vector<glimac::Texture> m_textures;

public:
    TexObjectManager(const std::string& vsPath, const std::string& fsPath);

    void                   addManualTexMesh(const std::vector<glimac::ShapeVertex>& shape, const glimac::Texture& texture);
    void                   initVaoVbo() override { ObjectManager::initVaoVbo(); };
    std::vector<glm::mat4> getBoneTransforms(int LOD) override;

    void uniformRender(const std::vector<LightManager>& AllLights, int LOD, const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix) override;
    void render(const std::vector<LightManager>& AllLights, int LOD) override;
    void shadowRender(int LOD) override;
};

#endif