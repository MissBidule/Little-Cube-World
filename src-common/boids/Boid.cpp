#include "Boid.hpp"
#include <cmath>
#include "Tools.hpp"

Boid ::Boid(glm::vec3 pos, glm::vec3 maxPos, glm::vec3 minPos)
    : position(pos), maxPos(maxPos), minPos(minPos)
{
    velocity = glm::vec3(getRandomNumber(0, 0.01f) - 0.005f, getRandomNumber(0, 0.01f) - 0.005f, getRandomNumber(0, 0.01f) - 0.005f);
}

void Boid::setPos(glm::vec3 newPos)
{
    // if(newPos.x > maxPos.x ){

    //     newPos.x = minPos.x;

    // }else if(newPos.x < minPos.x){

    //     newPos.x = maxPos.x;
    // }

    // if(newPos.y > maxPos.y){

    //     newPos.y = minPos.y;

    // }else if(newPos.y< minPos.y){

    //     newPos.y = maxPos.y;

    // }if(newPos.z > maxPos.z){

    //     newPos.z = minPos.z;

    // }else if(newPos.z< minPos.z){

    //     newPos.z = maxPos.z;

    // }

    position = newPos;
}

void Boid ::initVaoVbo()
{
}


glm::mat4 Boid::getRotationMatrix() const
{
    glm::vec3 baseVec = glm::vec3(0,-1.0f,0);
    glm::vec3 directionVec   = glm::normalize(velocity);
    glm::vec3 axis  = glm::cross(baseVec, directionVec);
    float     rotationAngle = glm::acos(glm::dot(baseVec, directionVec) / (glm::length(baseVec) * glm::length(directionVec)));
    glm::mat4 rotMatrix = glm::mat4(1.0f);
    
    if (rotationAngle != 0 && !glm::isnan(rotationAngle)){
        rotMatrix = glm::rotate(rotMatrix, rotationAngle, glm::normalize(axis));
    }

    return rotMatrix;
}
