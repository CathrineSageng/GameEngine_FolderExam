#ifndef POSITIONSYSTEM_H
#define POSITIONSYSTEM_H

#include "Components.h"

class PositionSystem {
public:
    void update(std::vector<PositionComponent>& positions,
        std::vector<VelocityComponent>& velocities,
        float deltaTime, float xMin, float xMax, float yMin, float yMax,
        float ballRadius, Surface& surface) {

        for (size_t i = 0; i < positions.size(); ++i) 
        {
            positions[i].position += velocities[i].velocity * deltaTime;

            if (positions[i].position.x - ballRadius < xMin || positions[i].position.x + ballRadius > xMax) 
            {
                velocities[i].velocity.x = -velocities[i].velocity.x;
                positions[i].position.x = glm::clamp(positions[i].position.x, xMin + ballRadius, xMax - ballRadius);
            }
            if (positions[i].position.y - ballRadius < yMin || positions[i].position.y + ballRadius > yMax) 
            {
                velocities[i].velocity.y = -velocities[i].velocity.y;
                positions[i].position.y = glm::clamp(positions[i].position.y, yMin + ballRadius, yMax - ballRadius);
            }
            float u = (positions[i].position.x - xMin) / (xMax - xMin);
            float v = (positions[i].position.y - yMin) / (yMax - yMin);
            glm::vec3 surfacePoint = surface.calculateSurfacePoint(u, v);
            positions[i].position.z = surfacePoint.z + ballRadius; 
        }
    }
};

#endif 
