#pragma once
#include "p6/p6.h"
#include <stdlib.h>
#include <cmath>
#include "Tools.hpp"
#include <glm/glm.hpp>
#include "glimac/SkinnedObjectManager.hpp"


class Boid {
private:
    glm::vec3 position;
    glm::vec3 maxPos;
    glm::vec3 minPos;
    float speed = 0.2f;
    glm::vec3 velocity;

public:
    Boid();

    Boid(glm::vec3 pos,glm::vec3 maxPos, glm::vec3 minPos);

    glm::vec3 getPos() const { return position;}

    void setPos(glm::vec3 newPos);

    glm::vec3 getVelocity() const { return velocity;}

    void setVelocity(float xSpeed, float ySpeed, float zSpeed){velocity=glm::vec3(xSpeed,ySpeed,zSpeed);}

    void setVelocity(glm::vec3 newVel){velocity=newVel;}

    float getSpeed() const { return speed; }

    float distanceWithBoid(Boid toCheck)  const ;

    void initVaoVbo();

    void render();

    float getPitch();

    float getAngle();

};

