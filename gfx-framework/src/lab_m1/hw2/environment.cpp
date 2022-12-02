#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "core/managers/resource_path.h"
#include "utils/text_utils.h"
#include "environment.h"

using namespace std;
using namespace environment;

// Constructors
Environment::Environment() {}
Environment::~Environment() {}

void Environment::addMesh(std::string mesh_name, Mesh *mesh) {
    meshes[mesh_name] = mesh;
}


void Environment::generateMeshes() {
    /* Ground */
    Mesh *ground = new Mesh("ground-plane50");
	ground->LoadMesh(RESOURCE_PATH::MODELS + "/primitives", "plane50.obj");
    addMesh("ground-plane50", ground);

    /* Obstacle */
    Mesh *obstacle = new Mesh("obstacle");
    obstacle->LoadMesh(RESOURCE_PATH::MODELS + "/primitives", "box.obj");
    addMesh("obstacle", obstacle);
}


void Environment::generateCube(const char *name) {
    Mesh *cube = new Mesh(name);
    cube->LoadMesh(RESOURCE_PATH::MODELS + "/primitives", "box.obj");
    addMesh(name, cube);
}


void Environment::generateTrack() {
    // Read a set of points for the middle points of car track
    // Read from `hw2/track_points.txt` file
    std::ifstream trackPointsFile;
    trackPointsFile.open(SOURCE_PATH::M1 + "/hw2/track_points.txt");

    if (trackPointsFile.is_open()) {
        std::string line;
        while (std::getline(trackPointsFile, line)) {
            std::istringstream iss(line);
            float x, z;
            if (!(iss >> x >> z)) {
                break;
            }
			trackMiddlePoints.push_back(glm::vec3(x, 0, z));
        }
    }

    // Generate the interior and exterior points of the track using 3 consecutive middle points
    vector<glm::vec3> trackPoints;
    for (int i = 0; i < trackMiddlePoints.size(); i++) {
        glm::vec3 p1 = trackMiddlePoints[i];
        glm::vec3 p2 = trackMiddlePoints[(i + 1) % trackMiddlePoints.size()];
        glm::vec3 p3 = trackMiddlePoints[(i + 2) % trackMiddlePoints.size()];

        glm::vec3 v1 = glm::normalize(p2 - p1); // Direction vector from p1 to p2
        glm::vec3 v2 = glm::normalize(p3 - p2); // Direction vector from p2 to p3

        glm::vec3 n1 = glm::normalize(glm::cross(v1, glm::vec3(0, 1, 0))); // Normal vector to v1 (in XOZ plane)
        glm::vec3 n2 = glm::normalize(glm::cross(v2, glm::vec3(0, 1, 0))); // Normal vector to v2 (in XOZ plane)

        glm::vec3 n = glm::normalize(n1 + n2); // Average of the two normal vectors size

        trackPoints.push_back(p1 + n * 0.5f); // Interior point
        trackPoints.push_back(p1 - n * 0.5f); // Exterior point
    }

    // Complete the triangles vector with points from `trackPoints`
    for (int i = 0; i < trackPoints.size(); i++) {
        vector<glm::vec3> triangle;
        triangle.push_back(trackPoints[i]);
        triangle.push_back(trackPoints[(i + 1) % trackPoints.size()]);
        triangle.push_back(trackPoints[(i + 2) % trackPoints.size()]);
        triangles.push_back(triangle);
    }

    // Multiply each triangle's point with `trackScale`
    for (auto &triangle: triangles) {
        for (auto &point: triangle) {
            point *= trackScale;
        }
    }

    // Generate trees' positions
    for (int i = 0; i < trackPoints.size(); i++) {
        glm::vec3 p1 = trackPoints[i];
        glm::vec3 p2 = trackPoints[(i + 1) % trackPoints.size()];
        glm::vec3 p3 = trackPoints[(i + 2) % trackPoints.size()];

        glm::vec3 v1 = glm::normalize(p2 - p1); // Direction vector from p1 to p2
        glm::vec3 v2 = glm::normalize(p3 - p2); // Direction vector from p2 to p3
        
        glm::vec3 n1 = glm::normalize(glm::cross(v1, glm::vec3(0, 1, 0))); // Normal vector to v1 (in XOZ plane)
        glm::vec3 n2 = glm::normalize(glm::cross(v2, glm::vec3(0, 1, 0))); // Normal vector to v2 (in XOZ plane)

        glm::vec3 n = glm::normalize(n1 + n2); // Average of the two normal vectors size

        // Move the tree a little bit to the left or right
        glm::vec3 treePosition = p1 * trackScale;
        if (i % 2 == 0)
            treePosition += n * 2.0f;
        else
            treePosition -= n * 2.0f;

        treePositions.push_back(treePosition);
    }

    // Generate the track mesh
    std::vector<VertexFormat> vertices = generateTrackMesh(trackPoints);

    // Remove trees that are too close to the track
    for (int i = 0; i < treePositions.size(); i++) {
        glm::vec3 treePosition = treePositions[i];
        for (auto &vertex: vertices) {
            if (glm::distance(treePosition, vertex.position * trackScale) < 1.5f) {
                treePositions.erase(treePositions.begin() + i);
                i--;
                break;
            }
        }
    }

    // Remove trees that are too close to each other
    for (int i = 0; i < treePositions.size(); i++) {
        glm::vec3 treePosition = treePositions[i];
        for (int j = 0; j < treePositions.size(); j++) {
            if (i == j)
                continue;
            if (glm::distance(treePosition, treePositions[j]) < 1.5f) {
                treePositions.erase(treePositions.begin() + i);
                i--;
                break;
            }
        }
    }

    // Keep only 75% of the trees
    int treesToKeep = treePositions.size() * 0.75f;
    while (treePositions.size() > treesToKeep) {
        int index = rand() % treePositions.size();
        treePositions.erase(treePositions.begin() + index);
    }

    // Give to each tree a random size between 1.2 and 1.7
    for (int i = 0; i < treePositions.size(); i++)
        treeSizes.push_back(1.2f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.7f - 1.2f))));
}


