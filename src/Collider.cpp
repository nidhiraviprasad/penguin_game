#include "Collider.h"
#include "Entity.h"


Collider::Collider(){};

Collider::Collider(std::vector<std::shared_ptr<Shape>> mesh, entityType type)
{
    shared_ptr<Shape> base;
    float adjustment;
    if (type == FLOWER){        
        base = mesh[2];
        adjustment = 2.5f;
    } else if (type == TREE){
        base = mesh[0];
        adjustment = 1.0f;
    } else if (type == BUTTERFLY){
        base = mesh[0];
        adjustment = 5.0f;
    } else if (type == CAT){
        base = mesh[0];
        adjustment = 5.0f;
    }
    radial = std::sqrt((base->max.x - base->min.x) * (base->max.x - base->min.x) + 
                    (base->max.z - base->max.z) * (base->max.z - base->max.z));
    radial *= adjustment;
}

void Collider::CheckCollision(std::vector<Entity> entities)
{
    for(int i = 0; i < entities.size(); i++){
        if(entities[i].id != this->entity->id) // exclude self when checking collisions
        {
            float distance = std::sqrt(
            (entities[i].position.x - this->entity->position.x) * (entities[i].position.x - this->entity->position.x) + 
            (entities[i].position.z - this->entity->position.z) * (entities[i].position.z - this->entity->position.z)
            );
            distance = std::abs(distance);
            if(distance < entity[i].collider->GetRadial() + this->radial){
                // update this to account for butterfly collection
                colliding = true;
                return;
            }
        }        
    }
}

void Collider::SetEntity(Entity e){
    entity = &e;
}

bool Collider::IsColliding(){
    return colliding;
}

void Collider::ExitCollision(){
    colliding = false;
}

float Collider::GetRadial(){
    return radial;
}