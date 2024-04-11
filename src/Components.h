#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent {

    // these attributes will be found through the entity class:
        // entities will have position, velocity, foward direction 

    // the position of each obstacle is randomly generated, the position of the player is set
    glm::vec3 position;
    // the velocity of the obstacles will be a constant speed in the forward direction
    glm::vec3 velocity;
    // a vector to inform which direction the object is facing
    glm::vec3 forward; 
};

#endif