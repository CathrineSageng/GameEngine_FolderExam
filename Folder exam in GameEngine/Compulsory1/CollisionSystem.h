#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "Components.h"
#include "Octree.h"
#include <vector>
#include <utility>

class CollisionSystem 
{
    public:
        void whenCollisionHappens(vector<PositionComponent>& positions,
            vector<VelocityComponent>& velocities,
            const vector<RadiusComponent>& radii,
            Octree& octree) 
        {
            vector<pair<int, int>> potentialCollisions;

            vector<glm::vec3> positionVec(positions.size());
            for (size_t i = 0; i < positions.size(); ++i) 
            {
                positionVec[i] = positions[i].position;
            }

            octree.getPotentialCollisions(potentialCollisions, positionVec, radii[0].radius);

            for (const auto& pair : potentialCollisions) 
            {
                int i = pair.first;
                int j = pair.second;

                glm::vec3& p1 = positions[i].position;
                glm::vec3& p2 = positions[j].position;
                glm::vec3& v1 = velocities[i].velocity;
                glm::vec3& v2 = velocities[j].velocity;
                float radius = radii[i].radius;

                if (glm::distance(p1, p2) < 2.0f * radius) 
                {
                    glm::vec3 normal = glm::normalize(p1 - p2);

                    float overlap = 2.0f * radius - glm::length(p1 - p2);
                    if (overlap > 0.0f) 
                    {
                        glm::vec3 separation = normal * (overlap / 2.0f);
                        p1 += separation;
                        p2 -= separation;
                    }

                    glm::vec3 relativeVelocity = v1 - v2;
                    float velocityAlongNormal = glm::dot(relativeVelocity, normal);

                    if (velocityAlongNormal > 0.0f)
                        continue;

                    float e = 1.0f;
                    float j = -(1 + e) * velocityAlongNormal / 2.0f;

                    glm::vec3 impulse = j * normal;
                    v1 += impulse;
                    v2 -= impulse;

                    v1 = glm::normalize(v1) * glm::length(v1);
                    v2 = glm::normalize(v2) * glm::length(v2);

                         /*  cout << "Collision detected between ball " << pair.first << " and ball " << pair.second <<endl;*/
                }
            }
        }
};
#endif // COLLISIONSYSTEM_H

