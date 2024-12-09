#include "ParticleSystem.h"
#include <cstdlib> 

ParticleSystem::ParticleSystem(int maxParticles)
    : totalAmountOfParticles(maxParticles), howManyParticlesAreActive(0) 
{
    position.resize(maxParticles);
    velocity.resize(maxParticles);
    activeParticles.resize(maxParticles, false);
}

void ParticleSystem::emitter() 
{
    if (howManyParticlesAreActive < totalAmountOfParticles) 
    {
        float x = static_cast<float>(rand()) / RAND_MAX * 3.0f; 
        float y = static_cast<float>(rand()) / RAND_MAX * 2.0f;
        float z = 2.0f; 

        float slowFallSpeed = static_cast<float>(rand()) / RAND_MAX * 0.005f + 0.001f;

        position[howManyParticlesAreActive] = glm::vec3(x, y, z);
        velocity[howManyParticlesAreActive] = glm::vec3(0.0f, 0.0f, -slowFallSpeed);
        activeParticles[howManyParticlesAreActive] = true;

        howManyParticlesAreActive++;
    }
}

void ParticleSystem::updateParticles(float deltaTime, Surface& surface) 
{
    for (int i = 0; i < howManyParticlesAreActive; ++i) 
    {
        if (!activeParticles[i]) continue;

        float u = position[i].x / 3.0f;
        float v = position[i].y / 2.0f;
        glm::vec3 surfacePoint = surface.calculateSurfacePoint(u, v);

        velocity[i].z -= 9.81f * deltaTime;
        position[i] += velocity[i] * deltaTime;

        if (position[i].z <= surfacePoint.y) 
        {
            position[i].z = surfacePoint.y;
            activeParticles[i] = false;
        }
    }
}

void ParticleSystem::renderParticles(Shader& shader, glm::mat4& projection, glm::mat4& view) 
{
    vector<float> vertices;

    for (int i = 0; i < howManyParticlesAreActive; ++i) 
    {
        if (activeParticles[i]) 
        {
            vertices.push_back(position[i].x);
            vertices.push_back(position[i].y);
            vertices.push_back(position[i].z);
        }
    }

    if (vertices.empty()) return;

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
