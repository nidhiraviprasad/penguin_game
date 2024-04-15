#include "Collider.h"

Collider::Collider(Shape base, entityType type)
{
    radial = std::sqrt((base->max.x - base->min.x)^2 + (base->max.z - base->max.z)^2);
    if (type == FLOWER) { radial *= 2.5 }; // radial adjustment for flowers
}

void Collider::CheckCollision(Entity[] entities)
{
    for(int i = 0; i < entities.size(); i++){
        float distance = std::sqrt(
            (entities[i].transform.position.x - this.entity.transform.position.x)^2 + 
            (entities[i].transform.position.z - this.entity.transform.position.z)^2
		);
		distance = std::abs(distance);
        if(distance < entity[i].collider.GetRadial() + this.radial){
            // update this to account for butterfly collection
            colliding = true;
            return;
        }
    }
}

void Collider::SetEntity(Entity e){
    this.entity = e;
}

bool Collider::IsColliding(){
    return colliding;
}

void Collider::ExitCollision(){
    this.colliding = false;
}

float Collider::GetRadial(){
    return radial;
}