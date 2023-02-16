#include "enemyGenerator.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include <unordered_map>


// initializarea generarii punctelor de spawn inamice
void enemyGenerator::generateEnemyCars(std::vector<glm::vec3>& translateEnemy,
    std::vector <int>& currentEnemyPointIndex,
    std::vector <float>& timeElapsedBetweenPointsEnemy,
    std::vector<glm::vec3> initialTrajectoryVertices,
    std::unordered_map<int, std::vector<glm::vec3>>& enemyTrajectoryPoints,
    std::vector <float>& enemyRotation)
{
    // initializare vectori
    translateEnemy.resize(7);
    std::fill(translateEnemy.begin(), translateEnemy.end(), glm::vec3(0, 0, 0));

    currentEnemyPointIndex.resize(7);
    std::fill(currentEnemyPointIndex.begin(), currentEnemyPointIndex.end(), 0);

    timeElapsedBetweenPointsEnemy.resize(7);
    std::fill(timeElapsedBetweenPointsEnemy.begin(), timeElapsedBetweenPointsEnemy.end(), 0);

    enemyRotation.resize(7);
    std::fill(enemyRotation.begin(), enemyRotation.end(), 0);

    // pentru fiecare masina inamica, genereaza punctele traseului
    // pe care o sa il parcurga
    for (int i = 0; i < 7; i++) {
        generateEnemyTrajectory(i * 2, i, (i - 3) * 0.1f, initialTrajectoryVertices, enemyTrajectoryPoints);
        // tin minte la ce punct din punctele traseului ma aflu
        currentEnemyPointIndex[i] = 0;
    }

}

// updatare masina inamica
void enemyGenerator::updateEnemyCars(float deltaTimeSeconds, std::vector<glm::vec3>& translateEnemy,
    std::vector <int>& currentEnemyPointIndex,
    std::vector <float>& timeElapsedBetweenPointsEnemy,
    std::unordered_map<int, std::vector<glm::vec3>>& enemyTrajectoryPoints,
    std::vector <float>& enemyRotation)
{
    // pentru fiecare masina inamica, folosind o viteza diferita pentru fiecare,
    // updateaza pozitia acesteia pe traseu
    for (int i = 0; i < 7; i++) {
        float speed = 0.2 * (i + 2);
        updateEnemyPosition(deltaTimeSeconds, i, speed, translateEnemy,
            currentEnemyPointIndex, timeElapsedBetweenPointsEnemy, enemyTrajectoryPoints,
            enemyRotation);
    }
}

