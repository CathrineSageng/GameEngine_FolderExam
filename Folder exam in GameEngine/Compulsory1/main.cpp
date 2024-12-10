#include <iostream>
#include "glm/mat4x3.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Camera.h"
#include "Ball.h"
#include "ParticleSystem.h"
#include "CollisionSystem.h"
#include "FrictionSystem.h"
#include "PositionSystem.h"
#include "EntityManager.h"
#include "BallTrackingSystem.h"

extern "C"
{
 #include "Lua/Lua/include/lua.h"
#include "Lua/Lua/include/lauxlib.h"
#include "Lua/Lua/include/lualib.h"
}

#ifdef _WIN32
#pragma comment(lib, "Lua/Lua/lua54.lib")
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using namespace std;
//-----------------------------------------------------------------------------------------------------------------------------------------------------//
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;

Camera camera(glm::vec3(1.5f, -2.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 45.0f);

float xMin = 0.0f; 
float xMax = 3.0f; 
float yMin = 0.0f; 
float yMax = 2.0f; 

bool ballsMoving = false;
bool SnowMoving = false;
float ballRadius = 0.1f;

bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
const float fixedTimeStep = 0.01f;
float accumulator = 0.0f;

float frictionAreaXMin = 0.0f;
float frictionAreaXMax = 1.0f;
float frictionAreaYMin = 0.0f;
float frictionAreaYMax = 1.0f;
 
float normalFriction = 0.01f; 
float highFriction = 0.5f; 

glm::vec3 sunPos(2.0f, 2.0f, 2.0f);

vector<glm::vec3> controlPoints = 
{
    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(2.0f, 1.0f, 1.0f), glm::vec3(3.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 2.0f, 0.0f), glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(3.0f, 2.0f, 0.0f),
};

vector<float> knotVectorU = { 0, 0, 0, 1, 2, 2, 2 }; 
vector<float> knotVectorV = { 0, 0, 0, 1, 1, 1 };

//------------------------------------------------------------------------------------------------------------------------------------------------//

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
void selectStartPointForBall(Surface& surface, glm::vec3& ballPosition, float xMin, float xMax, float yMin, float yMax, float ballRadius);
void loadEntities(lua_State* L, EntityManager& entityManager);

//-----------------------------------------------------------------------------------------------------------------------------------------------//

int main()
{
//----------------------------------------------------------------------------------------------------------------------------------------------//
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    EntityManager entityManager;
    loadEntities(L, entityManager);

    lua_close(L);
//----------------------------------------------------------------------------------------------------------------------------------------------//

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Folder exam", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" <<endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------//

    Shader phongShader("phong.vert", "phong.frag");
    Shader textureShader("Texture.vs", "Texture.fs");
    Shader particleShader("particle.vert", "particle.frag");

    ParticleSystem particleSystem(50000);
    PositionSystem positionSystem;
    FrictionSystem frictionSystem;
    CollisionSystem  collisionSystem;
    BallTrackingSystem ballTrackingSystem;
    Octree octree(glm::vec3(xMin, yMin, xMin), glm::vec3(xMax, yMax, xMax));
    Surface surface(controlPoints, 4, 3, knotVectorU, knotVectorV); 
 
    vector<Ball> balls;
    for (size_t i = 0; i < entityManager.positions.size(); ++i) {
        float radius = entityManager.radii[i].radius; 
        balls.push_back(Ball(radius, 30, 30, glm::vec3(1.0f, 1.0f, 1.0f)));
    }


    int pointsOnTheSurface = 20;

    unsigned int surfaceVAO, surfaceVBO, colorVBO, normalVBO, EBO, normalVAO, normalLineVBO;
    surface.setupBuffers(surfaceVAO, surfaceVBO, colorVBO, normalVBO, EBO, normalVAO, normalLineVBO,
        pointsOnTheSurface, frictionAreaXMin, frictionAreaXMax,
        frictionAreaYMin, frictionAreaYMax);

    unsigned int diffuseMap1 = loadTexture("Textures/ball.jpg");
    unsigned int specularMap = loadTexture("Textures/ball2.jpg");

    textureShader.use();
    textureShader.setInt("material.diffuse", 0);
    textureShader.setInt("material.specular", 1);

//--------------------------------------------------------------------------------------------------------------------------------------------------------//

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        phongShader.use();

        phongShader.setVec3("light.position", sunPos);
        phongShader.setVec3("viewPos", camera.Position);

        phongShader.setVec3("light.ambient", 0.3f, 0.4f, 0.3f);
        phongShader.setVec3("light.diffuse", 0.3f, 0.7f, 0.3f);
        phongShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        phongShader.setVec3("material.ambient", 0.1f, 0.5f, 0.1f);
        phongShader.setVec3("material.diffuse", 0.2f, 0.8f, 0.8f);
        phongShader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);
        phongShader.setFloat("material.shininess", 32.0f);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        phongShader.setMat4("projection", projection);
        phongShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        phongShader.setMat4("model", model);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(surfaceVAO);
        glDrawElements(GL_TRIANGLES, pointsOnTheSurface * pointsOnTheSurface * 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        for (size_t i = 0; i < entityManager.tracks.size(); ++i) 
        {
            const vector<glm::vec3>& ballTrack = entityManager.tracks[i].track;
            if (ballTrack.size() > 1) 
            {
                auto curvePoints = surface.calculateBSplineCurve(ballTrack, 3, 50);
                surface.renderBSplineCurve(curvePoints, phongShader, projection, view);
            }
        }
        ballTrackingSystem.update(entityManager.positions, entityManager.tracks, 0.01f);

        textureShader.use();
        textureShader.setVec3("light.position", sunPos);
        textureShader.setVec3("viewPos", camera.Position);

        textureShader.setVec3("light.ambient", 0.5f, 0.6f, 0.6f);
        textureShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        textureShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        textureShader.setFloat("material.shininess", 64.0f);

        textureShader.setMat4("projection", projection);
        textureShader.setMat4("view", view);

        accumulator += deltaTime;
        while (accumulator >= fixedTimeStep)
        {
            if (ballsMoving)
            {
              /*  cout << "--- Updating Physics ---" << endl;*/
                positionSystem.update(entityManager.positions,
                    entityManager.velocities,
                    fixedTimeStep,
                    xMin, xMax, yMin, yMax,
                    ballRadius, surface);
                frictionSystem.applyFriction(entityManager.velocities, entityManager.positions,
                    fixedTimeStep, normalFriction, highFriction,
                    frictionAreaXMin, frictionAreaXMax,
                    frictionAreaYMin, frictionAreaYMax);

                octree = Octree(glm::vec3(xMin, yMin, xMin), glm::vec3(xMax, yMax, xMax));
      
                vector<glm::vec3> positionVec(entityManager.positions.size());
                for (size_t i = 0; i < entityManager.positions.size(); ++i) 
                {
                    positionVec[i] = entityManager.positions[i].position;
                }

                for (size_t i = 0; i < positionVec.size(); ++i) 
                {
                    octree.insert(i, positionVec, ballRadius);
                }

                collisionSystem.whenCollisionHappens(entityManager.positions, entityManager.velocities, entityManager.radii, octree);
            }
            accumulator -= fixedTimeStep;
        }

        accumulator += deltaTime;
     
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap1);

        for (size_t i = 0; i < entityManager.positions.size(); ++i) 
        {
            balls[i].UpdateRotation(entityManager.velocities[i].velocity, deltaTime, ballsMoving);
            model = glm::mat4(1.0f);
            model = glm::translate(model, entityManager.positions[i].position);
            model = model * balls[i].rotationMatrix; 
            textureShader.setMat4("model", model);
            balls[i].DrawBall();
        }

        if (SnowMoving) 
        {
            particleSystem.emitter();
            
            float snowUpdateInterval = 0.1f; 
            static float snowAccumulator = 0.0f;

            snowAccumulator += deltaTime;
                
            if (snowAccumulator >= snowUpdateInterval) 
            {
                particleSystem.updateParticles(deltaTime, surface);
                snowAccumulator = 0.0f;
            }

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            particleSystem.renderParticles(particleShader, projection, view);
            glDisable(GL_BLEND);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

//----------FUNKSJONER-----------------------------------------------------------------------------------------------------------------//

void loadEntities(lua_State* L, EntityManager& entityManager) 
{
    if (luaL_dofile(L, "entities.lua") != LUA_OK) 
    {
        cerr << "Error loading Lua script: " << lua_tostring(L, -1) <<endl;
        return;
    }

    lua_getglobal(L, "entities");
    if (!lua_istable(L, -1)) 
    {
        cerr << "Error: 'entities' is not a table" << endl;
        return;
    }

    lua_pushnil(L); 
    while (lua_next(L, -2) != 0) 
    {
        lua_getfield(L, -1, "position");
        glm::vec3 position(0.0f);
        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, "x"); position.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
            lua_getfield(L, -1, "y"); position.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
            lua_getfield(L, -1, "z"); position.z = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        }
        lua_pop(L, 1); 

        lua_getfield(L, -1, "velocity");
        glm::vec3 velocity(0.0f);
        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, "x"); velocity.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
            lua_getfield(L, -1, "y"); velocity.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
            lua_getfield(L, -1, "z"); velocity.z = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, "radius");
        float radius = (float)lua_tonumber(L, -1);
        lua_pop(L, 1); 

        entityManager.createEntity(position, velocity, radius);

        lua_pop(L, 1); 
    }

    lua_pop(L, 1); 
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

 /*   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);*/

    // Ballene begynner å bevege seg når 'space' er trykket på 
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        ballsMoving = true; 
        SnowMoving = true;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
 /*   float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);*/
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void selectStartPointForBall(Surface& surface, glm::vec3& ballPosition, float xMin, float xMax, float yMin, float yMax, float ballRadius)
{
    float x, y;
    cout << "Velg startpunkt for ballen innenfor oppgitte koordinatgrenser:"<<endl;
    cout << "x (mellom " << xMin << " og " << xMax << "): ";
    cin >> x;
    cout << "y (mellom " << yMin << " og " << yMax << "): ";
    cin >> y;

    x = glm::clamp(x, xMin, xMax);
    y = glm::clamp(y, yMin, yMax);

    float u = (x - xMin) / (xMax - xMin);
    float v = (y - yMin) / (yMax - yMin);

    glm::vec3 surfacePoint = surface.calculateSurfacePoint(u, v);

    ballPosition = glm::vec3(x, y, surfacePoint.z + ballRadius);

    cout << "Ballen er plassert på punkt ("<< ballPosition.x << ", " << ballPosition.y << ", "<< ballPosition.z << ")."<<endl;
}