std::vector<VertexFormat> Environment::generateTrackMesh(vector<glm::vec3> trackPoints) {
    // Create the track
    Mesh* track = new Mesh("track");
    addMesh("track", track);

    // Create the vertices buffer
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    // Iterate over `trackPoints`, take 3 consecutive points and create a triangle
    for (int i = 0; i < trackPoints.size(); i++) {
        glm::vec3 p1 = trackPoints[i % trackPoints.size()];
        glm::vec3 p2 = trackPoints[(i + 1) % trackPoints.size()];
        glm::vec3 p3 = trackPoints[(i + 2) % trackPoints.size()];

        glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

        // Split the p2-p3 edge in `n_parts`and create `n_parts` triangles
        float n_parts = 30;
        for (int j = 0; j < n_parts; j++) {
            glm::vec3 p4 = p2 + (p3 - p2) * (float) (j + 1) / n_parts;
            glm::vec3 p5 = p2 + (p3 - p2) * (float) (j + 0) / n_parts;

            // This random color is used for debugging purposes (to visualize the triangles)
            glm::vec3 color = glm::vec3(rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f);
            vertices.push_back(VertexFormat(p1, normal, color));
            vertices.push_back(VertexFormat(p4, normal, color));
            vertices.push_back(VertexFormat(p5, normal, color));

            indices.push_back(i * n_parts * 3 + j * 3 + 0);
            indices.push_back(i * n_parts * 3 + j * 3 + 1);
            indices.push_back(i * n_parts * 3 + j * 3 + 2);
        }

        // Create another triangle to have more vertices
        i++;
        glm::vec3 p4 = trackPoints[i % trackPoints.size()];
        p3 = trackPoints[(i - 2) % trackPoints.size()];
        p2 = trackPoints[(i - 1) % trackPoints.size()];
        if (i == 1) {
            p2 = trackPoints[0];
            p3 = trackPoints[trackPoints.size() - 1];
        }

        normal = glm::normalize(glm::cross(p2 - p4, p3 - p4));

        // Split the p2-p3 edge in `n_parts`and create `n_parts` triangles
        for (int j = 0; j < n_parts; j++) {
            glm::vec3 p1 = p2 + (p3 - p2) * (float) (j + 1) / n_parts;
            glm::vec3 p5 = p2 + (p3 - p2) * (float) (j + 0) / n_parts;

            // This random color is used only for debugging purposes (to visualize the triangles)
            glm::vec3 color = glm::vec3(rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f);
            vertices.push_back(VertexFormat(p1, normal, color));
            vertices.push_back(VertexFormat(p4, normal, color));
            vertices.push_back(VertexFormat(p5, normal, color));

            indices.push_back(i * n_parts * 3 + j * 3 + 0);
            indices.push_back(i * n_parts * 3 + j * 3 + 1);
            indices.push_back(i * n_parts * 3 + j * 3 + 2);
        }
    }

    // Create the track mesh
    track->SetDrawMode(GL_TRIANGLES);
    track->InitFromData(vertices, indices);

    return vertices;
}


