// LA. -> Lilou ALIDOR
// Dernière version : 22/03/2021

#include "particles.hpp"
#include <glimac/common.hpp>
#include <glimac/sphere_vertices.hpp>
#include <memory>
#include <vector>
#include "glm/gtx/transform.hpp"

Particles::Particles()
{
    for (int i = 0; i < MaxParticles; i++)
    {
        m_ParticlesContainer.emplace_back();
        m_particulePositionData.emplace_back(0);
        m_particuleColorData.emplace_back(0);
        m_particuleScale.emplace_back(0);
        m_meshes.emplace_back(std::make_unique<SimpleObjProgram>("shaders/particle.vs.glsl", "shaders/particle.fs.glsl"));

        // LA. Couleur des particules (avec opacité)
        m_ParticlesContainer[i].color = glm::vec4(0.5f, 0.5f, 0.5f, 0.8f);

        glimac::Color color = {
            glm::vec3(m_ParticlesContainer[i].color),
            glm::vec3(0),
            glm::vec3(0),
            0,
            m_ParticlesContainer[i].color[3]};

        // Meshes init (no update here)
        //  to be changed with the quad afterwards
        m_meshes[i]->addManualMesh(glimac::sphere_vertices(1.f, 32, 16), color);

        m_ParticlesContainer[i].life           = -1.0f;
        m_ParticlesContainer[i].cameradistance = -1.0f;
    }
}

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int Particles::FindUnusedParticle()
{
    for (int i = m_LastUsedParticle; i < MaxParticles; i++)
    {
        if (m_ParticlesContainer[i].life < 0)
        {
            m_LastUsedParticle = i;
            return i;
        }
    }

    for (int i = 0; i < m_LastUsedParticle; i++)
    {
        if (m_ParticlesContainer[i].life < 0)
        {
            m_LastUsedParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

// LA. Tri des particules, important pour le renouvellement de ces dernières
void Particles::SortParticles()
{
    std::sort(&m_ParticlesContainer[0], &m_ParticlesContainer[MaxParticles]);
}

// LA. Création et actualisation des particules
void Particles::beginParticles(glm::vec3 position, glm::vec3 cameraPosition, double delta)
{
    // LA. Renouvellement régulier mais limité de particules
    int newparticles = (int)(delta * ((float)MaxParticles / MaxLife));
    if (newparticles > (int)(0.016f * 10000.0))
        newparticles = (int)(0.016f * 10000.0);

    for (int i = 0; i < newparticles; i++)
    {
        int particleIndex                        = FindUnusedParticle();
        m_ParticlesContainer[particleIndex].life = MaxLife; // This particle will live MaxLife seconds.
        // LA. Position de départ de la particule
        m_ParticlesContainer[particleIndex].position = position;

        // LA. Force de propagation des particules
        float spread = 4.f;
        // LA. Direction suivie par les particules avant de retomber
        glm::vec3 maindir = glm::vec3(0.0f, 10.f, 0.3f);
        // LA. Position aléatoire des particules par rapport à la trajectoire des particules
        glm::vec3 randomdir = glm::vec3(
            -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2.0f))),
            -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2.0f))),
            -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2.0f)))
        );

        m_ParticlesContainer[particleIndex].speed = maindir + randomdir * spread;

        // LA. Angle aléatoire des particules en z
        // m_ParticlesContainer[particleIndex].angle = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (180.0f)));

        // LA. Taille des particules
        m_ParticlesContainer[particleIndex].size = static_cast<float>(rand()) / static_cast<float>(RAND_MAX * (4.0f));
    }

    // Simulate all particles
    int ParticlesCount = 0;
    for (int i = 0; i < MaxParticles; i++)
    {
        glimac::Particle& p = m_ParticlesContainer[i]; // shortcut

        if (p.life > 0.0f)
        {
            //  Decrease life
            p.life -= delta;
            if (p.life > 0.0f)
            {
                // LA. Calcul du stade lié à l'âge de la particule
                int temp = ((MaxLife - p.life) / MaxLife) * (m_nbStades - 1);

                // LA. À cause de malloc je prends mes précautions et limite le résultat
                p.stade   = (temp > m_nbStades - 1 ? m_nbStades - 1 : temp);
                p.color.a = 1 - (p.stade / static_cast<float>(m_nbStades));

                // Simulate simple physics : gravity only, no collisions
                p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
                float speedFactor = 0.15f;
                p.position += p.speed * (float)delta * speedFactor;
                p.cameradistance = pow(distance(p.position, cameraPosition), 2);
            }
            else
            {
                // Particles that just died will be put at the end of the buffer in SortParticles();
                p.cameradistance = -1.0f;
                // LA. Même si elles mettent longtemps avant de mourir les particules "mortent" n'apparaissent pas
                p.stade   = m_nbStades;
                p.color.a = 0;
            }

            ParticlesCount++;
        }
        else
        {
            // LA. La particule devrait être morte
            p.stade   = m_nbStades;
            p.color.a = 0;
        }

        // Fill the GPU buffer
        m_particulePositionData[ParticlesCount] = glm::vec3(p.position);
        m_particuleScale[ParticlesCount]        = p.size;
        m_particuleColorData[ParticlesCount]    = glm::vec4(p.color);

        glm::mat4 MMatrix = glm::translate(glm::mat4(1), m_particulePositionData[ParticlesCount]);
        MMatrix           = glm::scale(MMatrix, glm::vec3(m_particuleScale[ParticlesCount]));

        m_meshes[i]->m_MMatrix = MMatrix;
        glimac::Color color    = {
            glm::vec3(m_ParticlesContainer[i].color),
            glm::vec3(0),
            glm::vec3(0),
            0,
            m_ParticlesContainer[i].color[3]};
        m_meshes[i]->updateColor(color, 0);
    }

    SortParticles();
}