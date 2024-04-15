#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Camera
{
    public:
        Camera();
        ~Camera();
        // void updateCamera();
        glm::vec3 cameraPos;
        glm::vec3 lookAtPt;
        glm::vec3 upV;
        // float pitch;
        // float yaw;
};

