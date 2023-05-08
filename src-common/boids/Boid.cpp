#include "Boid.hpp"
#include "Tools.hpp"


Boid :: Boid(glm::vec3 pos, glm::vec3 maxPos,glm::vec3 minPos) : position(pos),maxPos(maxPos), minPos(minPos)  {

    velocity = glm::vec3(getRandomNumber(0, 0.01f)-0.005f,getRandomNumber(0, 0.01f)-0.005f,getRandomNumber(0, 0.01f)-0.005f);

}

void Boid::setPos(glm::vec3 newPos)
    {

    if(newPos.x > maxPos.x ){

        newPos.x = minPos.x;

    }else if(newPos.x < minPos.x){

        newPos.x = maxPos.x;
    }

    if(newPos.y > maxPos.y){

        newPos.y = minPos.y;

    }else if(newPos.y< minPos.y){

        newPos.y = maxPos.y;

    }if(newPos.z > maxPos.z){

        newPos.z = minPos.z;

    }else if(newPos.z< minPos.z){

        newPos.z = maxPos.z;

    }

    position = newPos;
}

void Boid :: initVaoVbo(){
    
}

void Boid :: render(){
    
}

float Boid::getAngle(){
    return std::atan2(velocity.y, velocity.x) ; // heading angle in x-y plane
}

float Boid :: getPitch(){
    return -std::atan2(velocity.z, std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y)) ; // pitch angle

}
