#pragma once

#include <glm/glm.hpp>

class TrackballCamera {
private:
    glm::vec3 m_position;
    float     m_fZoom;
    float     m_fAngleX;
    float     m_fAngleY;

public:
    explicit TrackballCamera(glm::vec3 position = glm::vec3(0.f), float Zoom = 0, float AngleX = 0, float AngleY = 0);

public:
    void moveFront(float delta);
    void moveLeft(float delta);
    void moveUp(float delta);
    void zoom(float delta);
    void setPos(glm::vec3 position);
    void rotateLeft(float degrees);
    void rotateUp(float degrees);

public:
    glm::vec3 getPosition() const;
    glm::mat4 getViewMatrix() const;
    float     getAngleY() const;
    float     getAngleX() const;
};
