
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Entity.h"
#include "Shape.h"




using namespace std;
using namespace glm;

Entity::Entity(std::vector<std::shared_ptr<Shape>> ref) {
    objs = ref;
    for (int i = 0; i < ref.size(); i++) {
        materials m;
        material.push_back(m);
    }
}


void Entity::setObjs(std::vector<std::shared_ptr<Shape>> ref) {
    
}


void Entity::setMaterials(int i, float r1, float g1, float b1, float r2, float g2, float b2, 
    float r3, float g3, float b3, float s) {
        material[i].matAmb.r = r1;
        material[i].matAmb.g = g1;
        material[i].matAmb.b = b1;
        material[i].matDif.r = r2;
        material[i].matDif.g = g2;
        material[i].matDif.b = b2;
        material[i].matSpec.r = r3;
        material[i].matSpec.g = g3;
        material[i].matSpec.b = b3;
        material[i].matShine = s;
}


