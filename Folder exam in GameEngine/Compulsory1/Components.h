#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <vector>

struct PositionComponent 
{
    glm::vec3 position;
};

struct VelocityComponent 
{
    glm::vec3 velocity;
};

struct RadiusComponent 
{
    float radius;
};

struct BallTrackComponent
{
    std::vector<glm::vec3> track;
};

#endif 

