#include "sphere_vertices.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

namespace glimac {

std::vector<ShapeVertex> quad_vertices(float sideLength) {
    const float halfSide = sideLength / 2.f;

    // Define the 8 corners of the cube
    glm::vec3 topLeftFront(-halfSide, halfSide, halfSide);
    glm::vec3 topRightFront(halfSide, halfSide, halfSide);
    glm::vec3 bottomLeftFront(-halfSide, -halfSide, halfSide);
    glm::vec3 bottomRightFront(halfSide, -halfSide, halfSide);
    glm::vec3 topLeftBack(-halfSide, halfSide, -halfSide);
    glm::vec3 topRightBack(halfSide, halfSide, -halfSide);
    glm::vec3 bottomLeftBack(-halfSide, -halfSide, -halfSide);
    glm::vec3 bottomRightBack(halfSide, -halfSide, -halfSide);

    std::vector<ShapeVertex> vertices;

    // Define the positions, normals and texture coordinates of the 36 vertices of the cube.
    // Each face is composed of two triangles.
    // The normals of each face point outside of the cube.
    // The texture coordinates are arbitrary and will be used to apply a texture to the cube.
    // Front face
    vertices.push_back({topLeftFront, glm::vec3(0.f, 0.f, 1.f), glm::vec2(0.f, 1.f)});
    vertices.push_back({bottomLeftFront, glm::vec3(0.f, 0.f, 1.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({topRightFront, glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f)});

    vertices.push_back({bottomLeftFront, glm::vec3(0.f, 0.f, 1.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({bottomRightFront, glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 0.f)});
    vertices.push_back({topRightFront, glm::vec3(0.f, 0.f, 1.f), glm::vec2(1.f, 1.f)});
    // Back face
    vertices.push_back({topLeftBack, glm::vec3(0.f, 0.f, -1.f), glm::vec2(0.f, 1.f)});
    vertices.push_back({topRightBack, glm::vec3(0.f, 0.f, -1.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({bottomLeftBack, glm::vec3(0.f, 0.f, -1.f), glm::vec2(1.f, 1.f)});

    vertices.push_back({bottomLeftBack, glm::vec3(0.f, 0.f, -1.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({topRightBack, glm::vec3(0.f, 0.f, -1.f), glm::vec2(1.f, 0.f)});
    vertices.push_back({bottomRightBack, glm::vec3(0.f, 0.f, -1.f), glm::vec2(1.f, 1.f)});
    //top Face
    vertices.push_back({topLeftBack, glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 1.f)});
    vertices.push_back({topRightBack, glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({topLeftFront, glm::vec3(1.f, 0.f, 0.f), glm::vec2(1.f, 1.f)});

    vertices.push_back({topRightFront, glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({topLeftFront, glm::vec3(1.f, 0.f, 0.f), glm::vec2(1.f, 0.f)});
    vertices.push_back({topRightBack, glm::vec3(1.f, 0.f, 0.f), glm::vec2(1.f, 1.f)});
    //bottom Face
    vertices.push_back({bottomLeftBack, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(0.f, 1.f)});
    vertices.push_back({bottomRightBack, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({bottomLeftFront, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(1.f, 1.f)});

    vertices.push_back({bottomRightFront, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({bottomLeftFront, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(1.f, 0.f)});
    vertices.push_back({bottomRightBack, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(1.f, 1.f)});
    //left face
    vertices.push_back({bottomLeftBack, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(0.f, 1.f)});
    vertices.push_back({topLeftBack, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({topLeftFront, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(1.f, 1.f)});

    vertices.push_back({bottomLeftFront, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({bottomLeftBack, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(1.f, 0.f)});
    vertices.push_back({topLeftFront, glm::vec3(-1.f, 0.f, 0.f), glm::vec2(1.f, 1.f)});
    //right face
    vertices.push_back({bottomRightBack, glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 1.f)});
    vertices.push_back({topRightBack, glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({topRightFront, glm::vec3(1.f, 0.f, 0.f), glm::vec2(1.f, 1.f)});

    vertices.push_back({bottomRightFront, glm::vec3(1.f, 0.f, 0.f), glm::vec2(0.f, 0.f)});
    vertices.push_back({bottomRightBack, glm::vec3(1.f, 0.f, 0.f), glm::vec2(1.f, 0.f)});
    vertices.push_back({topRightFront, glm::vec3(1.f, 0.f, 0.f), glm::vec2(1.f, 1.f)});

    return vertices;

}

} // namespace glimac
