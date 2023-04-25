#include "ObjProgram.hpp"

class SimpleObjProgram : public ObjProgram {
private:
    GLint m_uKa;
    GLint m_uKd;
    GLint m_uKs;
    GLint m_uShininess;
    GLint m_uOpacity;

    std::vector<glimac::Color> m_colors;

public:
    SimpleObjProgram(const std::string& vsPath, const std::string& fsPath);

    void                   addManualMesh(const std::vector<glimac::ShapeVertex>& shape, glimac::Color color);
    void                   initVaoVbo() override { ObjProgram::initVaoVbo(); };
    std::vector<glm::mat4> getBoneTransforms(int LOD) override;

    void uniformRender(const std::vector<Light>& AllLights, int LOD) override;
    void render(const std::vector<Light>& AllLights, int LOD) override;
    void shadowRender(int LOD) override;
};
