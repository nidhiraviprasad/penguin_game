#include "Shape.h"

class Entity; // forward declaration to enable use of this class in Entity class

class Collider
{


public:

    enum entityType {
        FLOWER,
        TREE,
        BUTTERFLY,
        CAT
    };

    Collider();
    Collider(std::vector<std::shared_ptr<Shape>> mesh, entityType type);
    void CheckCollision(std::vector<Entity> entities);
    void SetEntity(Entity e);
    bool IsColliding();
    void ExitCollision();
    float GetRadial();

private:

    Entity* entity;
    float radial;
    bool colliding = false;

};