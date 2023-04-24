#ifndef SHADOW_MAP_HPP
#define SHADOW_MAP_HPP

#include "p6/p6.h"

class ShadowMap {
public:
    virtual bool Init(unsigned int WindowSize) = 0;

    virtual void BindForReading(GLenum TextureUnit) const = 0;

protected:
    ShadowMap() = default;

    ~ShadowMap()
    {
        glDeleteFramebuffers(1, &m_fbo);
        glDeleteTextures(1, &m_shadowMap);
    };

    unsigned int m_size      = 0;
    GLuint       m_fbo       = 0;
    GLuint       m_shadowMap = 0;
};

#endif