#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace grass
{
    Mesh* CreateGrass(const std::string& name, glm::vec3 color);
}
#pragma once
