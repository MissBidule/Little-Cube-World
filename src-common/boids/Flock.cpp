#include "Flock.hpp"
#include "Tools.hpp"
#include <cmath>


Flock::Flock(glm::vec3 maxPos, glm::vec3 minPos, int numberOfBoids, std::string name): maxPos(maxPos), minPos(minPos),numberOfBoids(numberOfBoids), name(name){

    for(int i=0; i<numberOfBoids; i++){
        glm::vec3 RandCoord = glm::vec3(getRandomNumber(minPos.x, maxPos.x),
                                        getRandomNumber(minPos.y, maxPos.y),
                                        getRandomNumber(minPos.z, maxPos.z));
        Boid toAdd = Boid(RandCoord,maxPos, minPos);
        myBoids.push_back(toAdd);
    }

}

void Flock :: limitVelocity (Boid& boid){

    if(glm::length(boid.getVelocity())>speedLimit){
        boid.setVelocity((boid.getVelocity()/glm::length(boid.getVelocity()))*speedLimit);
    }
}

glm::vec3 Flock::separation(Boid& boid) {

    glm::vec3 avoidVector = glm::vec3(0,0,0);

    for(Boid& currentBoid : myBoids){

            if(&currentBoid != &boid && glm::distance(currentBoid.getPos(),boid.getPos())<separationDistance){

                avoidVector -= (currentBoid.getPos() - boid.getPos());
            }
    }
    return avoidVector;
    
}

glm::vec3 Flock :: cohesion(Boid& boid){

    glm::vec3 centerOfMass = glm::vec3(0,0,0);
    
        for(Boid& currentBoid : myBoids){

            if(&currentBoid != &boid && glm::distance(currentBoid.getPos(),boid.getPos())<cohesionDistance){

                centerOfMass+=currentBoid.getPos();
            }

    }

    return (centerOfMass/static_cast<float>(myBoids.size()))/100.0f;
}

glm::vec3 Flock :: alignment(Boid& boid){

    glm::vec3 velocityAverage = glm::vec3(0,0,0);
    
        for(Boid& currentBoid : myBoids){

            if(&currentBoid != &boid && glm::distance(currentBoid.getPos(),boid.getPos())<distanceToFollow){

                velocityAverage+=currentBoid.getVelocity();
            }

    }

    return (velocityAverage/static_cast<float>(myBoids.size()))/8.0f;
}

glm::vec3 Flock :: boundaries(Boid& boid) {

    glm::vec3 v;
    if(boid.getPos().x < minPos.x){
        v.x = BoundaryWallValue;
    }else if(boid.getPos().x > maxPos.x){
        v.x = -BoundaryWallValue;
    }
    if(boid.getPos().y < minPos.y){
        v.y = BoundaryWallValue;
    }else if(boid.getPos().y > maxPos.y){
        v.y = -BoundaryWallValue;
    }
    if(boid.getPos().z < minPos.z){
        v.z = BoundaryWallValue;
    }else if(boid.getPos().z > maxPos.z){
        v.z = -BoundaryWallValue;
    }
    return v;
}

void Flock :: simulate(){

    if(!isPaused){

        for(Boid& currentBoid : myBoids){

            glm::vec3 cohesionValue = cohesionCoeff*cohesion(currentBoid);
            glm::vec3 separationValue = separationCoeff*separation(currentBoid);
            glm::vec3 alignmentValue = alignmentCoeff*alignment(currentBoid);
            glm::vec3 boundariesValue = boundaryCoeff*boundaries(currentBoid);

            currentBoid.setVelocity(currentBoid.getVelocity()+boundariesValue+separationValue+alignmentValue+cohesionValue);
            limitVelocity(currentBoid);
            currentBoid.setPos(currentBoid.getPos()+currentBoid.getVelocity()) ;
        }

    }
}

void Flock::displayParam() {
    ImGui::Begin(("Flock settings "+name).c_str());
    ImGui::Text("maxSpeed");
    ImGui::SliderFloat("Max speed", &speedLimit, .001f, 10.f);
    ImGui::SliderFloat("Distance see for alignment", &distanceToFollow, .01f, 10);
    ImGui::SliderFloat("Distance see for cohesion", &cohesionDistance, .01f, 10);
    ImGui::SliderFloat("Distance see for separation", &separationDistance, .01f, 10);

    ImGui::SliderFloat("coefficient for alignment force", &alignmentCoeff, .1f, 10);
    ImGui::SliderFloat("coefficient for cohesion force", &cohesionCoeff, .1f, 10);
    ImGui::SliderFloat("coefficient for separation force", &separationCoeff, .1f, 10);
    ImGui::SliderFloat("coefficient for boundary force", &boundaryCoeff, .1f, 10);
    ImGui::Checkbox("Pause", &isPaused);

    ImGui::End();
}

void Flock :: initVaoVbo() {
    
    for(Boid current : myBoids){
        current.initVaoVbo();
    }   
}
