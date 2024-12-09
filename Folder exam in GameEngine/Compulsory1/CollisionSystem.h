#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "Components.h"
#include "Octree.h"
#include <vector>
#include <utility>

class CollisionSystem {
public:
    void handleCollisions(std::vector<PositionComponent>& positions,
        std::vector<VelocityComponent>& velocities,
        const std::vector<RadiusComponent>& radii,
        Octree& octree) {
        std::vector<std::pair<int, int>> potentialCollisions;

        // Extract positions into a temporary vector for Octree processing
        std::vector<glm::vec3> positionVec(positions.size());
        for (size_t i = 0; i < positions.size(); ++i) {
            positionVec[i] = positions[i].position;
        }

        octree.getPotentialCollisions(potentialCollisions, positionVec, radii[0].radius);

        // Handle collisions
        for (const auto& pair : potentialCollisions) {
            int i = pair.first;
            int j = pair.second;

            glm::vec3& posA = positions[i].position;
            glm::vec3& posB = positions[j].position;
            glm::vec3& velA = velocities[i].velocity;
            glm::vec3& velB = velocities[j].velocity;
            float radius = radii[i].radius;

            // Check for collision
            if (glm::distance(posA, posB) < 2.0f * radius) {
                // Resolve collision
                glm::vec3 normal = glm::normalize(posA - posB);

                // Correct positions to resolve overlap
                float overlap = 2.0f * radius - glm::length(posA - posB);
                if (overlap > 0.0f) {
                    glm::vec3 separation = normal * (overlap / 2.0f);
                    posA += separation;
                    posB -= separation;
                }

                // Calculate relative velocity
                glm::vec3 relativeVelocity = velA - velB;
                float velocityAlongNormal = glm::dot(relativeVelocity, normal);

                // Skip if balls are separating
                if (velocityAlongNormal > 0.0f)
                    continue;

                // Calculate impulse scalar
                float e = 1.0f; // Perfectly elastic collision
                float j = -(1 + e) * velocityAlongNormal / 2.0f;

                // Apply impulse
                glm::vec3 impulse = j * normal;
                velA += impulse;
                velB -= impulse;

                // Normalize velocities to maintain correct speed
                velA = glm::normalize(velA) * glm::length(velA);
                velB = glm::normalize(velB) * glm::length(velB);
            }
        }
    }


};
#endif // COLLISIONSYSTEM_H

