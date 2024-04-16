#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Camera
{
    public:
        Camera(vec3 v, float p, float d, float a, float pp);
        ~Camera();
        // void updateCamera();
        glm::vec3 cameraPos;
        glm::vec3 lookAtPt;
        glm::vec3 upV;
        glm::vec3 view;
        glm::vec3 player_pos;
        float player_rot;
        float pitch;
        float dist;
        float angle;
        float horiz;
        float vert;
        float offX;
        float offZ;

        // float pitch;
        // float yaw;
};


