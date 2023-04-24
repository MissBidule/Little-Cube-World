#include "ShadowCubeMap.hpp"

ShadowCubeMapFBO::ShadowCubeMapFBO() = default;

ShadowCubeMapFBO::~ShadowCubeMapFBO() = default;

bool ShadowCubeMapFBO::Init(unsigned int size)
{
    m_size = size;

    // create the cube map
    glGenTextures(1, &m_shadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (size_t i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_size, m_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    // Create the FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap, 0);

    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "FB error status : " << Status << std::endl;
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return true;
}

std::vector<glimac::CameraDirection> CameraDirections = {
    {GL_TEXTURE_CUBE_MAP_POSITIVE_X, 180.f, -90.f},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 180.f, 90.f},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 90.f, 180.f},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, -90.f, 0.f},
    {GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 180.f, 180.f},
    {GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 180.f, 0.f}};

void ShadowCubeMapFBO::BindForWriting(unsigned int CubeFace)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_size, m_size);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CameraDirections[CubeFace].CubemapFace, m_shadowMap, 0);
    // glDrawBuffer(GL_DEPTH_ATTACHMENT);
}

void ShadowCubeMapFBO::BindForReading(GLenum TextureUnit) const
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);
}
