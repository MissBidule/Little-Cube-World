#pragma once

#include <glm/glm.hpp>
#include "ShadowMapFBO.hpp"
#include "img/src/Image.h"
#include "p6/p6.h"

namespace glimac {

struct ShapeVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Color {
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;

    float shininess;
    float opacity;
};

struct Texture {
    GLuint ka;
    GLuint kd;
    GLuint ks;

    float shininess;
    float opacity;
};

enum class LightType {
    Directional,
    Spot,
    Point
};

struct Light {
    glm::vec3    position;
    glm::mat4    MMatrix;
    glm::mat4    ViewMatrix;
    glm::mat4    ProjMatrix;
    int          type;
    glm::vec3    color;
    ShadowMapFBO shadowMap;
};

inline GLuint textureToUVtex(img::Image& texture)
{
    // init ONE texture
    GLuint uvtex = 0;
    glGenTextures(1, &uvtex);
    // texture_2D is for simple 2D
    glBindTexture(GL_TEXTURE_2D, uvtex);

    // specify the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data());
    // needed for pixel overlapping (I guess)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // debind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return uvtex;
}

} // namespace glimac