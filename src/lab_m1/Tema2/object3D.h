#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object3D
{

    // Create square with given bottom left corner, length and color
    Mesh* CreateCube(const std::string &name, float length, glm::vec3 color);
    Mesh* CreateMesh(const std::string& name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
    Mesh* CreateCubeNoFill(
        const std::string& name,
        float length,
        glm::vec3 color);
}
