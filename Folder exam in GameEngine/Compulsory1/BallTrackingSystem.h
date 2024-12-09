#ifndef BALL_TRACKING_SYSTEM_H
#define BALL_TRACKING_SYSTEM_H

#include <vector>
#include "Components.h"
#include <glm/glm.hpp>

class BallTrackingSystem {
public:
    void update(std::vector<PositionComponent>& positions,
        std::vector<BallTrackComponent>& tracks,
        float trackThreshold = 0.01f)
    {
        for (size_t i = 0; i < positions.size(); ++i) 
        {
            glm::vec3& position = positions[i].position;
            BallTrackComponent& track = tracks[i];

            if (track.track.empty() || glm::distance(position, track.track.back()) > trackThreshold) 
            {
                track.track.push_back(position); // Add position to the track
            }
        }
    }
};

#endif

