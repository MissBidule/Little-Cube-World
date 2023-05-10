#ifndef OBJ_MANAGER_HPP
#define OBJ_MANAGER_HPP

#include <string>
#include <vector>
#include "LightManager.hpp"
#include "img/src/Image.h"
#include "p6/p6.h"

class ObjectManager {
protected:
    const unsigned int MAXTAB = 3;

    unsigned int m_LODsNB = 0;

    GLint m_uMVPMatrix;
    GLint m_uMVMatrix;
    GLint m_uMMatrix;
    GLint m_uNormalMatrix;

    GLint m_uLightNB;

    std::vector<GLint> m_uLightType;
    std::vector<GLint> m_uLightPos;
    std::vector<GLint> m_uLightPos_vs;
    std::vector<GLint> m_uLightIntensity;
    std::vector<GLint> m_uMVPLight;

    std::vector<GLint> m_uShadowMap;
    std::vector<GLint> m_uShadowMapCube;
    std::vector<GLint> m_ufar_plane;

    std::vector<GLuint> m_VAO;
    std::vector<GLuint> m_VBO;

    std::vector<std::vector<glimac::ShapeVertex>> m_shapes;

    void prerender(const std::vector<LightManager>& AllLights) const;
    void postrender() const;

    ObjectManager(const std::string& vsPath, const std::string& fsPath);

public:
    bool autoplay = true;

    virtual ~ObjectManager() = default;

    bool ignoreShadowRender = false;

    p6::Shader m_Program;
    glm::mat4  m_MMatrix;

    glm::vec3           getPosition(const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix) const;
    inline unsigned int getLODmax() const { return m_LODsNB; }

    void                           clear();
    virtual void                   initVaoVbo();
    virtual std::vector<glm::mat4> getBoneTransforms(int LOD) = 0;
    virtual void                   uniformRender(const std::vector<LightManager>& AllLights, int LOD, const glm::mat4& ViewMatrix, const glm::mat4& ProjMatrix);
    virtual void                   render(const std::vector<LightManager>& AllLights, int LOD) = 0;
    virtual void                   shadowRender(int LOD)                                       = 0;
};

#endif