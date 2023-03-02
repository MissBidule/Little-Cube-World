#pragma once

#include <glm/glm.hpp>

class TrackballCamera {
private:
    float m_fDistance;
    float m_fAngleX;
    float m_fAngleY;

public:
    explicit TrackballCamera(float Distance = 5.f, float AngleX = 0, float AngleY = 0);

public:
    void moveFront(float delta);
    void rotateLeft(float degrees);
    void rotateUp(float degrees);

public:
    glm::mat4 getViewMatrix() const;
};
