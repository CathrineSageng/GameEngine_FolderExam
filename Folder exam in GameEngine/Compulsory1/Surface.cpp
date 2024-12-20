#include "Surface.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

Surface::Surface(const vector<glm::vec3>& controlPoints, int widthU, int widthV,
    const vector<float>& knotU, const vector<float>& knotV)
    : controlPoints(controlPoints), widthU(widthU), widthV(widthV), knotU(knotU), knotV(knotV) {}

float Surface::BSplineBasisFunctions(int i, int d, float t, const vector<float>& knots) const 
{
    if (i < 0 || i >= knots.size() - 1) return 0.0f;
    if (d == 0) return (knots[i] <= t && t < knots[i + 1]) ? 1.0f : 0.0f;
    float intervalLength1 = knots[i + d] - knots[i];
    float basisContribution1 = (intervalLength1 != 0.0f) ? (t - knots[i]) / intervalLength1 * BSplineBasisFunctions(i, d - 1, t, knots) : 0.0f;
    float intervalLength2 = knots[i + d + 1] - knots[i + 1];
    float basisContribution2 = (intervalLength2 != 0.0f) ? (knots[i + d + 1] - t) / intervalLength2 * BSplineBasisFunctions(i + 1, d - 1, t, knots) : 0.0f;
    return basisContribution1 + basisContribution2;
}

glm::vec3 Surface::calculatePartialDerivative(float u, float v, bool evaluateInUDirection) const 
{
    glm::vec3 derivative(0.0f);
    int degreeU = 2, degreeV = 2;
    float scaledU = u * (knotU[knotU.size() - degreeU - 1] - knotU.front()) + knotU.front();
    float scaledV = v * (knotV[knotV.size() - degreeV - 1] - knotV.front()) + knotV.front();

    //G�r gjennom alle kontrollpunktene. 
    for (int i = 0; i < widthU; ++i) 
    {
        for (int j = 0; j < widthV; ++j) 
        {
            int index = j * widthU + i;
            if (index < controlPoints.size()) 
            {
                float basisU = evaluateInUDirection ? BSplineBasisFunctions(i, degreeU - 1, scaledU, knotU) : BSplineBasisFunctions(i, degreeU, scaledU, knotU);
                float basisV = evaluateInUDirection ? BSplineBasisFunctions(j, degreeV, scaledV, knotV) : BSplineBasisFunctions(j, degreeV - 1, scaledV, knotV);
                derivative += basisU * basisV * controlPoints[index];
            }
        }
    }
    return derivative;
}

glm::vec3 Surface::calculateSurfacePoint(float u, float v) const 
{
    glm::vec3 point(0.0f);
    int degreeU = 2, degreeV = 2;
    float scaledU = std::min(u * (knotU[knotU.size() - degreeU - 1] - knotU.front()) + knotU.front(), knotU[knotU.size() - degreeU - 1] - 0.001f);
    float scaledV = std::min(v * (knotV[knotV.size() - degreeV - 1] - knotV.front()) + knotV.front(), knotV[knotV.size() - degreeV - 1] - 0.001f);

    for (int i = 0; i < widthU; ++i) 
    {
        for (int j = 0; j < widthV; ++j) 
        {
            int index = j * widthU + i;
            if (index < controlPoints.size()) 
            {
                float basisU = BSplineBasisFunctions(i, degreeU, scaledU, knotU);
                float basisV = BSplineBasisFunctions(j, degreeV, scaledV, knotV);
                point += basisU * basisV * controlPoints[index];
            }
        }
    }
    return point;
}

vector<glm::vec3> Surface::calculateSurfacePoints(int pointsOnTheSurface) const 
{
    vector<glm::vec3> surfacePoints;
    for (int i = 0; i < pointsOnTheSurface; ++i) 
    {
        for (int j = 0; j < pointsOnTheSurface; ++j) 
        {
            float u = i / static_cast<float>(pointsOnTheSurface - 1);
            float v = j / static_cast<float>(pointsOnTheSurface - 1);
            surfacePoints.push_back(calculateSurfacePoint(u, v));
        }
    }
    return surfacePoints;
}

vector<glm::vec3> Surface::calculateSurfaceNormals(int pointsOnTheSurface) const 
{
    vector<glm::vec3> normals;
    float epsilon = 0.001f; 

    for (int i = 0; i < pointsOnTheSurface; ++i) 
    {
        for (int j = 0; j < pointsOnTheSurface; ++j) 
        {
           
            float u = i / static_cast<float>(pointsOnTheSurface - 1);
            float v = j / static_cast<float>(pointsOnTheSurface - 1);

            if (u <= 0.0f) u += epsilon;
            else if (u >= 1.0f) u -= epsilon;

            if (v <= 0.0f) v += epsilon;
            else if (v >= 1.0f) v -= epsilon;

            glm::vec3 partialU = calculatePartialDerivative(u, v, true);
            glm::vec3 partialV = calculatePartialDerivative(u, v, false);

            glm::vec3 normal = glm::normalize(glm::cross(partialU, partialV));

            normals.push_back(normal);
        }
    }
    return normals;
}

