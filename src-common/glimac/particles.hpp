// LA. -> Lilou ALIDOR
// Dernière version : 22/03/2021

#ifndef PARTICLES_HPP
#define PARTICLES_HPP

#include <glimac/common.hpp>
#include <memory>
#include <vector>
#include "SimpleObjProgram.hpp"
#include "p6/p6.h"

class Particles {
private:
    // LA. Choix du nombre de particules
    static const int MaxParticles = 40;
    // LA. Temps de survie en secondes d'une particule
    const float MaxLife = 1.f;
    // LA. Containeur de particules
    std::vector<glimac::Particle> m_ParticlesContainer;
    int                           m_LastUsedParticle = 0;
    int                           m_nbStades         = 20;

    std::vector<glm::vec3> m_particulePositionData;
    std::vector<glm::vec4> m_particuleColorData;
    std::vector<float>     m_particuleScale;

public:
    std::vector<std::unique_ptr<SimpleObjProgram>> m_meshes;

    Particles();

    int  FindUnusedParticle();
    void SortParticles();

    // LA. On a besoin de la position des particules, de la caméra et du deltatime
    void beginParticles(glm::vec3 position, glm::vec3 cameraPosition, double delta);
};

#endif
