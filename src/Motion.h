#ifndef MOTION_H
#define MOTION_H

#include <vector> 
#include "Components.h"

class Motion {
public:
    // Class constructor for movement
    Motion(float planeX, float planeY, float planeZ);

    // Update position based on velocity and forward vector
    void Update(std::vector<TransformComponent>& entities, float deltaTime);

    float m_planeX;
    float m_planeY;
    float m_planeZ;
};

#endif // MOTION_H
