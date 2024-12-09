#ifndef SURFACE_H
#define SURFACE_H

#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include "Shader.h"
#include "Octree.h"

using namespace std;

class Surface 
{
public:
    Surface(const vector<glm::vec3>& controlPoints, int widthU, int widthV,
        const vector<float>& knotU, const vector<float>& knotV);

    glm::vec3 calculateSurfacePoint(float u, float v) const;

    vector<glm::vec3> calculateSurfacePoints(int pointsOnTheSurface) const;

    vector<glm::vec3> calculateSurfaceNormals(int pointsOnTheSurface) const;

    vector<unsigned int> generateIndices(int pointsOnTheSurface) const;

    void setupBuffers(unsigned int& surfaceVAO, unsigned int& surfaceVBO, unsigned int& colorVBO,
        unsigned int& normalVBO, unsigned int& EBO, unsigned int& normalVAO, unsigned int& normalLineVBO,
        int pointsOnTheSurface, float frictionAreaXMin, float frictionAreaXMax,
        float frictionAreaYMin, float frictionAreaYMax);

    glm::vec3 calculatePartialDerivative(float u, float v, bool evaluateInUDirection) const;

    std::vector<glm::vec3> calculateBSplineCurve(const std::vector<glm::vec3>& controlPoints, int degree, int resolution) const;
    void renderBSplineCurve(const std::vector<glm::vec3>& curvePoints, Shader& shader, glm::mat4& projection, glm::mat4& view) const;
private:
    float BSplineBasisFunctions(int i, int d, float t, const vector<float>& knots) const;
    vector<glm::vec3> controlPoints;
    int widthU, widthV; 
    vector<float> knotU, knotV;
};

#endif
