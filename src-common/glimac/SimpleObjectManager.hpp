#ifndef SIMPLE_OBJ_MANAGER_HPP
#define SIMPLE_OBJ_MANAGER_HPP

#include "ObjectManager.hpp"

class SimpleObjectManager : public ObjectManager {
private:
    GLint m_uKa;
    GLint m_uKd;
    GLint m_uKs;
    GLint m_uShininess;
    GLint m_uOpacity;

    std::vector<glimac::Color> m_colors;

public:
    SimpleObjectManager(const std::string& vsPath, const std::string& fsPath);

    void                   addManualMesh(const std::vector<glimac::ShapeVertex>& shape, glimac::Color color);
    void                   initVaoVbo() override { ObjectManager::initVaoVbo(); };
    std::vector<glm::mat4> getBoneTransforms(int LOD) override;

    void updateColor(const glimac::Color& newColor, int LOD);

    void uniformRender(const std::vector<LightManager>& AllLights, int LOD, const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix) override;
    void render(const std::vector<LightManager>& AllLights, int LOD) override;
    void shadowRender(int LOD) override;
};

#endif