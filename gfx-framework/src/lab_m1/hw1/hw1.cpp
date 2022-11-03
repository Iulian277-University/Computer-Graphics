#include <iostream>
#include <vector>

#include "lab_m1/hw1/hw1.h"
#include "lab_m1/hw1/transform2D.h"
#include "lab_m1/hw1/object2D.h"

#include "duck.h"

using namespace std;
using namespace m1;

Hw1::Hw1() {
}


Hw1::~Hw1() {
}


void Hw1::Init() {
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	// Create Duck
	duck = Duck(curr_duck_id);
	Mesh* duck_body = object2D::CreateTriangle("duck_body", glm::vec3(0, 0, 0), 200, 400, glm::vec3(1, 0, 0), true);
	duck.addMesh("body", duck_body);
	AddMeshToList(duck_body);
}


void Hw1::FrameStart() {
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}


void Hw1::Update(float deltaTimeSeconds) {
	// RenderMesh2D(meshes["square1"], shaders["VertexColor"], glm::mat3(1));
	// RenderMesh2D(duck.getMeshes()["body"], shaders["VertexColor"], glm::mat3(1));
}


void Hw1::FrameEnd() {
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Hw1::OnInputUpdate(float deltaTime, int mods) {
}


void Hw1::OnKeyPress(int key, int mods) {
	// Add key press event
}


void Hw1::OnKeyRelease(int key, int mods) {
	// Add key release event
}


void Hw1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
	// Add mouse move event
}


void Hw1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
	// Add mouse button press event
}


void Hw1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
	// Add mouse button release event
}


void Hw1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}


void Hw1::OnWindowResize(int width, int height) {
}
