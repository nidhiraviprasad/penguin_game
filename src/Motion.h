#ifndef MOTION_H
#define MOTION_H

#include <vector> 
#include "Components.h"

class Motion {
public:
    // Class constructor for movement
    Motion(float planeWidth, float planeHeight);

    // Update position based on velocity and forward vector
    void Update(std::vector<TransformComponent> entities, float deltaTime);

    float m_planeWidth;
    float m_planeHeight;
};

#endif // MOTION_H
