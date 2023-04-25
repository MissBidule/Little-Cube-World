#ifndef TEX_OBJ_PROGRAM_HPP
#define TEX_OBJ_PROGRAM_HPP

#include "ObjProgram.hpp"

class TexObjProgram : public ObjProgram {
private:
    GLint m_uTexKa;
    GLint m_uTexKd;
    GLint m_uTexKs;
    GLint m_uShininess;
    GLint m_uOpacity;

    std::vector<glimac::Texture> m_textures;

public:
    TexObjProgram(const std::string& vsPath, const std::string& fsPath);

    void                   addManualTexMesh(const std::vector<glimac::ShapeVertex>& shape, const glimac::Texture& texture);
    void                   initVaoVbo() override { ObjProgram::initVaoVbo(); };
    std::vector<glm::mat4> getBoneTransforms(int LOD) override;

    void uniformRender(const std::vector<Light>& AllLights, int LOD) override;
    void render(const std::vector<Light>& AllLights, int LOD) override;
    void shadowRender(int LOD) override;
};

#endif