bool Environment::isOnTrack(glm::vec3 center) {
    // Iterate over all big triangles and check if the center is inside any of them
    // Reference: https://www.baeldung.com/cs/check-if-point-is-in-2d-triangle#triangles-area-approach
    for (auto triangle: triangles) {
        // Ignore the y coordinate
        center.y = 0;
        for (auto &point: triangle)
            point.y = 0;

        // Compute the area of the triangle
        float area = glm::length(glm::cross(triangle[1] - triangle[0], triangle[2] - triangle[0])) / 2;

        // Compute the area of the 3 sub-triangles
        float area1 = glm::length(glm::cross(triangle[1] - triangle[0], center - triangle[0])) / 2;
        float area2 = glm::length(glm::cross(triangle[2] - triangle[1], center - triangle[1])) / 2;
        float area3 = glm::length(glm::cross(triangle[0] - triangle[2], center - triangle[2])) / 2;

        // If the sum of the areas is equal to the area of the triangle, then the point is inside
        // Check with 0.001 precision
        if (abs(area - (area1 + area2 + area3)) < 0.001f)
            return true;
    }

    return false;
}


void Environment::generateObstacles() {
    // Ensure that `max_obstacles` is not greater than the number of `trackMiddlePoints`
    max_obstacles = std::min((int) max_obstacles, (int) trackMiddlePoints.size());

    // Generate `max_obstacles` unique random indices from `trackMiddlePoints`
    while (obstacleIdxs.size() < max_obstacles) {
        int index = rand() % trackMiddlePoints.size();
        if (std::find(obstacleIdxs.begin(), obstacleIdxs.end(), index) == obstacleIdxs.end())
            obstacleIdxs.push_back(index);
    }

    // Remove obstacles that are too close to each other
    for (int i = 0; i < obstacleIdxs.size(); i++) {
        for (int j = i + 1; j < obstacleIdxs.size(); j++) {
            if (glm::distance(trackMiddlePoints[obstacleIdxs[i]], trackMiddlePoints[obstacleIdxs[j]]) < 1.0f) {
                obstacleIdxs.erase(obstacleIdxs.begin() + j);
                j--;
            }
        }
    }

    // Initialize `obstaclePositions` with MAX_INT
    for (int i = 0; i < max_obstacles; i++)
        obstaclePositions.push_back(glm::vec3(INT_MAX, INT_MAX, INT_MAX));

    // Give some random offsets to the obstacles in range [-1.5, 1.5]
    for (int i = 0; i < max_obstacles; i++)
        obstacleOffsets.push_back(glm::vec3(rand() % 300 / 100.0f - 1.5f, 0, rand() % 300 / 100.0f - 1.5f));

    // Give some random colors to the obstacles
    for (int i = 0; i < max_obstacles; i++)
        obstacleColors.push_back(glm::vec3(rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f));

    // Give some random scales to the obstacles in range [1.1, 1.5]
    for (int i = 0; i < max_obstacles; i++)
        obstacleScales.push_back(1.1f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(1.5f - 1.1f))));

}


bool Environment::isOnCollision(glm::vec3 center) {
    // Check if car's center is on colision with any `obstaclePosition`
    for (int i = 0; i < obstaclePositions.size(); i++) {
        glm::vec3 obstaclePosition = obstaclePositions[i];
        if (glm::distance(center, obstaclePosition + obstacleOffsets[i]) < 1.0f)
            return true;
    }
    return false;
}
