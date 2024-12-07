#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "Particle.h"
#include <vector>
#include "Surface.h"


class ParticleSystem {
public:
    ParticleSystem(int maxParticles);

    void update(float deltaTime, Surface& surface);  // Oppdater partiklene
    void render(Shader& shader, glm::mat4& projection, glm::mat4& view); // Render partiklene
    void emitParticle();  // Slipper ut en ny partikkel

private:
    std::vector<Particle> particles;
    int maxParticles;
};

#endif

