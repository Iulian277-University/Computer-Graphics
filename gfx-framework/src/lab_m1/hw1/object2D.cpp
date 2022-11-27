#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = {0, 1, 2, 3};

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}


Mesh* object2D::CreateRectangle(
	const std::string &name,
	glm::vec3 leftBottomCorner,
	float width,
	float height,
	glm::vec3 color,
	bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(width, 0, 0), color),
		VertexFormat(corner + glm::vec3(width, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, 0), color)
	};

	Mesh* rectangle = new Mesh(name);
	std::vector<unsigned int> indices = {0, 1, 2, 3};

	if (!fill) {
		rectangle->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// Draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	rectangle->InitFromData(vertices, indices);
	return rectangle;
}


Mesh* object2D::CreateTriangle(
	const std::string &name,
	glm::vec3 leftBottomCorner,
	float width,
	float height,
	glm::vec3 color,
	bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(width, height / 2, 0), color),
		VertexFormat(corner + glm::vec3(0, height, 0), color)
	};

	Mesh* triangle = new Mesh(name);
	std::vector<unsigned int> indices = {0, 1, 2};

	if (!fill) {
		triangle->SetDrawMode(GL_LINE_LOOP);
	}

	triangle->InitFromData(vertices, indices);
	return triangle;
}


Mesh* object2D::CreateCircle(
	const std::string &name,
	glm::vec3 center,
	int num_triangles,
	float radius,
	glm::vec3 color,
	bool fill)
{
	float theta = 2 * M_PI / num_triangles;
	// Circle parametric equation
	// x = radius * cos(angle)
	// y = radius * sin(angle)

	// Vertices
	std::vector<VertexFormat> vertices;
	vertices.push_back(VertexFormat(center, color));
	float angle = 0;
	for (int i = 0; i < num_triangles; ++i) {
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		vertices.push_back(VertexFormat(center + glm::vec3(x, y, 0), color));
		angle -= theta;
	}

	// Indices
	std::vector<unsigned int> indices;
	indices.push_back(0); // center
	for (int i = 1; i <= num_triangles; ++i)
		indices.push_back(i);
	 indices.push_back(1); // close the circle

	Mesh* circle = new Mesh(name);
	if (!fill) {
		indices.erase(indices.begin());
		circle->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		circle->SetDrawMode(GL_TRIANGLE_FAN);
	}

	circle->InitFromData(vertices, indices);
	return circle;
}
