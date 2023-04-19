#ifndef SHADOW_MAP_FBO_HPP
#define SHADOW_MAP_FBO_HPP

#include "p6/p6.h"

class ShadowMapFBO {
public:
    ShadowMapFBO();

    ~ShadowMapFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading(GLenum TextureUnit) const;

private:
    unsigned int m_width     = 0;
    unsigned int m_height    = 0;
    GLuint       m_fbo       = 0;
    GLuint       m_shadowMap = 0;
};

#endif