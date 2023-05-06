// LA. -> Lilou ALIDOR
// Dernière version : 22/03/2021

#ifndef PARTICLES_HPP
#define PARTICLES_HPP

#include <glimac/common.hpp>
#include <memory>
#include <vector>
#include "SimpleObjectManager.hpp"
#include "p6/p6.h"

class ParticleManager {
private:
    // LA. Choix du nombre de particules
    static const int MaxParticles = 30;
    // LA. Temps de survie en secondes d'une particule
    const float MaxLife = 2.f;
    // LA. Containeur de particules
    std::vector<glimac::Particle> m_ParticlesContainer;
    int                           m_LastUsedParticle = 0;
    int                           m_nbStades         = 20;

    std::vector<glm::vec3> m_ParticlePositionData;
    std::vector<glm::vec4> m_ParticleColorData;
    std::vector<float>     m_ParticleScale;

    int  findUnusedParticle();
    void sortParticles();

public:
    std::vector<std::unique_ptr<SimpleObjectManager>> m_meshes;

    ParticleManager();

    // LA. On a besoin de la position des particules, de la caméra et du deltatime
    void refreshParticles(glm::vec3 position, glm::vec3 cameraPosition, double delta);
};

#endif
