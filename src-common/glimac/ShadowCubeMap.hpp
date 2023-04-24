#ifndef SHADOW_CUBE_MAP_FBO_HPP
#define SHADOW_CUBE_MAP_FBO_HPP

#include <glimac/ShadowMap.hpp>
#include <glimac/common.hpp>
#include "p6/p6.h"

// cube point light
class ShadowCubeMapFBO : public ShadowMap {
public:
    ShadowCubeMapFBO();
    ~ShadowCubeMapFBO();
    bool Init(unsigned int WindowSize) override;
    void BindForWriting(unsigned int CubeFace);
    void BindForReading(GLenum TextureUnit) const override;
};

#endif