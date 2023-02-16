#include "trackGeometry.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include <iostream>
using namespace std;

// creare pista
Mesh* track::CreateTrack(
    const std::string& name,
    glm::vec3 color,
    std::vector<glm::vec3> initialTrajectoryVertices)
{
    std::vector<glm::vec3> preliminaryTrajectoryVertices;

    // pentru fiecare segment al pistei, generez cate 100 de puncte intermediare
    // care apartin segmentului
    for (int i = 0; i < initialTrajectoryVertices.size() - 1; i++) {
        preliminaryTrajectoryVertices.push_back(initialTrajectoryVertices[i]);
        // calculez directia dintre cele doua puncte ale segmentului
        glm::vec3 P1 = initialTrajectoryVertices[i];
        glm::vec3 P2 = initialTrajectoryVertices[i + 1];
        glm::vec3 D = P2 - P1;
        // distanta dintre segmente
        float j = 0.001;
        while (1) {
            // calcul punct nou
            glm::vec3 P = P1 + j * D;
            j += 0.001;
            
            // verific ca punctul calculat sa nu fi iesit din segment
            if (P1.z <= P2.z && P1.x <= P2.x) {
                if (P.z >= P2.z && P.x >= P2.x) {
                    break;
                }
            }
            else if (P1.z >= P2.z && P1.x <= P2.x) {
                if (P.z <= P2.z && P.x >= P2.x) {
                    break;
                }
            }
            else if (P1.z <= P2.z && P1.x >= P2.x) {
                if (P.z >= P2.z && P.x <= P2.x) {
                    break;
                }
            }
            else if (P1.z >= P2.z && P1.x >= P2.x) {
                if (P.z <= P2.z && P.x <= P2.x) {
                    break;
                }
            }
            // adaugare punct nou
            preliminaryTrajectoryVertices.push_back(P);
        }
        // ma asigur ca am nr impar de puncte pentru a fi egal pe ambele segmente
        if (preliminaryTrajectoryVertices.size() % 2 == 1) {
            preliminaryTrajectoryVertices.pop_back();
        }
    }
    // adaugare primele doua puncte din nou pentru a putea forma triunghiuri
    preliminaryTrajectoryVertices.push_back(preliminaryTrajectoryVertices[0]);
    preliminaryTrajectoryVertices.push_back(preliminaryTrajectoryVertices[1]);

    Mesh* track = new Mesh(name);
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    int noind = 0;

    // pentru fiecare punct, formez generez puncte in interior si exterior
    // pentru a putea forma triunghiuri
    for (int i = 0; i < preliminaryTrajectoryVertices.size() - 2; i++) {
        glm::vec3 P1_1 = preliminaryTrajectoryVertices[i];
        glm::vec3 P2_1 = preliminaryTrajectoryVertices[i + 1];
        glm::vec3 D_1 = P2_1 - P1_1;
        glm::vec3 UP = glm::vec3(0, 1, 0);
        glm::vec3 P_1 = glm::cross(D_1, UP);
        glm::vec3 R_1 = P1_1 + 350.0f * P_1;
        glm::vec3 A_1 = P1_1 - 350.0f * P_1;

        glm::vec3 P1_2 = preliminaryTrajectoryVertices[i + 1];
        glm::vec3 P2_2 = preliminaryTrajectoryVertices[i + 2];
        glm::vec3 D_2 = P2_2 - P1_2;
        glm::vec3 P_2 = glm::cross(D_2, UP);
        glm::vec3 R_2 = P1_2 + 350.0f * P_2;
        glm::vec3 A_2 = P1_2 - 350.0f * P_2;

        vertices.push_back(VertexFormat(R_1, color));
        indices.push_back(noind);
        noind++;

        vertices.push_back(VertexFormat(A_1, color));
        indices.push_back(noind);
        noind++;

        vertices.push_back(VertexFormat(A_2, color));
        indices.push_back(noind);
        noind++;

        vertices.push_back(VertexFormat(R_1, color));
        indices.push_back(noind);
        noind++;

        vertices.push_back(VertexFormat(A_2, color));
        indices.push_back(noind);
        noind++;

        vertices.push_back(VertexFormat(R_2, color));
        indices.push_back(noind);
        noind++;
    }

    track->InitFromData(vertices, indices);
    return track;
}


