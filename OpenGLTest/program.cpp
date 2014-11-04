#define GLEW_STATIC

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <algorithm>
#include <ctime>

#include <GL/glew.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp";

#include "Vertex.h"

using namespace glm;

GLFWwindow* window;

int GetPyramidArraySize();
GLfloat* CreatePyramid();

int GetCubeArraySize();
GLfloat* CreateCube();

int GetSphereArraySize(int meridians, int paralells);
GLfloat* CreateSphere(float radius, int meridians, int paralells, vec3 baseColor, float minColorValue);

vec3 GetRandomColor(vec3 baseColor, float minValue);

int main()
{
    srand(time(NULL));

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int screenWidth = 800;
    int screenHeight = 800;
    float aspect = screenWidth / screenHeight;
    // Open a window and create its OpenGL context
    window = glfwCreateWindow(screenWidth, screenHeight, "Playground", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        _getch();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.39f, 0.58f, 0.93f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    mat4 projection = perspective(60.0f, aspect, 0.1f, 100.0f);

    GLuint programId = LoadShaders("shaders/simple.vs", "shaders/simple.fs");
    GLuint colorProgramId = LoadShaders("shaders/color.vs", "shaders/color.fs");

    GLuint uMVP = glGetUniformLocation(programId, "uMVP");
    GLuint uMV = glGetUniformLocation(programId, "uMV");
    GLuint uNormalMatrix = glGetUniformLocation(programId, "uNormalMatrix");

    GLuint pyramidBufferId;
    glGenBuffers(1, &pyramidBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidBufferId);
    GLfloat* pyramid = CreatePyramid();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * GetPyramidArraySize(), pyramid, GL_STATIC_DRAW);
    delete[] pyramid;

    GLuint cubeBufferId;
    glGenBuffers(1, &cubeBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, cubeBufferId);
    GLfloat* cube = CreateCube();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * GetCubeArraySize(), cube, GL_STATIC_DRAW);
    delete[] cube;

    float radius = 0.7;
    int meridians = 64;
    int paralells = 32;
    GLuint sphereBufferId;
    glGenBuffers(1, &sphereBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, sphereBufferId);
    GLfloat* sphere = CreateSphere(radius, meridians, paralells, vec3(0, 0.3, 1), 0.5f);
    int sphereArraySize = GetSphereArraySize(meridians, paralells);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sphereArraySize, sphere, GL_STATIC_DRAW);
    delete[] sphere;

    GLuint pyramidVertexArrayId;
    glGenVertexArrays(1, &pyramidVertexArrayId);

    GLuint cubeVertexArrayId;
    glGenVertexArrays(1, &cubeVertexArrayId);

    GLuint sphereVertexArrayId;
    glGenVertexArrays(1, &sphereVertexArrayId);

    float phi = 330;
    float psi = -30;

    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        vec4 lookFrom = vec4(0, 0, 6, 1);
        mat4 rotX = mat4(1);
        rotX = rotate(rotX, psi, vec3(1, 0, 0));
        mat4 rotY = mat4(1);
        rotY = rotate(rotY, phi, vec3(0, 1, 0));
        lookFrom = rotX * lookFrom;
        lookFrom = rotY * lookFrom;
        vec3 lookFrom3 = lookFrom.swizzle(X, Y, Z);
        mat4 view = lookAt(
            lookFrom3,
            vec3(0, 0, 0),
            vec3(0, 1, 0));

        // PYRAMID
        glUseProgram(programId);

        mat4 pyramidModel = mat4(1);
        pyramidModel = translate(pyramidModel, vec3(-2, 0, 0));
        mat4 pyramidMV = view * pyramidModel;
        mat4 pyramidNormalMatrix = transpose(inverse(pyramidMV));
        mat4 pyramidMVP = projection * pyramidMV;

        glUniformMatrix4fv(uMVP, 1, GL_FALSE, &pyramidMVP[0][0]);
        glUniformMatrix4fv(uMV, 1, GL_FALSE, &pyramidMV[0][0]);
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, &pyramidNormalMatrix[0][0]);

        glBindVertexArray(pyramidVertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, pyramidBufferId);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * 4,
            (void*)0
            );

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * 4,
            (void*)(3 * 4)
            );

        glDrawArrays(GL_TRIANGLES, 0, GetPyramidArraySize() / 6);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // CUBE

        mat4 cubeModel = mat4(1);
        mat4 cubeMV = view * cubeModel;
        mat4 cubeNormalMatrix = transpose(inverse(cubeMV));
        mat4 cubeMVP = projection * cubeMV;

        glUniformMatrix4fv(uMVP, 1, GL_FALSE, &cubeMVP[0][0]);
        glUniformMatrix4fv(uMV, 1, GL_FALSE, &cubeMV[0][0]);
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, &cubeNormalMatrix[0][0]);

        glBindVertexArray(cubeVertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, cubeBufferId);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * 4,
            (void*)0
            );

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * 4,
            (void*)(3 * 4)
            );

        glDrawArrays(GL_TRIANGLES, 0, GetCubeArraySize() / 6);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // SPHERE
        glUseProgram(colorProgramId);

        mat4 sphereModel = mat4(1);
        sphereModel = translate(sphereModel, vec3(2, 0, 0));
        mat4 sphereMV = view * sphereModel;
        mat4 sphereNormalMatrix = transpose(inverse(sphereMV));
        mat4 sphereMVP = projection * sphereMV;

        glUniformMatrix4fv(uMVP, 1, GL_FALSE, &sphereMVP[0][0]);
        glUniformMatrix4fv(uMV, 1, GL_FALSE, &sphereMV[0][0]);
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE, &sphereNormalMatrix[0][0]);

        glBindVertexArray(sphereVertexArrayId);
        glBindBuffer(GL_ARRAY_BUFFER, sphereBufferId);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            (3 + 3 + 3) * 4,
            (void*)0
            );

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            (3 + 3 + 3) * 4,
            (void*)(3 * 4)
            );

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,
            3,
            GL_FLOAT,
            GL_FALSE,
            (3 + 3 + 3) * 4,
            (void*)((3 + 3) * 4)
            );

        glDrawArrays(GL_TRIANGLES, 0, sphereArraySize / 9);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // OTHER

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) phi += 0.2;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) phi -= 0.2;
        if (phi > 360) phi -= 360;
        if (phi < 0) phi += 360;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) psi -= 0.2;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) psi += 0.2;
        if (psi > 89) psi = 89;
        if (psi < -89) psi = -89;


    }
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);

    glDeleteVertexArrays(1, &cubeVertexArrayId);
    glDeleteBuffers(1, &cubeBufferId);

    glDeleteVertexArrays(1, &sphereVertexArrayId);
    glDeleteBuffers(1, &sphereBufferId);

    glDeleteProgram(programId);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

