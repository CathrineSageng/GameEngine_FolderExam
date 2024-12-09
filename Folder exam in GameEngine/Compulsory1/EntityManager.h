#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "Entity.h"
#include "Components.h"
#include <vector>

class EntityManager 
{
public:
    vector<Entity> entities;
    vector<PositionComponent> positions;
    vector<VelocityComponent> velocities;
    vector<RadiusComponent> radii;
    vector<BallTrackComponent> tracks;

    size_t createEntity(glm::vec3 position, glm::vec3 velocity, float radius) 
    {
        size_t id = entities.size();
        entities.push_back({ id });

        positions.push_back({ position });
        velocities.push_back({ velocity });
        radii.push_back({ radius });
        tracks.push_back({ std::vector<glm::vec3>() });

        return id;
    }
};

#endif 

