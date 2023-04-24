#ifndef SHADOW_MAP_FBO_HPP
#define SHADOW_MAP_FBO_HPP

#include <glimac/ShadowMap.hpp>
#include "p6/p6.h"

class ShadowMapFBO : public ShadowMap {
public:
    ShadowMapFBO();

    ~ShadowMapFBO();

    bool Init(unsigned int WindowSize) override;

    void BindForWriting();

    void BindForReading(GLenum TextureUnit) const override;
};

#endif