int GetPyramidArraySize()
{
    return (3 + 3) * 3 * 8;
}

GLfloat* CreatePyramid()
{
    int size = GetPyramidArraySize();
    GLfloat* pyramid = new GLfloat[size];

    GLfloat pyramidStatic[] =
    {
        -0.5, -0.3, -0.5, +0, +0, -1,
        +0.0, -0.3, +0.0, +0, +0, -1,
        -0.5, -0.3, +0.5, +0, +0, -1,

        -0.5, -0.3, +0.5, +0, +0, -1,
        +0.0, -0.3, +0.0, +0, +0, -1,
        +0.5, -0.3, +0.5, +0, +0, -1,

        +0.5, -0.3, +0.5, +0, +0, -1,
        +0.0, -0.3, +0.0, +0, +0, -1,
        +0.5, -0.3, -0.5, +0, +0, -1,

        +0.5, -0.3, -0.5, +0, +0, -1,
        +0.0, -0.3, +0.0, +0, +0, -1,
        -0.5, -0.3, -0.5, +0, +0, -1,

        -0.5, -0.3, -0.5, -0.5, +1, 0,
        -0.5, -0.3, +0.5, -0.5, +1, 0,
        +0.0, +0.7, +0.0, -0.5, +1, 0,

        -0.5, -0.3, +0.5, +0, +1, +0.5,
        +0.5, -0.3, +0.5, +0, +1, +0.5,
        +0.0, +0.7, +0.0, +0, +1, +0.5,

        +0.5, -0.3, +0.5, +0.5, +1, 0,
        +0.5, -0.3, -0.5, +0.5, +1, 0,
        +0.0, +0.7, +0.0, +0.5, +1, 0,

        +0.5, -0.3, -0.5, +0, +1, -0.5,
        -0.5, -0.3, -0.5, +0, +1, -0.5,
        +0.0, +0.7, +0.0, +0, +1, -0.5,
    };

    for (int i = 0; i < size; i++)
    {
        pyramid[i] = pyramidStatic[i];
    }

    return pyramid;
}

