#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <iostream>
#include <glad/glad.h>
#include <algorithm>


#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "stb_image.h"
using namespace std;
using namespace glm;

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>
#define PI 3.1415927


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