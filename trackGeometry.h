#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace track
{
    Mesh* CreateTrack(const std::string& name, glm::vec3 color, std::vector<glm::vec3> initialTrajectoryVertices);
    std::vector<glm::vec3> generateFewerTrackTriangles(std::vector<glm::vec3>& initialTrajectoryVertices);
    bool checkInsideTriangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P);
    void generateTreeSpawnPoints(std::vector<glm::vec3> initialTrajectoryVertices,
        std::vector<glm::vec3>& treeSpawnPoints);
}
#pragma once