vector<unsigned int> Surface::generateIndices(int pointsOnTheSurface) const 
{
    vector<unsigned int> indices;
    for (int i = 0; i < pointsOnTheSurface - 1; ++i) 
    {
        for (int j = 0; j < pointsOnTheSurface - 1; ++j) 
        {
            int start = i * pointsOnTheSurface + j;
            indices.push_back(start);
            indices.push_back(start + 1);
            indices.push_back(start + pointsOnTheSurface);
            indices.push_back(start + 1);
            indices.push_back(start + pointsOnTheSurface);
            indices.push_back(start + pointsOnTheSurface + 1);
        }
    }
    return indices;
}

std::vector<glm::vec3> Surface::calculateBSplineCurve(const std::vector<glm::vec3>& controlPoints, int degree, int resolution) const {
    std::vector<glm::vec3> splinePoints;

    if (controlPoints.size() < degree + 1) 
    {
        return splinePoints; 
    }

    std::vector<glm::vec3> validControlPoints;
    for (const auto& point : controlPoints) 
    {
        if (point != glm::vec3(0.0f, 0.0f, 0.0f)) 
        {
            validControlPoints.push_back(point);
        }
    }

    if (validControlPoints.size() < degree + 1) 
    {
        return splinePoints;
    }

    int knotCount = validControlPoints.size() + degree + 1;
    std::vector<float> knots(knotCount);
    for (int i = 0; i < knotCount; ++i) 
    {
        knots[i] = i < degree + 1 ? 0.0f : (i > validControlPoints.size() ? 1.0f : (float)(i - degree) / (validControlPoints.size() - degree));
    }

    for (int step = 0; step <= resolution; ++step) 
    {
        float t = step / static_cast<float>(resolution);

        glm::vec3 point(0.0f);
        for (int i = 0; i < validControlPoints.size(); ++i) 
        {
            float basis = BSplineBasisFunctions(i, degree, t, knots);
            point += basis * validControlPoints[i];
        }

        splinePoints.push_back(point);
    }

    return splinePoints;
}

void Surface::renderBSplineCurve(const std::vector<glm::vec3>& curvePoints, Shader& shader, glm::mat4& projection, glm::mat4& view) const 
{
    static std::vector<float> permanentVertices; 
    static glm::vec3 lastPoint(0.0f, 0.0f, 0.0f); 
    float pointSpacing = 0.1f; 

    for (const auto& point : curvePoints) 
    {
        if (permanentVertices.empty() || glm::distance(lastPoint, point) >= pointSpacing) 
        {
            permanentVertices.push_back(point.x);
            permanentVertices.push_back(point.y);
            permanentVertices.push_back(point.z);
            lastPoint = point;
        }
    }

    if (permanentVertices.empty()) 
    {
        return; 
    }

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, permanentVertices.size() * sizeof(float), permanentVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

   
    glPointSize(5.0f); 
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, permanentVertices.size() / 3);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}


void Surface::setupBuffers(unsigned int& surfaceVAO, unsigned int& surfaceVBO, unsigned int& colorVBO,
    unsigned int& normalVBO, unsigned int& EBO, unsigned int& normalVAO, unsigned int& normalLineVBO,
    int pointsOnTheSurface, float frictionAreaXMin, float frictionAreaXMax,
    float frictionAreaYMin, float frictionAreaYMax)
{
    vector<glm::vec3> surfacePoints = calculateSurfacePoints(pointsOnTheSurface);
    vector<glm::vec3> normals = calculateSurfaceNormals(pointsOnTheSurface);
    vector<unsigned int> indices = generateIndices(pointsOnTheSurface);
    vector<glm::vec3> colors;


    for (const glm::vec3& point : surfacePoints) 
    {
        if (point.x >= frictionAreaXMin && point.x <= frictionAreaXMax &&
            point.y >= frictionAreaYMin && point.y <= frictionAreaYMax) 
        {
            colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else {
            colors.push_back(glm::vec3(0.7f, 0.7f, 0.7f));
        }
    }

    glGenVertexArrays(1, &surfaceVAO);
    glGenBuffers(1, &surfaceVBO);
    glGenBuffers(1, &colorVBO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(surfaceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);
    glBufferData(GL_ARRAY_BUFFER, surfacePoints.size() * sizeof(glm::vec3), &surfacePoints[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    vector<glm::vec3> normalLines;
    float normalLength = 0.05f;
    for (int i = 0; i < surfacePoints.size(); ++i) 
    {
        glm::vec3 startPoint = surfacePoints[i];
        glm::vec3 endPoint = startPoint + normals[i] * normalLength;
        normalLines.push_back(startPoint);
        normalLines.push_back(endPoint);
    }

    glGenVertexArrays(1, &normalVAO);
    glGenBuffers(1, &normalLineVBO);

    glBindVertexArray(normalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalLineVBO);
    glBufferData(GL_ARRAY_BUFFER, normalLines.size() * sizeof(glm::vec3), &normalLines[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
