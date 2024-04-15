#include "Shape.h"

class Entity; // forward declaration to enable use of this class in Entity class

class Collider
{

static enum entityType {
    FLOWER,
    TREE,
    BUTTERFLY,
    CAT
};

public:

    void CheckCollision();
    void SetEntity();
    bool IsColliding();
    void ExitCollision();
    float GetRadial();

private:

    Entity entity;
    float radial;
    bool colliding = false;

};