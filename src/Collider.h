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
    
    char entityName;

    Collider();
    Collider(std::vector<std::shared_ptr<Shape>> mesh, entityType type);
    void UpdateColliderSize();
    void CheckCollision(std::vector<Entity> entities, int thisID);
    int CatCollision(std::vector<Entity> entities, Entity *cat);
    void SetEntityID(int ID);
    bool IsColliding();
    void ExitCollision();
    float GetRadial();

private:

    int entityId;
    float radial;
    bool colliding = false;

};