int GetCubeArraySize()
{
    return (3 + 3) * 2 * 3 * 6;
}

GLfloat* CreateCube()
{
    int size = GetCubeArraySize();
    GLfloat* cube = new GLfloat[size];
    
    GLfloat cubeStatic[] = 
        {
            -0.5, -0.5, -0.5, -1, +0, +0,
            -0.5, -0.5, +0.5, -1, +0, +0,
            -0.5, +0.5, +0.5, -1, +0, +0,
            -0.5, -0.5, -0.5, -1, +0, +0,
            -0.5, +0.5, +0.5, -1, +0, +0,
            -0.5, +0.5, -0.5, -1, +0, +0,

            -0.5, -0.5, -0.5, +0, -1, +0,
            +0.5, -0.5, -0.5, +0, -1, +0,
            +0.5, -0.5, +0.5, +0, -1, +0,
            -0.5, -0.5, -0.5, +0, -1, +0,
            +0.5, -0.5, +0.5, +0, -1, +0,
            -0.5, -0.5, +0.5, +0, -1, +0,

            -0.5, -0.5, -0.5, +0, +0, -1,
            -0.5, +0.5, -0.5, +0, +0, -1,
            +0.5, +0.5, -0.5, +0, +0, -1,
            -0.5, -0.5, -0.5, +0, +0, -1,
            +0.5, +0.5, -0.5, +0, +0, -1,
            +0.5, -0.5, -0.5, +0, +0, -1,

            +0.5, +0.5, +0.5, +1, +0, +0,
            +0.5, -0.5, -0.5, +1, +0, +0,
            +0.5, +0.5, -0.5, +1, +0, +0,
            +0.5, +0.5, +0.5, +1, +0, +0,
            +0.5, -0.5, +0.5, +1, +0, +0,
            +0.5, -0.5, -0.5, +1, +0, +0,

            +0.5, +0.5, +0.5, +0, +1, +0,
            -0.5, +0.5, -0.5, +0, +1, +0,
            -0.5, +0.5, +0.5, +0, +1, +0,
            +0.5, +0.5, +0.5, +0, +1, +0,
            +0.5, +0.5, -0.5, +0, +1, +0,
            -0.5, +0.5, -0.5, +0, +1, +0,

            +0.5, +0.5, +0.5, +0, +0, +1,
            -0.5, -0.5, +0.5, +0, +0, +1,
            +0.5, -0.5, +0.5, +0, +0, +1,
            +0.5, +0.5, +0.5, +0, +0, +1,
            -0.5, +0.5, +0.5, +0, +0, +1,
            -0.5, -0.5, +0.5, +0, +0, +1
        };

    for (int i = 0; i < size; i++)
    {
        cube[i] = cubeStatic[i];
    }

    return cube;
}

int GetSphereArraySize(int meridians, int paralells)
{
    return (3 + 3 + 3) * 3 * 2 * paralells * meridians;
}

