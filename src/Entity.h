#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Components.h"


typedef struct color_vals{
    float r;
    float g;
    float b;
} color_vals;


typedef struct materials {
    color_vals matAmb;
    color_vals matDif;
    color_vals matSpec;
    float matShine;
} materials;


class Entity {
    public:

        Entity(std::vector<std::shared_ptr<Shape>> ref);

        void setMaterials(int i, float r1, float g1, float b1, float r2, float g2, float b2, 
            float r3, float g3, float b3, float s);

        
        std::vector<std::shared_ptr<Shape>> objs;
        std::vector<materials> material;

};
