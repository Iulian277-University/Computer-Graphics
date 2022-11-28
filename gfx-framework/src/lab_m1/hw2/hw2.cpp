#include <iostream>
#include <vector>

#include "lab_m1/hw2/hw2.h"

using namespace std;
using namespace m1;


Hw2::Hw2() {}
Hw2::~Hw2() {}


void Hw2::Init() {
    camera = new cam::Camera();
    camera->Set(glm::vec3(0, 1.5f, 5), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("sphere");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("plane");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
    meshes[mesh->GetMeshID()] = mesh;

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "hw2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "hw2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // TODO(student): After you implement the changing of the projection
    // parameters, remove hardcodings of these parameters
	fov = 60.0f;
	nearZ = 0.01f;
	farZ = 200.0f;
	projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, nearZ, farZ);

	right = 3.5f;
	left = -3.5f;
	bottom = 0.01f;
	top = 10.0f;

	perspectiveType = true;

    // Light & material properties
    lightPosition = glm::vec3(0, 10, 0);
    materialShininess = 30;
    materialKd = 0.5;
    materialKs = 0.5;

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

    // Generate the exterior and interior points of the track using only 2 consecutive points
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

    // Create the car track and fill it with white color
    CreateTrack(trackPoints, glm::vec3(0.1, 0.1, 0.1));
}

void Hw2::CreateTrack(vector<glm::vec3> trackPoints, glm::vec3 color) {
    // Create the track
    Mesh* mesh = new Mesh("track");
    meshes[mesh->GetMeshID()] = mesh;

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

        /* This code creates a triangle with the 3 consecutive points */
        // vertices.push_back(VertexFormat(p1, normal, color));
        // vertices.push_back(VertexFormat(p2, normal, color));
        // vertices.push_back(VertexFormat(p3, normal, color));

        // indices.push_back(i * 3 + 0);
        // indices.push_back(i * 3 + 1);
        // indices.push_back(i * 3 + 2);
    }

    mesh->SetDrawMode(GL_TRIANGLES);
    mesh->InitFromData(vertices, indices);
}

void Hw2::FrameStart() {
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Hw2::RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) {
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix,       1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix,      1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}


void Hw2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set light position uniform
    GLint locLightPos = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(locLightPos, 1, glm::value_ptr(lightPosition));

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    GLint locEyePos = glGetUniformLocation(shader->program, "eye_position");
    glUniform3fv(locEyePos, 1, glm::value_ptr(eyePosition));

    // TODO(student): Set material property uniforms (shininess, kd, ks, object color)
    GLint locMaterial = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(locMaterial, materialShininess);

    GLint locMaterialKd = glGetUniformLocation(shader->program, "material_kd");  // diffuse light
    glUniform1f(locMaterialKd, materialKd);

    GLint locMaterialKs = glGetUniformLocation(shader->program, "material_ks");  // specular light
    glUniform1f(locMaterialKs, materialKs);

    GLint locObject = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(locObject, 1, glm::value_ptr(color));

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Hw2::Update(float deltaTimeSeconds)
{
    /* Render target */
    // Translate the target based on the camera's position, but keep the y on the ground
    glm::mat4 modelMatrix = glm::mat4(1);
    glm::vec3 targetPosition = camera->GetTargetPosition();
    targetPosition.y = 0.5f;
    modelMatrix = glm::translate(modelMatrix, targetPosition);

    // Rotate the target based on the forward vector of the camera
    glm::vec3 forward = camera->forward * glm::vec3(1, 0, 1);
    forward = glm::normalize(forward);
    float angle = glm::acos(glm::dot(forward, glm::vec3(0, 0, 1))); // angle = arccos(cos_angle(forward, OZ))
    if (forward.x < 0)
        angle = -angle;
    modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0)); // rotate in plane XOZ (around OY)

    // Scale the target
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    /* Render ground */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -5, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100, 1, 100));
    RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(133, 216, 84) / 255.0f);

    /* Render a big sphere for ambient light */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 10, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100, 100, 100));
    RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, glm::vec3(0, 0.5, 1));

    /* Render light */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, lightPosition);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
    RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);

    /* Render track */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(4.0f));
    RenderMesh(meshes["track"], shaders["VertexNormal"], modelMatrix);
}


void Hw2::FrameEnd() {
    // DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Hw2::OnInputUpdate(float deltaTime, int mods) {
    float cameraSpeed = 3.0f;

    if (window->KeyHold(GLFW_KEY_W))
        camera->TranslateForward(cameraSpeed * deltaTime);
    
    if (window->KeyHold(GLFW_KEY_S))
        camera->TranslateForward(-cameraSpeed * deltaTime);

    if (window->KeyHold(GLFW_KEY_A))
        camera->RotateThirdPerson_OY(cameraSpeed * deltaTime);

    if (window->KeyHold(GLFW_KEY_D))
        camera->RotateThirdPerson_OY(-cameraSpeed * deltaTime);


    // TODO(student): Change projection parameters. Declare any extra
    // variables you might need in the class header. Inspect this file
    // for any hardcoded projection arguments (can you find any?) and
    // replace them with those extra variables.

    // Changing fov
	if (perspectiveType) {
		if (window->KeyHold(GLFW_KEY_R)) {
			fov -= deltaTime * 3.0f;
			projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, nearZ, farZ);
		}
		if (window->KeyHold(GLFW_KEY_Y)) {
			fov += deltaTime * 3.0f;
			projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, nearZ, farZ);
		}
	}

	// Ortho change bottom and top
	if (!perspectiveType) {
		if (window->KeyHold(GLFW_KEY_1)) {
			top += deltaTime * 2.0f;
			bottom += deltaTime * 2.0f;
			projectionMatrix = glm::ortho(left, right, bottom, top, nearZ, farZ);
		}
		if (window->KeyHold(GLFW_KEY_2)) {
			top -= deltaTime * 2.0f;
			bottom -= deltaTime * 2.0f;
			projectionMatrix = glm::ortho(left, right, bottom, top, nearZ, farZ);
		}
	}

}

void Hw2::OnKeyPress(int key, int mods) {
    // Switch projections
    if (key == GLFW_KEY_P)
    {
        perspectiveType = !perspectiveType;
        if (perspectiveType)
            projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, nearZ, farZ);
        else
            projectionMatrix = glm::ortho(left, right, bottom, top, nearZ, farZ);
    }
}

void Hw2::OnKeyRelease(int key, int mods) {}

void Hw2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}

void Hw2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}

void Hw2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}

void Hw2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void Hw2::OnWindowResize(int width, int height) {}
