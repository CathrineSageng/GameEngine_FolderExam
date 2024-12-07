#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

class Particle {
public:
    glm::vec3 position; // Partikkelens posisjon
    glm::vec3 velocity; // Partikkelens hastighet
    bool active;        // Om partiklen er aktiv (ikke "smeltet" eller stoppet)

    Particle(glm::vec3 startPos, glm::vec3 startVelocity)
        : position(startPos), velocity(startVelocity), active(true) {}

    void update(float deltaTime, float groundHeight); // Oppdaterer partikkelens posisjon
};

#endif

