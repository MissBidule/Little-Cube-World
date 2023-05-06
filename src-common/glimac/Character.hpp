#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <glimac/LightManager.hpp>
#include <glimac/SkinnedObjectManager.hpp>
#include <glimac/TrackballCamera.hpp>
#include <vector>
#include "glm/fwd.hpp"

#define off false
#define on  true

class Character {
private:
    TrackballCamera                                    m_POV;
    std::vector<std::unique_ptr<SkinnedObjectManager>> m_characterMesh;
    unsigned int                                       m_chosenMesh    = 0;
    float                                              m_angleMovement = 0;
    float                                              m_zoom          = 0;
    LightManager*                                      m_light         = nullptr;
    glm::vec3                                          m_lightColor    = glm::vec3(0);

public:
    bool m_switch   = off;
    bool m_isMoving = false;
    explicit Character(glm::vec3 position = glm::vec3(0));

    glm::mat4              getViewMatrix();
    void                   setLight(LightManager& Light);
    void                   setCharacter(unsigned int chosenMesh);
    void                   setPosition(glm::vec3 position);
    glm::vec3              getPosition();
    glm::mat4              getMMatrix();
    std::vector<glm::mat4> getBoneTransforms();
    void                   zoom(float delta);
    void                   moveFront(float delta);
    void                   moveLeft(float delta);
    void                   rotateUp(float degrees);
    void                   rotateLeft(float degrees);

public:
    void createCharacter(const std::string& objectPath, p6::Context& ctx);
    void use();
    void initVaoVbo();
    void updatePosition();
    void uniformRender(const std::vector<LightManager>& AllLights, glm::mat4& ProjMatrix);
    void render(const std::vector<LightManager>& AllLights);
    void shadowRender();

private:
    bool checkPosition();

    const int               m_width  = 25;
    const int               m_height = 25;
    const std::vector<bool> m_allowedPosition =
        {
            false, true, true, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false,                  // z = 0
            true, false, true, true, true, true, true, true, false, true, true, true, true, false, false, true, true, true, false, false, false, false, false, false, false,               // z = 1
            true, true, true, true, true, false, true, true, true, true, true, true, true, false, false, true, true, true, true, false, false, false, false, false, false,                 // z = 2
            true, true, true, true, true, true, true, false, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false,                   // z = 3
            true, true, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, true, false, false, false,                    // z = 4
            true, true, true, true, true, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false,                       // z = 5
            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,                          // z = 6
            true, false, true, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false,                   // z = 7
            true, true, true, true, true, true, true, true, true, true, true, true, true, false, true, false, false, false, false, false, false, false, false, false, false,               // z = 8
            true, true, true, true, true, true, true, true, true, true, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false,             // z = 9
            true, true, true, true, true, false, false, false, false, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false,        // z = 10
            false, false, false, false, false, false, false, false, false, false, false, true, true, false, false, false, false, false, false, false, true, true, true, true, true,        // z = 11
            false, false, false, false, false, false, false, false, false, false, false, true, true, false, false, true, true, true, false, false, false, false, false, false, true,       // z = 12
            false, false, false, false, false, false, false, false, false, false, false, true, true, false, true, true, true, true, false, false, false, false, false, false, true,        // z = 13
            false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, true,               // z = 14
            true, true, true, true, true, true, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,                        // z = 15
            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,                          // z = 16
            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,                          // z = 17
            false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, false, false, false, false, false, false, false, false, false,    // z = 18
            false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false,                  // z = 19
            false, false, true, true, true, true, false, false, false, true, true, true, false, false, false, false, false, true, true, false, false, false, false, false, false,          // z = 20
            false, false, true, true, true, true, false, false, false, true, true, true, false, false, false, false, false, true, true, false, false, false, false, false, false,          // z = 21
            false, false, true, true, true, true, false, false, false, true, true, true, false, false, false, false, false, true, true, false, false, false, false, false, false,          // z = 22
            false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false,                  // z = 23
            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, // z = 24
    };
};

#endif