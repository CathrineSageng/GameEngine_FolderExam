#ifndef FRICTIONSYSTEM_H
#define FRICTIONSYSTEM_H

#include "Components.h"

class FrictionSystem {
public:
    void applyFriction(std::vector<VelocityComponent>& velocities,
        const std::vector<PositionComponent>& positions,
        float deltaTime, float normalFriction, float highFriction,
        float frictionAreaXMin, float frictionAreaXMax,
        float frictionAreaYMin, float frictionAreaYMax) {
        const float gravity = 9.81f;  // Gravitational force
        const float minSpeed = 0.5f;  // Prevent balls from fully stopping

        for (size_t i = 0; i < velocities.size(); ++i) {
            float speed = glm::length(velocities[i].velocity);
            glm::vec3 direction = glm::normalize(velocities[i].velocity);

            // Determine if the ball is in the high-friction area
            float frictionCoefficient = normalFriction;
            if (positions[i].position.x >= frictionAreaXMin && positions[i].position.x <= frictionAreaXMax &&
                positions[i].position.y >= frictionAreaYMin && positions[i].position.y <= frictionAreaYMax) {
                frictionCoefficient = highFriction;
            }

            // Compute friction force
            float normalForce = gravity;
            float frictionForce = frictionCoefficient * normalForce;

            // Reduce speed due to friction
            float newSpeed = speed - frictionForce * deltaTime;

            // Ensure the ball doesn't fully stop
            if (newSpeed < minSpeed) {
                newSpeed = minSpeed;
            }

            // Apply the new velocity
            if (speed > 0.0f) { // Avoid NaN errors if direction is zero
                velocities[i].velocity = direction * newSpeed;
            }
        }
    }


};

#endif // FRICTIONSYSTEM_H

