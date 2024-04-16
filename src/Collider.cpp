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
        adjustment = .15f;
    } else if (type == BUTTERFLY){
        base = mesh[0];
        adjustment = 0.01f;
    } else if (type == CAT){
        base = mesh[0];
        adjustment = .4f;
    }
    radial = std::sqrt((base->max.x - base->min.x) * (base->max.x - base->min.x) + 
                    (base->max.z - base->max.z) * (base->max.z - base->max.z));
    radial *= adjustment;
}

void Collider::CheckCollision(std::vector<Entity> entities, int thisID)
{
    for(int i = 0; i < entities.size(); i++){
        cout << "this id = " << thisID << " and checking id " << entities[i].id << endl;
        cout << "this id = " << thisID << " and checking entities pos " << i << endl;
        if(entities[i].id != thisID) // exclude self when checking collisions
        {
            cout << "id = " << entities[i].id << endl;
            cout << "entity pos x = " << entities[i].position.x << endl;
            cout << "player pos x = " << entities[thisID].position.x << endl;
            cout << "entity pos z = " << entities[i].position.z << endl;
            cout << "player pos z = " << entities[thisID].position.z << endl;


            float distance = std::sqrt(
            (entities[i].position.x - entities[thisID].position.x) * (entities[i].position.x - entities[thisID].position.x) + 
            (entities[i].position.z - entities[thisID].position.z) * (entities[i].position.z - entities[thisID].position.z)
            );
            distance = std::abs(distance);
            cout << "distance is " << distance << endl;
            cout << "radial of other is " << entities[i].collider->GetRadial() << "compared to this radial which is " << entities[thisID].collider->GetRadial() << endl;
            if(distance < entities[i].collider->GetRadial() + entities[thisID].collider->GetRadial()){
                // update this to account for butterfly collection
                colliding = true;
                return;
            }
        } 
    }
}

void Collider::UpdateColliderSize(){

}

void Collider::SetEntityID(int ID){
    entityId = ID;
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