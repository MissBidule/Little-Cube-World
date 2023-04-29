#pragma once

#include <glm/glm.hpp>
#include "ShadowMap.hpp"
#include "img/src/Image.h"
#include "p6/p6.h"

#define POSITION_LOCATION         0
#define NORMAL_LOCATION           1
#define TEX_COORD_LOCATION        2
#define BONE_ID_LOCATION          3
#define BONE_WEIGHT_LOCATION      4
#define MAX_NUM_BONES_PER_VERTEX  4
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))

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
    Directional = 0,
    Spot,
    Point
};

// STRUCT FOR CUBEMAP
struct CameraDirection {
    GLenum CubemapFace;
    float  theta;
    float  phi;
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

// STRUCT FOR SKINNED MESH
struct VertexBoneData {
    unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = {0};
    float        Weights[MAX_NUM_BONES_PER_VERTEX] = {0.f};

    VertexBoneData() {}

    void AddBoneData(unsigned int BoneID, float Weight)
    {
        for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneIDs); i++)
        {
            if (Weights[i] == 0.0)
            {
                BoneIDs[i] = BoneID;
                Weights[i] = Weight;
                return;
            }
        }
        // Should never get here - more bones than we have space for
        assert(0);
    }
};

struct Particle {
    glm::vec3 position;
    glm::vec3 speed;
    glm::vec4 color;
    float     size;
    float     life;           // Remaining life of the particle. if <0 : dead and unused.
    float     cameraPosition; // distance to the camera. if dead : -1.0f
    bool      operator<(const Particle& that) const
    {
        // Sort in reverse order : far particles drawn first.
        return this->cameraPosition > that.cameraPosition;
    }
    int stade; //(life<0?0:life); (int)(MaxLife-life/MaxLife)*(nbStade-1);
};

} // namespace glimac