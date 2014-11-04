#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;

class Vertex
{
    vec3 coords;

public:
    Vertex(vec3 coords);

    vec3 getCoords();
    void getCoordsArray(GLfloat* floatArray);
};