GLfloat* CreateSphere(float radius, int meridians, int paralells, vec3 baseColor, float minColorValue)
{
    if (radius <= 0) radius = 1;
    if (meridians < 3) meridians = 3;
    if (paralells < 1) paralells = 1;

    vec3** vertices = new vec3*[meridians];
    vec3** colors = new vec3*[meridians];
    for (int lon = 0; lon < meridians; lon++)
    {
        vertices[lon] = new vec3[paralells];
        colors[lon] = new vec3[paralells];
    }

    float deltaPhi = 360.f / meridians;
    float deltaPsi = 180.f / (paralells + 1);
    float psiStart = -90 + deltaPsi;
    for (int lon = 0; lon < meridians; lon++)
    {
        float phi = lon * deltaPhi;
        float rPhi = radians(phi);
        for (int lat = 0; lat < paralells; lat++)
        {
            float psi = psiStart + lat * deltaPsi;
            float rPsi = radians(psi);
            vertices[lon][lat] = vec3(sin(rPhi) * cos(rPsi), sin(rPsi), cos(rPhi) * cos(rPsi));
            colors[lon][lat] = GetRandomColor(baseColor, minColorValue);
        }
    }

    GLfloat* data = new GLfloat[GetSphereArraySize(meridians, paralells)];

    vec3 southPole = vec3(0, -1, 0);
    vec3 southPoleColor = GetRandomColor(baseColor, minColorValue);
    for (int lon = 0; lon < meridians; lon++)
    {
        int stride = (3 + 3 + 3) * 3 * lon;

        data[stride + 0] = southPole.x * radius;
        data[stride + 1] = southPole.y * radius;
        data[stride + 2] = southPole.z * radius;
        data[stride + 3] = southPole.x;
        data[stride + 4] = southPole.y;
        data[stride + 5] = southPole.z;
        data[stride + 6] = southPoleColor.r;
        data[stride + 7] = southPoleColor.g;
        data[stride + 8] = southPoleColor.b;

        vec3 v1 = vertices[(lon + 1) % meridians][0];
        vec3 v1Color = colors[(lon + 1) % meridians][0];
        data[stride + 9] = v1.x * radius;
        data[stride + 10] = v1.y * radius;
        data[stride + 11] = v1.z * radius;
        data[stride + 12] = v1.x;
        data[stride + 13] = v1.y;
        data[stride + 14] = v1.z;
        data[stride + 15] = v1Color.r;
        data[stride + 16] = v1Color.g;
        data[stride + 17] = v1Color.b;

        vec3 v2 = vertices[lon][0];
        vec3 v2Color = colors[lon][0];
        data[stride + 18] = v2.x * radius;
        data[stride + 19] = v2.y * radius;
        data[stride + 20] = v2.z * radius;
        data[stride + 21] = v2.x;
        data[stride + 22] = v2.y;
        data[stride + 23] = v2.z;
        data[stride + 24] = v2Color.r;
        data[stride + 25] = v2Color.g;
        data[stride + 26] = v2Color.b;
    }

    int offset = (3 + 3 + 3) * 3 * meridians;
    for (int lon = 0; lon < meridians; lon++)
    {
        int lonStride = (3 + 3 + 3) * 3 * 2 * (paralells - 1) * lon;
        for (int lat = 0; lat < paralells - 1; lat++)
        {
            int latStride = (3 + 3 + 3) * 3 * 2 * lat;
            int stride = offset + lonStride + latStride;

            vec3 v1 = vertices[lon][lat];
            vec3 v1Color = colors[lon][lat];
            vec3 v2 = vertices[(lon + 1) % meridians][lat];
            vec3 v2Color = colors[(lon + 1) % meridians][lat];
            vec3 v3 = vertices[lon][lat + 1];
            vec3 v3Color = colors[lon][lat + 1];
            vec3 v4 = vertices[(lon + 1) % meridians][lat + 1];
            vec3 v4Color = colors[(lon + 1) % meridians][lat + 1];

            data[stride + 0] = v2.x * radius;
            data[stride + 1] = v2.y * radius;
            data[stride + 2] = v2.z * radius;
            data[stride + 3] = v2.x;
            data[stride + 4] = v2.y;
            data[stride + 5] = v2.z;
            data[stride + 6] = v2Color.r;
            data[stride + 7] = v2Color.g;
            data[stride + 8] = v2Color.b;

            data[stride + 9] = v1.x * radius;
            data[stride + 10] = v1.y * radius;
            data[stride + 11] = v1.z * radius;
            data[stride + 12] = v1.x;
            data[stride + 13] = v1.y;
            data[stride + 14] = v1.z;
            data[stride + 15] = v1Color.r;
            data[stride + 16] = v1Color.g;
            data[stride + 17] = v1Color.b;

            data[stride + 18] = v3.x * radius;
            data[stride + 19] = v3.y * radius;
            data[stride + 20] = v3.z * radius;
            data[stride + 21] = v3.x;
            data[stride + 22] = v3.y;
            data[stride + 23] = v3.z;
            data[stride + 24] = v3Color.r;
            data[stride + 25] = v3Color.g;
            data[stride + 26] = v3Color.b;

            data[stride + 27] = v4.x * radius;
            data[stride + 28] = v4.y * radius;
            data[stride + 29] = v4.z * radius;
            data[stride + 30] = v4.x;
            data[stride + 31] = v4.y;
            data[stride + 32] = v4.z;
            data[stride + 33] = v4Color.r;
            data[stride + 34] = v4Color.g;
            data[stride + 35] = v4Color.b;

            data[stride + 36] = v2.x * radius;
            data[stride + 37] = v2.y * radius;
            data[stride + 38] = v2.z * radius;
            data[stride + 39] = v2.x;
            data[stride + 40] = v2.y;
            data[stride + 41] = v2.z;
            data[stride + 42] = v2Color.r;
            data[stride + 43] = v2Color.g;
            data[stride + 44] = v2Color.b;

            data[stride + 45] = v3.x * radius;
            data[stride + 46] = v3.y * radius;
            data[stride + 47] = v3.z * radius;
            data[stride + 48] = v3.x;
            data[stride + 49] = v3.y;
            data[stride + 50] = v3.z;
            data[stride + 51] = v3Color.r;
            data[stride + 52] = v3Color.g;
            data[stride + 53] = v3Color.b;
        }
    }

    offset += (3 + 3 + 3) * 3 * 2 * (paralells - 1) * meridians;

    vec3 northPole = vec3(0, 1, 0);
    vec3 northPoleColor = GetRandomColor(baseColor, minColorValue);
    for (int lon = 0; lon < meridians; lon++)
    {
        int stride = offset + (3 + 3 + 3) * 3 * lon;

        data[stride + 0] = northPole.x * radius;
        data[stride + 1] = northPole.y * radius;
        data[stride + 2] = northPole.z * radius;
        data[stride + 3] = northPole.x;
        data[stride + 4] = northPole.y;
        data[stride + 5] = northPole.z;
        data[stride + 6] = northPoleColor.r;
        data[stride + 7] = northPoleColor.g;
        data[stride + 8] = northPoleColor.b;

        vec3 v1 = vertices[lon][paralells - 1];
        vec3 v1Color = colors[lon][paralells - 1];
        data[stride + 9] = v1.x * radius;
        data[stride + 10] = v1.y * radius;
        data[stride + 11] = v1.z * radius;
        data[stride + 12] = v1.x;
        data[stride + 13] = v1.y;
        data[stride + 14] = v1.z;
        data[stride + 15] = v1Color.r;
        data[stride + 16] = v1Color.g;
        data[stride + 17] = v1Color.b;

        vec3 v2 = vertices[(lon + 1) % meridians][paralells - 1];
        vec3 v2Color = colors[(lon + 1) % meridians][paralells - 1];
        data[stride + 18] = v2.x * radius;
        data[stride + 19] = v2.y * radius;
        data[stride + 20] = v2.z * radius;
        data[stride + 21] = v2.x;
        data[stride + 22] = v2.y;
        data[stride + 23] = v2.z;
        data[stride + 24] = v2Color.r;
        data[stride + 25] = v2Color.g;
        data[stride + 26] = v2Color.b;
    }

    for (int lon = 0; lon < meridians; lon++)
    {
        delete[] vertices[lon];
        delete[] colors[lon];
    }
    delete[] vertices;
    delete[] colors;

    return data;
}

vec3 GetRandomColor(vec3 baseColor, float minValue)
{
    float random = (float)rand() / RAND_MAX;
    float value = minValue + (1 - minValue) * random;
    return baseColor * value;
}