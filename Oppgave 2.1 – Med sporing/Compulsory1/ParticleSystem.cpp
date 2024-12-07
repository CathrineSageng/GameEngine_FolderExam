#include "ParticleSystem.h"
#include <cstdlib> 

ParticleSystem::ParticleSystem(int maxParticles)
    : totalAmountOfParticles(maxParticles), howManyParticlesAreActive(0) 
{
    positions.resize(maxParticles);
    velocities.resize(maxParticles);
    activeParticles.resize(maxParticles, false);
}

// Emit a new particle if the system has room
void ParticleSystem::emitter() 
{
    if (howManyParticlesAreActive < totalAmountOfParticles) 
    {
        // Randomize position within a defined range
        float x = static_cast<float>(rand()) / RAND_MAX * 3.0f; // x range: [0, 3]
        float y = static_cast<float>(rand()) / RAND_MAX * 2.0f; // y range: [0, 2]
        float z = 2.0f; // Start at a fixed height

        // Randomize slow falling velocity
        float slowFallSpeed = static_cast<float>(rand()) / RAND_MAX * 0.005f + 0.001f;

        positions[howManyParticlesAreActive] = glm::vec3(x, y, z);
        velocities[howManyParticlesAreActive] = glm::vec3(0.0f, 0.0f, -slowFallSpeed);
        activeParticles[howManyParticlesAreActive] = true;

        howManyParticlesAreActive++;
    }
}

// Update particle positions and deactivate if they reach the surface
void ParticleSystem::updateParticles(float deltaTime, Surface& surface) 
{
    for (int i = 0; i < howManyParticlesAreActive; ++i) 
    {
        if (!activeParticles[i]) continue;

        // Calculate surface height at particle's x, y position
        float u = positions[i].x / 3.0f;
        float v = positions[i].y / 2.0f;
        glm::vec3 surfacePoint = surface.calculateSurfacePoint(u, v);

        // Apply gravity and update position
        velocities[i].z -= 9.81f * deltaTime;
        positions[i] += velocities[i] * deltaTime;

        // Deactivate if the particle hits the ground
        if (positions[i].z <= surfacePoint.y) 
        {
            positions[i].z = surfacePoint.y;
            activeParticles[i] = false;
        }
    }
}

// Render all active particles as points
void ParticleSystem::renderParticles(Shader& shader, glm::mat4& projection, glm::mat4& view) 
{
    vector<float> vertices; // Collect active particle positions

    for (int i = 0; i < howManyParticlesAreActive; ++i) 
    {
        if (activeParticles[i]) 
        {
            vertices.push_back(positions[i].x);
            vertices.push_back(positions[i].y);
            vertices.push_back(positions[i].z);
        }
    }

    if (vertices.empty()) return;

    // Send the particle positions to the GPU
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    glPointSize(5.0f);
    glDrawArrays(GL_POINTS, 0, vertices.size() / 3);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
