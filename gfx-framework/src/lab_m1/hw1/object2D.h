#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D
{
	// Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateRectangle(const std::string &name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = false);
	Mesh* CreateTriangle(const std::string &name, glm::vec3 leftBottomCorner,float height, float width, glm::vec3 color, bool fill = false);
	Mesh* CreateCircle(const std::string &name, glm::vec3 center, int num_triangles, float radius, glm::vec3 color, bool fill = false);
}
