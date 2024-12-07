#include "ParticleSystem.h"
#include <cstdlib> // For random

ParticleSystem::ParticleSystem(int maxParticles) : maxParticles(maxParticles) {
    particles.reserve(maxParticles);
}

void ParticleSystem::emitParticle() {
    if (particles.size() < maxParticles) {
        float x = static_cast<float>(rand()) / RAND_MAX * 4.0f; // Tilfeldig x mellom 0 og 3
        float y = static_cast<float>(rand()) / RAND_MAX * 3.0f; // Tilfeldig z mellom 0 og 2
        float z = 2.0f;
        glm::vec3 position(x, y, z);
        float slowFallSpeed = static_cast<float>(rand()) / RAND_MAX * 0.005f + 0.001f; // Mellom 0.005 og 0.025
        glm::vec3 velocity(0.0f, 0.0f, -slowFallSpeed); // Øk fallhastigheten

        particles.emplace_back(position, velocity);
     
    }
}

void ParticleSystem::update(float deltaTime, Surface& surface) {
    for (auto& particle : particles) {
        // Finn høyden til B-spline overflaten ved partikkelens (x, z)
        float u = particle.position.x / 4.0f; // Skaler x til [0,1]
        float v = particle.position.y / 3.0f; // Skaler z til [0,1]
        glm::vec3 surfacePoint = surface.calculateSurfacePoint(u, v);

        particle.update(deltaTime, surfacePoint.y);
    }
}

void ParticleSystem::render(Shader& shader, glm::mat4& projection, glm::mat4& view) {
    std::vector<float> vertices;

    for (const auto& particle : particles) {
        if (particle.active) {
            vertices.push_back(particle.position.x);
            vertices.push_back(particle.position.y);
            vertices.push_back(particle.position.z);
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
    glBindVertexArray(VAO);

 

    glDrawArrays(GL_POINTS, 0, vertices.size() / 3);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
   

}
