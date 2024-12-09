#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include <glm/glm.hpp>
#include "Surface.h"

using namespace std; 

class ParticleSystem {
public:
    ParticleSystem(int maxParticles);

    void emitter();
    void updateParticles(float deltaTime, Surface& surface); 
    void renderParticles(Shader& shader, glm::mat4& projection, glm::mat4& view); 

    int totalAmountOfParticles;

    vector<glm::vec3> position; 
    vector<glm::vec3> velocity;
    vector<bool> activeParticles; 

    int howManyParticlesAreActive; 
};

#endif
