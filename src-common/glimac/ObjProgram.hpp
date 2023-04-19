#ifndef OBJ_PROGRAM_HPP
#define OBJ_PROGRAM_HPP

#include <glimac/common.hpp>
#include <string>
#include <vector>
#include "img/src/Image.h"
#include "p6/p6.h"

class ObjProgram {
protected:
    const unsigned int MAXTAB = 10;

    unsigned int m_LODsNB = 0;

    GLint m_uMVPMatrix;
    GLint m_uMVMatrix;
    GLint m_uNormalMatrix;

    GLint m_uLightNB;

    std::vector<GLint> m_uLightType;
    std::vector<GLint> m_uLightPos_vs;
    std::vector<GLint> m_uLightIntensity;
    std::vector<GLint> m_uMVPLight;

    std::vector<GLint> m_uShadowMap;

    std::vector<GLuint> m_VAO;
    std::vector<GLuint> m_VBO;

    std::vector<std::vector<glimac::ShapeVertex>> m_shapes;

    ObjProgram(const std::string& vsPath, const std::string& fsPath);

public:
    glm::mat4 m_MMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjMatrix;

    p6::Shader m_Program;

    void         clear();
    virtual void initVaoVbo();
    virtual void uniformRender(const std::vector<glimac::Light>& AllLights, int LOD);
    virtual void render(const std::vector<glimac::Light>& AllLights, int LOD) = 0;
    virtual void shadowRender(int LOD)                                        = 0;
};

#endif