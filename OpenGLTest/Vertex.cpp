#include "Vertex.h"

Vertex::Vertex(vec3 coords)
{
    this->coords = coords;
}

vec3 Vertex::getCoords()
{
    return coords;
}

void Vertex::getCoordsArray(GLfloat* floatArray)
{
    floatArray[0] = coords.x;
    floatArray[1] = coords.y;
    floatArray[2] = coords.z;
}