#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"
#include <unordered_map>

namespace enemyGenerator
{

    void generateEnemyTrajectory(int start, int enemyCarIndex, float trajectoryDeviation,
        std::vector<glm::vec3> initialTrajectoryVertices,
        std::unordered_map<int, std::vector<glm::vec3>>& enemyTrajectoryPoints);
    void updateEnemyPosition(float deltaTimeSeconds, int enemyCarIndex, float speed,
        std::vector<glm::vec3>& translateEnemy,
        std::vector <int>& currentEnemyPointIndex,
        std::vector <float>& timeElapsedBetweenPointsEnemy,
        std::unordered_map<int, std::vector<glm::vec3>>& enemyTrajectoryPoints,
        std::vector <float>& enemyRotation);
    void generateEnemyCars(std::vector<glm::vec3>& translateEnemy,
        std::vector <int>& currentEnemyPointIndex,
        std::vector <float>& timeElapsedBetweenPointsEnemy,
        std::vector<glm::vec3> initialTrajectoryVertices,
        std::unordered_map<int, std::vector<glm::vec3>>& enemyTrajectoryPoints,
        std::vector <float>& enemyRotation);

    void updateEnemyCars(float deltaTimeSeconds, std::vector<glm::vec3>& translateEnemy,
        std::vector <int>& currentEnemyPointIndex,
        std::vector <float>& timeElapsedBetweenPointsEnemy,
        std::unordered_map<int, std::vector<glm::vec3>>& enemyTrajectoryPoints,
        std::vector <float>& enemyRotation);
}
#pragma once
