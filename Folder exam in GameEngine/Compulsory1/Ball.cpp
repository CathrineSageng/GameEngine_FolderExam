#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "Ball.h"
#include "glm/mat4x3.hpp"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>



Ball::Ball(float radius, int sectors, int stacks, glm::vec3 color) :ballRadius(radius)
{
    VAO = 0;
    VBO = 0;
    EBO = 0;

    for (int i = 0; i <= stacks; ++i) 
    {
        float stackAngle = M_PI / 2 - i * M_PI / stacks;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) 
        {
            float sectorAngle = j * 2 * M_PI / sectors;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            colors.push_back(color.r);
            colors.push_back(color.g);
            colors.push_back(color.b);

            float u = (float)j / sectors; 
            float v = (float)i / stacks;  
            texCoords.push_back(u);
            texCoords.push_back(v);
        }
    }

    for (int i = 0; i < stacks; ++i) 
    {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) 
        {
            if (i != 0) 
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) 
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

 
    vector<float> interleavedData;
    for (size_t i = 0; i < vertices.size() / 3; ++i) 
    {
        interleavedData.push_back(vertices[3 * i]);      
        interleavedData.push_back(vertices[3 * i + 1]);  
        interleavedData.push_back(vertices[3 * i + 2]); 
        interleavedData.push_back(colors[3 * i]);        
        interleavedData.push_back(colors[3 * i + 1]);    
        interleavedData.push_back(colors[3 * i + 2]);    
        interleavedData.push_back(texCoords[2 * i]);     
        interleavedData.push_back(texCoords[2 * i + 1]); 
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(GLfloat), interleavedData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}


void Ball::DrawBall()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Ball::UpdateRotation(const glm::vec3& velocity, float deltaTime, bool ballsMoving)
{
    if (ballsMoving && glm::length(velocity) > 0.0f)
    {
        glm::vec3 rotationAxis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), velocity);
        if (glm::length(rotationAxis) != 0.0f)
        {
            rotationAxis = glm::normalize(rotationAxis);

            float speedOfTheRotation = glm::length(velocity) / ballRadius;
            float rotationAngle = speedOfTheRotation * deltaTime;
 
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);

            rotationMatrix = rotation * rotationMatrix;
        }
    }
}

