#pragma once

#include <glm/glm.hpp>
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"

class FreeflyCamera {
private:
    glm::vec3 m_Position; // cam pos
    float     m_fPhi;
    float     m_fTheta;      // coord spher
    glm::vec3 m_FrontVector; // F
    glm::vec3 m_LeftVector;  // L
    glm::vec3 m_UpVector;    // U

public:
    FreeflyCamera(glm::vec3 Position = glm::vec3(0, 0, 0), float Phi = glm::pi<float>(), float Theta = 0); // call compute
    FreeflyCamera(const FreeflyCamera& camera);

public:
    void computeDirectionVectors(); // calculate F L U
    void moveLeft(float t);         // L
    void moveFront(float t);        // F
    void rotateLeft(float degrees); // Phi
    void rotateUp(float degrees);   // Theta
    void setPos(glm::vec3 position);
    void setPhi(float degrees);
    void setTheta(float degrees);

public:
    glm::mat4 getViewMatrix() const; // glm::lookAt(eye, point, up)
};