void enemyGenerator::updateEnemyPosition(float deltaTimeSeconds, int enemyCarIndex, float speed,
    std::vector<glm::vec3>& translateEnemy,
    std::vector <int>& currentEnemyPointIndex,
    std::vector <float>& timeElapsedBetweenPointsEnemy,
    std::unordered_map<int, std::vector<glm::vec3>>& enemyTrajectoryPoints,
    std::vector <float>& enemyRotation)
{
    glm::vec3 enemyDirection;

    // daca nu trebuie sa reincep traseul de la capat (nu am ajuns sa completez bucla)
    if (currentEnemyPointIndex[enemyCarIndex] < enemyTrajectoryPoints.at(enemyCarIndex).size() - 1) {
        // calculez directia pe care merge masina, folosind capetele segmentului pe care se afla
        glm::vec3 P1 = enemyTrajectoryPoints.at(enemyCarIndex)[currentEnemyPointIndex[enemyCarIndex]];
        glm::vec3 P2 = enemyTrajectoryPoints.at(enemyCarIndex)[currentEnemyPointIndex[enemyCarIndex] + 1];
        enemyDirection = P2 - P1;
        // punctul in care se va translata
        glm::vec3 P = P1 + timeElapsedBetweenPointsEnemy[enemyCarIndex] * enemyDirection;

        // verific daca am iesit de pe segmentul curent si trebuie sa schimb directia
        bool shouldChangeDirection = false;
        if (P1.z <= P2.z && P1.x <= P2.x) {
            if (P.z >= P2.z && P.x >= P2.x) {
                shouldChangeDirection = true;
            }
        }
        else if (P1.z >= P2.z && P1.x <= P2.x) {
            if (P.z <= P2.z && P.x >= P2.x) {
                shouldChangeDirection = true;
            }
        }
        else if (P1.z <= P2.z && P1.x >= P2.x) {
            if (P.z >= P2.z && P.x <= P2.x) {
                shouldChangeDirection = true;
            }
        }
        else if (P1.z >= P2.z && P1.x >= P2.x) {
            if (P.z <= P2.z && P.x <= P2.x) {
                shouldChangeDirection = true;
            }
        }

        // daca trebuie sa schimb directia
        if (shouldChangeDirection == true) {
            currentEnemyPointIndex[enemyCarIndex]++;
            timeElapsedBetweenPointsEnemy[enemyCarIndex] = 0;
            if (enemyTrajectoryPoints.at(enemyCarIndex).size() - 1 == currentEnemyPointIndex[enemyCarIndex]) {
                currentEnemyPointIndex[enemyCarIndex] = 0;
            }

            P1 = enemyTrajectoryPoints.at(enemyCarIndex)[currentEnemyPointIndex[enemyCarIndex]];
            P2 = enemyTrajectoryPoints.at(enemyCarIndex)[currentEnemyPointIndex[enemyCarIndex] + 1];
            enemyDirection = P2 - P1;
        }
        // adun la time elapsed intre doua puncte deltatimeseconds * viteza masinii
        timeElapsedBetweenPointsEnemy[enemyCarIndex] += deltaTimeSeconds * speed;
        // calculez punctul in care se translateaza masina
        P = P1 + timeElapsedBetweenPointsEnemy[enemyCarIndex] * enemyDirection;
        translateEnemy[enemyCarIndex] = P;
        // calculez rotatia masinii inamice
        enemyRotation[enemyCarIndex] = atan2(enemyDirection.x, enemyDirection.z) + M_PI;
    }
}

// generare traiectorie inamici
void enemyGenerator::generateEnemyTrajectory(int start, int enemyCarIndex, float trajectoryDeviation,
    std::vector<glm::vec3> initialTrajectoryVertices,
    std::unordered_map<int, std::vector<glm::vec3>>& enemyTrajectoryPoints)
{
    std::vector<glm::vec3> preliminaryTrajectoryPoints;
    std::vector<glm::vec3> enemyTrajectoryPointsVect;

    // calculez punctele paralele cu pista care se afla la o anumita
    // deviatie de segmentele din mijloc ale acesteia
    for (int i = 0; i < initialTrajectoryVertices.size() - 1; i++) {
        glm::vec3 P1_1 = initialTrajectoryVertices[i];
        glm::vec3 P2_1 = initialTrajectoryVertices[i + 1];
        glm::vec3 D_1 = P2_1 - P1_1;
        glm::vec3 UP = glm::vec3(0, 1, 0);
        glm::vec3 P_1 = glm::cross(D_1, UP);
        glm::vec3 R_1 = P1_1 + trajectoryDeviation * P_1;

        preliminaryTrajectoryPoints.push_back(R_1);
    }

    // intr-un alt vector, adaug punctele calculate antrior intr-un alt vector,
    // asigurandu-ma ca e circular fata de punctul de unde porneste masina
    for (int i = start; i < preliminaryTrajectoryPoints.size(); i++) {
        enemyTrajectoryPointsVect.push_back(preliminaryTrajectoryPoints[i]);
    }
    for (int i = 0; i < start; i++) {
        enemyTrajectoryPointsVect.push_back(preliminaryTrajectoryPoints[i]);
    }
    enemyTrajectoryPointsVect.push_back(enemyTrajectoryPointsVect[0]);

    // adaugare vector in hashmap-ul cu punctele de traiectorie ale masinilor,
    // la cheia ce refera masina curenta
    enemyTrajectoryPoints.insert({enemyCarIndex, enemyTrajectoryPointsVect});
}
