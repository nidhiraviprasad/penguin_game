#include "Camera.h"
#include <iostream>
#include <assert.h>
#include "GLSL.h"

using namespace std;
using namespace glm;

Camera::Camera()
{   
    cameraPos = vec3(0.0f, 0.0f, 4.0f);
    lookAtPt = vec3(0.0f, 0.0f, 0.0f);
    upV = vec3(0, 1, 0);
}

Camera::~Camera()
{

}

