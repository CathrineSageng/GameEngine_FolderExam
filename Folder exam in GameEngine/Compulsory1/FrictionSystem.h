#ifndef FRICTIONSYSTEM_H
#define FRICTIONSYSTEM_H

#include "Components.h"

class FrictionSystem 
{
    public:
        void applyFriction(vector<VelocityComponent>& velocities,
            const vector<PositionComponent>& positions,
            float deltaTime, float normalFriction, float highFriction,
            float frictionAreaXMin, float frictionAreaXMax,
            float frictionAreaYMin, float frictionAreaYMax) 
            {
                const float gravity = 9.81f;
                  
                for (size_t i = 0; i < velocities.size(); ++i) 
                {
                    float speed = glm::length(velocities[i].velocity);
                    glm::vec3 direction = glm::normalize(velocities[i].velocity);

                    float frictionCoefficient = normalFriction;
                    if (positions[i].position.x >= frictionAreaXMin && positions[i].position.x <= frictionAreaXMax &&
                        positions[i].position.y >= frictionAreaYMin && positions[i].position.y <= frictionAreaYMax) 
                    {
                        frictionCoefficient = highFriction;
                    }

                    float normalForce = gravity;
                    float frictionForce = frictionCoefficient * normalForce;

                    float newSpeed = speed - frictionForce * deltaTime;

                    const float minSpeed = 0.5f;
                    if (newSpeed < minSpeed) 
                    {
                        newSpeed = minSpeed;
                    }

                    if (speed > 0.0f)
                    { 
                        velocities[i].velocity = direction * newSpeed;
                    }
                }
        }
};

#endif // FRICTIONSYSTEM_H