// similar cu crearea pistei, creez triunghiuri (dar folosesc cate 10 puncte pe segment)
std::vector<glm::vec3> track::generateFewerTrackTriangles(std::vector<glm::vec3>& initialTrajectoryVertices)
{
    std::vector<glm::vec3> preliminaryTrajectoryVertices;
    std::vector<glm::vec3> trackTriangless;
    for (int i = 0; i < initialTrajectoryVertices.size() - 1; i++) {
        preliminaryTrajectoryVertices.push_back(initialTrajectoryVertices[i]);
        glm::vec3 P1 = initialTrajectoryVertices[i];
        glm::vec3 P2 = initialTrajectoryVertices[i + 1];
        glm::vec3 D = P2 - P1;
        float j = 0.1;
        int nr = 0;
        nr++;
        while (1) {
            glm::vec3 P = P1 + j * D;
            j += 0.1;

            //1
            if (P1.z <= P2.z && P1.x <= P2.x) {
                if (P.z >= P2.z && P.x >= P2.x) {
                    break;
                }
                //2
            }
            else if (P1.z >= P2.z && P1.x <= P2.x) {
                if (P.z <= P2.z && P.x >= P2.x) {
                    break;
                }
                //3
            }
            else if (P1.z <= P2.z && P1.x >= P2.x) {
                if (P.z >= P2.z && P.x <= P2.x) {
                    break;
                }
                //4
            }
            else if (P1.z >= P2.z && P1.x >= P2.x) {
                if (P.z <= P2.z && P.x <= P2.x) {
                    break;
                }
            }
            preliminaryTrajectoryVertices.push_back(P);
        }
        if (preliminaryTrajectoryVertices.size() % 2 == 1) {
            preliminaryTrajectoryVertices.pop_back();
        }
    }

    preliminaryTrajectoryVertices.push_back(preliminaryTrajectoryVertices[0]);
    preliminaryTrajectoryVertices.push_back(preliminaryTrajectoryVertices[1]);

    for (int i = 0; i < preliminaryTrajectoryVertices.size() - 2; i++) {
        glm::vec3 P1_1 = preliminaryTrajectoryVertices[i];
        glm::vec3 P2_1 = preliminaryTrajectoryVertices[i + 1];
        glm::vec3 D_1 = P2_1 - P1_1;
        glm::vec3 UP = glm::vec3(0, 1, 0);
        glm::vec3 P_1 = glm::cross(D_1, UP);
        glm::vec3 R_1 = P1_1 + 350.0f / 100 * P_1;
        glm::vec3 A_1 = P1_1 - 350.0f / 100 * P_1;

        glm::vec3 P1_2 = preliminaryTrajectoryVertices[i + 1];
        glm::vec3 P2_2 = preliminaryTrajectoryVertices[i + 2];
        glm::vec3 D_2 = P2_2 - P1_2;
        glm::vec3 P_2 = glm::cross(D_2, UP);
        glm::vec3 R_2 = P1_2 + 350.0f / 100 * P_2;
        glm::vec3 A_2 = P1_2 - 350.0f / 100 * P_2;

        trackTriangless.push_back(R_1);

        trackTriangless.push_back(A_1);

        trackTriangless.push_back(A_2);

        trackTriangless.push_back(R_1);

        trackTriangless.push_back(A_2);

        trackTriangless.push_back(R_2);
    }
    return trackTriangless;
}


