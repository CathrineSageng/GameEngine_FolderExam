#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Surface.h"

using namespace std; 

class ParticleSystem {
public:
    ParticleSystem(int maxParticles);

    void emitter(); // Add new particles
    void updateParticles(float deltaTime, Surface& surface); // Update particle positions
    void renderParticles(Shader& shader, glm::mat4& projection, glm::mat4& view); // Render particles

private:
    int totalAmountOfParticles;

    // Particle data stored in a struct-of-arrays (SoA) format
    vector<glm::vec3> positions; // Positions of particles
    vector<glm::vec3> velocities; // Velocities of particles
    vector<bool> activeParticles; // Active state of each particle

    int howManyParticlesAreActive; // Tracks how many particles are active
};

#endif
