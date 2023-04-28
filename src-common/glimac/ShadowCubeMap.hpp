#ifndef SHADOW_CUBE_MAP_FBO_HPP
#define SHADOW_CUBE_MAP_FBO_HPP

#include <glimac/ShadowFBO.hpp>
#include <glimac/common.hpp>
#include "p6/p6.h"

// cube point light
class ShadowCubeMap : public ShadowFBO {
public:
    ShadowCubeMap();
    virtual ~ShadowCubeMap();
    bool Init(unsigned int WindowSize) override;
    void BindForWriting(unsigned int CubeFace);
    void BindForReading(GLenum TextureUnit) const override;

    inline float getTheta(unsigned int i) { return CameraDirections[i].theta; }
    inline float getPhi(unsigned int i) { return CameraDirections[i].phi; }

private:
    std::vector<glimac::CameraDirection> CameraDirections = {
        {GL_TEXTURE_CUBE_MAP_POSITIVE_X, 180.f, -90.f},
        {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 180.f, 90.f},
        {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 90.f, 180.f},
        {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, -90.f, 180.f},
        {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 180.f, 180.f},
        {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 180.f, 0.f}};
};

#endif