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
    return std::atan2(velocity.y, velocity.x);
}

float Boid :: getPitch(){
    return glm::degrees(std::atan2(-velocity.z, glm::length(glm::vec2(velocity.x, velocity.y))));

}

float Boid :: getYaw(){

    glm::vec3 normalVelocity = glm::normalize(velocity);
    return std::atan2(normalVelocity.y,normalVelocity.x);

}
