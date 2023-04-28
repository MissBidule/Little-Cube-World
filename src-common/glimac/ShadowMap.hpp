#ifndef SHADOW_MAP_FBO_HPP
#define SHADOW_MAP_FBO_HPP

#include <glimac/ShadowFBO.hpp>
#include "p6/p6.h"

class ShadowMap : public ShadowFBO {
public:
    ShadowMap();

    ~ShadowMap();

    bool Init(unsigned int WindowSize) override;

    void BindForWriting();

    void BindForReading(GLenum TextureUnit) const override;
};

#endif