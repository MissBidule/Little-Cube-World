#include "Boid.hpp"
#include <cmath>
#include "Tools.hpp"


Boid :: Boid(glm::vec3 pos, glm::vec3 maxPos,glm::vec3 minPos) : position(pos),maxPos(maxPos), minPos(minPos)  {

    velocity = glm::vec3(getRandomNumber(0, 0.01f)-0.005f,getRandomNumber(0, 0.01f)-0.005f,getRandomNumber(0, 0.01f)-0.005f);

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

void Boid :: initVaoVbo(){
    
}

void Boid :: render(){
    
}

float Boid::getAngle(){
    glm::vec3 normalVelocity = glm::normalize(velocity);
    return std::atan2(normalVelocity.y, normalVelocity.x);
}

float Boid :: getPitch(){
    glm::vec3 normalVelocity = glm::normalize(velocity);
    return glm::degrees(std::atan2(normalVelocity.z, glm::length(glm::vec2(normalVelocity.x, normalVelocity.y))));

}

float Boid :: getYaw(){

    glm::vec3 normalVelocity = glm::normalize(velocity);
    return std::atan2(normalVelocity.y,normalVelocity.x);

}

glm::mat4 Boid::getRotationMatrix() const
{
    glm::vec3 originalVec = glm::vec3(0,1,0);
    glm::vec3 targetVec   = glm::normalize(velocity);

    glm::vec3 axis  = glm::cross(originalVec, targetVec);
    float     angle = glm::acos(glm::dot(originalVec, targetVec) / (glm::length(originalVec) * glm::length(targetVec)));

    glm::mat4 rotMatrix = glm::mat4(1.0f);
    if (angle != 0 && !glm::isnan(angle)){
        rotMatrix = glm::rotate(rotMatrix, angle, glm::normalize(axis));
    }

    return rotMatrix;
}
