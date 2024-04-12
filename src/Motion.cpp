#include "Motion.h"

// class constructor for movement
Motion::Motion(float planeX, float planeY, float planeZ){
    m_planeX = planeZ;
    m_planeY = planeY;
    m_planeZ = planeZ;
}
    

// use deltatime to update position

// eventually, when we have collision component, we will add it and will reverse entity 
// velocity upon collision (bounds of world, or obstacle) and will rotate the model/"flip" the forward vector
void Motion::Update(std::vector<TransformComponent>& entities, float deltaTime) {
    for (auto& entity : entities) {

        // Calculate dot product between forward vector and velocity
        float dotProduct = dot(normalize(entity.forward), entity.velocity);
        
        // Update position based on dot product and velocity
        entity.position += dotProduct * entity.forward * deltaTime;
        
        // TODO add collision component
        // for now, check bounding grid of world
        // if (entity.position.x < 0 || entity.position.x > m_planeX ||
        //     entity.position.y < 0 || entity.position.y > m_planeY ||
        //     entity.position.z < 0 || entity.position.z > m_planeZ) {
        //     // we want to flip the forward direction of the object, and TODO rotate the object
        //     entity.forward *= -1.0;
        // }
    }
}