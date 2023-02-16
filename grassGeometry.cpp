#include "grassGeometry.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include <iostream>

// create rectangle
Mesh* grass::CreateGrass(
    const std::string& name,
    glm::vec3 color)
{
    std::vector<glm::vec3> squareCorners = {
        glm::vec3(-5, -0.018, 10), // A
        glm::vec3(-5, -0.018, -10), // B
        glm::vec3(15, -0.018, 10), // C
        glm::vec3(15, -0.018, -10) // D
    };

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    int noind = 0;

    // directiile dreptelor care il contin pe A
    glm::vec3 DABmain = glm::normalize(squareCorners[0] - squareCorners[1]);
    glm::vec3 DACmain = glm::normalize(squareCorners[0] - squareCorners[2]);
    glm::vec3 DCDmain = glm::normalize(squareCorners[2] - squareCorners[3]);
    for (int i = 1; i <= 100; i++) {
        // generam cele 2 puncte curente de pe CD (luam puncte dinspre C spre D)
        glm::vec3 P3init = squareCorners[2] + 0.2f * (i - 1) * DCDmain;
        glm::vec3 P4init = squareCorners[2] + 0.2f * i * DCDmain;
        // generam cele 2 puncte curente de pe AB (luam puncte dinspre A spre B), la pozitii simetrice cu
        // punctele de pe CD calculate mai devreme
        glm::vec3 P1init = squareCorners[0] + 0.2f * (i - 1) * DABmain;
        glm::vec3 P2init = squareCorners[0] + 0.2f * i * DABmain;

        glm::vec3 DAC1 = glm::normalize(P3init - P1init);
        glm::vec3 DAC2 = glm::normalize(P4init - P2init);
        for (int j = 1; j <= 100; j++) {
            // punctele vecine cu distanta 0.2f intre ele paralele cu AC
            glm::vec3 P1 = P1init + 0.2f * (j - 1) * DAC1;
            glm::vec3 P2 = P1init + 0.2f * j * DAC1;

            // punctele vecine cu distanta 0.2f intre ele paralele cu AB
            glm::vec3 P3 = P2init + 0.2f * (j - 1) * DAC2;
            glm::vec3 P4 = P2init + 0.2f * j * DAC2;
            
            vertices.push_back(VertexFormat(P1, color));
            indices.push_back(noind);
            noind++;

            vertices.push_back(VertexFormat(P2, color));
            indices.push_back(noind);
            noind++;

            vertices.push_back(VertexFormat(P3, color));
            indices.push_back(noind);
            noind++;

            vertices.push_back(VertexFormat(P3, color));
            indices.push_back(noind);
            noind++;

            vertices.push_back(VertexFormat(P2, color));
            indices.push_back(noind);
            noind++;

            vertices.push_back(VertexFormat(P4, color));
            indices.push_back(noind);
            noind++;
        }
    }
    Mesh* grass = new Mesh(name);

    grass->InitFromData(vertices, indices);
    return grass;
}
