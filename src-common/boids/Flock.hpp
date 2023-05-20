#pragma once
#include "Boid.hpp"
#include <vector>
#include "p6/p6.h"
#include "Tools.hpp"
#include <glm/glm.hpp>
#include "imgui.h"


class Flock{

    public:
        Flock();
        Flock(glm::vec3 maxPos, glm::vec3 minPos, int numberOfBoids, std::string name);
        std::vector<Boid> getMyBoids() const { return myBoids; }
        glm::vec3 alignment(Boid& boid);
        void simulate();
        glm::vec3 separation(Boid& boid);
        glm::vec3 cohesion(Boid& boid);
        glm::vec3 boundaries(Boid& boid);
        void limitVelocity (Boid& boid);
        void displayParam();
        void initVaoVbo();
        std::vector<Boid> myBoids;


    private:
        glm::vec3 maxPos;
        glm::vec3 minPos;
        float distanceToFollow = 5.f;
        float cohesionDistance = 7.f;
        const int numberOfBoids;
        float separationDistance = 0.8f;
        float speedLimit = 0.3f;
        glm::vec3 boundaryVecMax = glm::vec3(2.f,2.f,2.f);
        glm::vec3 boundaryVecMin = glm::vec3(-2.f,-2.f,-2.f);
        float BoundaryWallValue = 10.f;
        bool isPaused = false;
        float cohesionCoeff = 0.8f;
        float separationCoeff = 1.0f;
        float alignmentCoeff = 5.0f;
        float boundaryCoeff = 6.0f;
        std::string name;


};