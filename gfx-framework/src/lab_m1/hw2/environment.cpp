#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "core/managers/resource_path.h"
#include "environment.h"

using namespace std;
using namespace environment;

// Constructors
Environment::Environment() {}
Environment::~Environment() {}

void Environment::addMesh(std::string mesh_name, Mesh *mesh) {
    this->meshes[mesh_name] = mesh;
}


void Environment::addShader(std::string shader_name, Shader *shader) {
    this->shaders[shader_name] = shader;
}


void Environment::generateMeshes() {
    // Skybox
    Mesh *skybox = new Mesh("skybox");
    skybox->LoadMesh(RESOURCE_PATH::MODELS + "/primitives", "box.obj");
    this->addMesh("skybox", skybox);

    // Ground
    Mesh *ground = new Mesh("ground");
    ground->LoadMesh(RESOURCE_PATH::MODELS + "/primitives", "box.obj");
    this->addMesh("ground", ground);
}


void Environment::generateShaders() {
	// Create a shader program for drawing face polygon with the color of the normal
    Shader* shader = new Shader("LabShader");
	shader->AddShader(SOURCE_PATH::M1 + "/hw2/" + "shaders" + "/VertexShader.glsl", GL_VERTEX_SHADER);
	shader->AddShader(SOURCE_PATH::M1 + "/hw2/" + "shaders" + "/FragmentShader.glsl", GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    this->addShader("LabShader", shader);
}


void Environment::generateTrack() {
    // Define a set of points for the middle points of car track
    vector<glm::vec3> trackPointsMiddlePoints;
    trackPointsMiddlePoints.push_back(glm::vec3(5.13,  0,  0.98)); // A
    trackPointsMiddlePoints.push_back(glm::vec3(4.33,  0,  2.21)); // B
    trackPointsMiddlePoints.push_back(glm::vec3(3.13,  0,  3.04)); // C
    trackPointsMiddlePoints.push_back(glm::vec3(1.37,  0,  3.44)); // D
    trackPointsMiddlePoints.push_back(glm::vec3(0.11,  0,  2.75)); // E
    trackPointsMiddlePoints.push_back(glm::vec3(-1.17, 0,  1.85)); // F
    trackPointsMiddlePoints.push_back(glm::vec3(-2.58, 0,  2.15)); // G
    trackPointsMiddlePoints.push_back(glm::vec3(-3.68, 0,  3.02)); // H
    trackPointsMiddlePoints.push_back(glm::vec3(-5.09, 0,  2.98)); // I
    trackPointsMiddlePoints.push_back(glm::vec3(-6.02, 0,  2.16)); // J
    trackPointsMiddlePoints.push_back(glm::vec3(-6.40, 0,  0.98)); // K
    trackPointsMiddlePoints.push_back(glm::vec3(-6.13, 0, -0.45)); // L
    trackPointsMiddlePoints.push_back(glm::vec3(-4.46, 0, -1.31)); // M
    trackPointsMiddlePoints.push_back(glm::vec3(-2.86, 0, -1.53)); // N
    trackPointsMiddlePoints.push_back(glm::vec3(-1.15, 0, -1.01)); // O
    trackPointsMiddlePoints.push_back(glm::vec3(0.17,  0, -0.26)); // P
    trackPointsMiddlePoints.push_back(glm::vec3(1.40,  0, -0.26)); // Q
    trackPointsMiddlePoints.push_back(glm::vec3(2.21,  0, -0.86)); // R
    trackPointsMiddlePoints.push_back(glm::vec3(3.42,  0, -1.65)); // S
    trackPointsMiddlePoints.push_back(glm::vec3(4.47,  0, -1.22)); // T
    trackPointsMiddlePoints.push_back(glm::vec3(5.04,  0, -0.38)); // U

    // Generate the exterior and interior points of the track using 3 consecutive points
    vector<glm::vec3> trackPoints;
    for (int i = 0; i < trackPointsMiddlePoints.size(); i++) {
        glm::vec3 p1 = trackPointsMiddlePoints[i];
        glm::vec3 p2 = trackPointsMiddlePoints[(i + 1) % trackPointsMiddlePoints.size()];
        glm::vec3 p3 = trackPointsMiddlePoints[(i + 2) % trackPointsMiddlePoints.size()];

        glm::vec3 v1 = glm::normalize(p2 - p1); // Direction vector from p1 to p2
        glm::vec3 v2 = glm::normalize(p3 - p2); // Direction vector from p2 to p3

        glm::vec3 n1 = glm::normalize(glm::cross(v1, glm::vec3(0, 1, 0))); // Normal vector to v1 (in XOZ plane)
        glm::vec3 n2 = glm::normalize(glm::cross(v2, glm::vec3(0, 1, 0))); // Normal vector to v2 (in XOZ plane)

        glm::vec3 n = glm::normalize(n1 + n2); // Average of the two normal vectors size

        trackPoints.push_back(p1 + n * 0.5f); // Exterior point
        trackPoints.push_back(p1 - n * 0.5f); // Interior point
    }

    // Complete the triangles vector with points from `trackPoints`
    for (int i = 0; i < trackPoints.size(); i++) {
        vector<glm::vec3> triangle;
        triangle.push_back(trackPoints[i]);
        triangle.push_back(trackPoints[(i + 1) % trackPoints.size()]);
        triangle.push_back(trackPoints[(i + 2) % trackPoints.size()]);
        this->triangles.push_back(triangle);
    }
    
    // Generate the track mesh
    generateTrackMesh(trackPoints, trackColor);
}


void Environment::generateTrackMesh(vector<glm::vec3> trackPoints, glm::vec3 color) {
    // Create the track
    Mesh* track = new Mesh("track");
    this->addMesh("track", track);

    // Create the vertices buffer
    vector<VertexFormat> vertices;
    vector<unsigned int> indices;

    // Iterate over trackPoints, take 3 consecutive points and create a triangle
    for (int i = 0; i < trackPoints.size(); i++) {
        glm::vec3 p1 = trackPoints[i];
        glm::vec3 p2 = trackPoints[(i + 1) % trackPoints.size()];
        glm::vec3 p3 = trackPoints[(i + 2) % trackPoints.size()];

        glm::vec3 v1 = p2 - p1;
        glm::vec3 v2 = p3 - p2;

        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

        // Split the p2-p3 edge in `n_parts`and create `n_parts` triangles
        float n_parts = 50;
        for (int j = 0; j < n_parts; j++) {
            glm::vec3 p4 = p2 + (p3 - p2) * (float) (j + 1) / n_parts;
            glm::vec3 p5 = p2 + (p3 - p2) * (float) (j + 0) / n_parts;

            // This random color is used only for debugging purposes (to visualize the triangles)
            // glm::vec3 color = glm::vec3(rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f);
            vertices.push_back(VertexFormat(p1, normal, color));
            vertices.push_back(VertexFormat(p4, normal, color));
            vertices.push_back(VertexFormat(p5, normal, color));

            indices.push_back(i * n_parts * 3 + j * 3 + 0);
            indices.push_back(i * n_parts * 3 + j * 3 + 1);
            indices.push_back(i * n_parts * 3 + j * 3 + 2);
        }
    }

    track->SetDrawMode(GL_TRIANGLES);
    track->InitFromData(vertices, indices);
}