// verificare daca un punct se afla intr-un triunghi
// sursa functie: https://blackpawn.com/texts/pointinpoly/
bool track::checkInsideTriangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P)
{
    // calcul vectori      
    glm::vec3 v0 = C - A;
    glm::vec3 v1 = B - A;
    glm::vec3 v2 = P - A;

    // calcul produs scalar
    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    // calcul coordonate baricentrice
    float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // verificare
    return (u >= 0) && (v >= 0) && (u + v < 1);
}


// generare puncte spawn copaci
void track::generateTreeSpawnPoints(std::vector<glm::vec3> initialTrajectoryVertices,
    std::vector<glm::vec3>& treeSpawnPoints)
{
    std::vector<glm::vec3> preliminaryTrajectoryPoints;

    for (int i = 0; i < initialTrajectoryVertices.size(); i++) {
        preliminaryTrajectoryPoints.push_back(initialTrajectoryVertices[i]);
    }

    // pentru fiecare segment de drum, calculez puncte interioare si exterioare
    // cu formula
    for (int i = 0; i < preliminaryTrajectoryPoints.size() - 1; i++) {
        glm::vec3 P1_1 = preliminaryTrajectoryPoints[i];
        glm::vec3 P2_1 = preliminaryTrajectoryPoints[i + 1];
        glm::vec3 D_1 = P2_1 - P1_1;
        glm::vec3 UP = glm::vec3(0, 1, 0);
        glm::vec3 P_1 = glm::cross(D_1, UP);
        glm::vec3 R_1;
        glm::vec3 R_2 = P1_1 + 2.3f * P_1;
        glm::vec3 R_3 = P1_1 + 3.5f * P_1;

        // copacii care ajung pe pista sunt eliminati
        if (i == 13 || i == 14 || i == 15) {
            treeSpawnPoints.push_back(R_3);
            continue;
        }
        // copacii inghesuiti sunt mutati intr-o parte
        else if (i == 11 || i == 12) {
            R_1 = P1_1 + 0.65f * P_1;
        }
        else if (i == 17) {
            R_1 = P1_1 + 1.0f * P_1;
        }
        else {
            R_1 = P1_1 + 0.8f * P_1;
        }

        treeSpawnPoints.push_back(R_1);
        treeSpawnPoints.push_back(R_2);
        treeSpawnPoints.push_back(R_3);
    }

    // pentru fiecare segment de drum, calculez puncte interioare si exterioare
    // cu formula
    for (int i = 0; i < preliminaryTrajectoryPoints.size() - 1; i += 1) {
        // copacii care ajung pe pista sunt eliminati
        if (i == 6 || i == 7 || i == 13 || i == 14 || i == 16) {
            continue;
        }

        glm::vec3 P1_1 = preliminaryTrajectoryPoints[i];
        glm::vec3 P2_1 = preliminaryTrajectoryPoints[i + 1];
        glm::vec3 D_1 = P2_1 - P1_1;
        glm::vec3 UP = glm::vec3(0, 1, 0);
        glm::vec3 P_1 = glm::cross(D_1, UP);
        glm::vec3 R_1;

        // copacii inghesuiti sunt mutati intr-o parte
        if (i == 0) {
            R_1 = P1_1 - 1.2f * P_1;
        }
        else if (i == 8 || i == 9 || i == 11 || i == 13) {
            R_1 = P1_1 - 0.4f * P_1;
        }
        else if (i == 14) {
            R_1 = P1_1 - 1.5f * P_1;
        }
        else {
            R_1 = P1_1 - 0.9f * P_1;
        }
        treeSpawnPoints.push_back(R_1);
    }
    // adaugare copaci aditionali in pozitiile in care este gol
    treeSpawnPoints.push_back(glm::vec3(0.5, 0, -1));
    treeSpawnPoints.push_back(glm::vec3(1.6, 0, 0.5));
    treeSpawnPoints.push_back(glm::vec3(0.25, 0, -2));
    treeSpawnPoints.push_back(glm::vec3(-0.5, 0, 1));
    treeSpawnPoints.push_back(glm::vec3(-2, 0, 1));
    treeSpawnPoints.push_back(glm::vec3(-0.5, 0, 3));
}