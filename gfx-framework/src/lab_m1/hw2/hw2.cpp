#include <iostream>
#include <vector>

#include "lab_m1/hw2/hw2.h"

using namespace std;
using namespace m1;


Hw2::Hw2() {}
Hw2::~Hw2() {}


void Hw2::Init() {
    camera = new cam::Camera();
    camera->Set(glm::vec3(0, 1.5, 5.0f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("sphere");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
    meshes[mesh->GetMeshID()] = mesh;

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
}


void Hw2::FrameStart() {
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Hw2::Update(float deltaTimeSeconds)
{
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
}


void Hw2::FrameEnd() {
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Hw2::RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) {
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}


void Hw2::OnInputUpdate(float deltaTime, int mods) {
    float cameraSpeed = 1.0f;

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
