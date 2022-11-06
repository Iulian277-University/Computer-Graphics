#include <iostream>
#include <vector>

#include "hw1.h"

#include "lab_m1/hw1/transform2D.h"
#include "lab_m1/hw1/object2D.h"

#include "duck.h"

#include <glm/gtx/string_cast.hpp> // print glm mat

//using namespace std;
using namespace m1;

Hw1::Hw1() {
}


Hw1::~Hw1() {
}


void Hw1::Init() {
	resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	// Create Duck
	duck_id = 0;
	duck = Duck(duck_id);
	duck.generateMeshes();
}


void Hw1::FrameStart() {
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}


void Hw1::RenderDuck(float deltaTimeSeconds) {
	// Wall reflection
	if (duck.cy > resolution.y || duck.cy < 0)
		duck.dy_sign *= -1.0f;
	if (duck.cx > resolution.x || duck.cx < 0)
		duck.dx_sign *= -1.0f;

	// Trajectory
	float speed = 200.0f;
	float dx = duck.dx_sign * speed * deltaTimeSeconds;
	float dy = duck.dy_sign * speed * deltaTimeSeconds;
	float length = sqrt(dx * dx + dy * dy);
	dx /= length;
	dy /= length;
	duck.curr_x -= duck.angle_sign * speed * dx * deltaTimeSeconds;
	duck.curr_y += speed * dy * deltaTimeSeconds;

	// General matrix
	glm::mat3 general_mat = glm::mat3(1);
	 general_mat *= transform2D::Translate(200, 200);
	general_mat *= transform2D::Translate(duck.curr_x, duck.curr_y);
	general_mat *= transform2D::Rotate(duck.start_angle);
	if (duck.angle_sign > 0) // mirror the duck if the starting angle is positive (to the left)
		general_mat *= transform2D::Mirror_OY();
	general_mat *= transform2D::Rotate(RADIANS(90)); // rotate the duck vertically
	
	// Wall reflection matrix
	if (duck.dx_sign * duck.dy_sign < 0)
		general_mat *= transform2D::Rotate(RADIANS(-90));
	if (duck.dx_sign < 0)
		general_mat *= transform2D::Mirror_OY();

	duck.general_matrix = general_mat;


	// Meshes
	auto duck_meshes = duck.getMeshes();
	Mesh *body		 = duck_meshes["body"];
	Mesh *wing_left  = duck_meshes["wing_left"];
	Mesh *wing_right = duck_meshes["wing_right"];
	Mesh *head		 = duck_meshes["head"];
	Mesh *bake		 = duck_meshes["beak"];
	Mesh *eye		 = duck_meshes["eye"];
	Mesh *iris		 = duck_meshes["iris"];
	Mesh *bbox		 = duck_meshes["bbox"];

	// Iris
	glm::mat3 iris_mat = duck.iris_mat();
	RenderMesh2D(iris, shaders["VertexColor"], general_mat * iris_mat);

	// Eye
	glm::mat3 eye_mat = duck.eye_mat();
	RenderMesh2D(eye, shaders["VertexColor"], general_mat * eye_mat);

	// Head
	glm::mat3 head_mat = duck.head_mat();
	RenderMesh2D(head, shaders["VertexColor"], general_mat * head_mat);

	// Beak
	glm::mat3 bake_mat = duck.beak_mat();
	RenderMesh2D(bake, shaders["VertexColor"], general_mat * bake_mat);
	// std::cout << duck.beak_tip_x << ", " << duck.beak_tip_y << "\n";

	// Body
	glm::mat3 body_mat = duck.body_mat();
	RenderMesh2D(body, shaders["VertexColor"], general_mat * body_mat);

	// Wings animation
	if (duck.wing_pos_rot_angle)
		duck.wing_rot_angle += 1.0f * deltaTimeSeconds;
	else
		duck.wing_rot_angle -= 1.0f * deltaTimeSeconds;

	if (duck.wing_rot_angle >  RADIANS(20))
		duck.wing_pos_rot_angle = false;
	if (duck.wing_rot_angle <  RADIANS(0))
		duck.wing_pos_rot_angle = true;

	// Left wing
	glm::mat3 wing_left_mat = duck.wing_left_mat();
	RenderMesh2D(wing_left, shaders["VertexColor"], general_mat * wing_left_mat);

	// Right wing
	glm::mat3 wing_right_mat = duck.wing_right_mat();
	RenderMesh2D(wing_right, shaders["VertexColor"], general_mat * wing_right_mat);

	// Bounding box
	glm::mat3 bbox_mat = duck.bbox_mat();
	RenderMesh2D(bbox, shaders["VertexColor"], general_mat * bbox_mat);
	
	// bbox coordinates
	//std::cout << "(" << duck.x1 << ", " << duck.y1 << ") ";
	//std::cout << "(" << duck.x2 << ", " << duck.y2 << ") ";
	//std::cout << "(" << duck.x3 << ", " << duck.y3 << ") ";
	//std::cout << "(" << duck.x4 << ", " << duck.y4 << ")\n";
}


void Hw1::Update(float deltaTimeSeconds) {
	RenderDuck(deltaTimeSeconds);
}


void Hw1::FrameEnd() {}


void Hw1::OnInputUpdate(float deltaTime, int mods) {}

void Hw1::OnKeyPress(int key, int mods) {}

void Hw1::OnKeyRelease(int key, int mods) {}

void Hw1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}


void Hw1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
	float x = mouseX;
	float y = resolution.y - mouseY;

	if (button == GLFW_MOUSE_BUTTON_2) {
		float d12 = (x - duck.x1) * (duck.y1 - duck.y2) + (y - duck.y1) * (duck.x2 - duck.x1);
		float d23 = (x - duck.x2) * (duck.y2 - duck.y3) + (y - duck.y2) * (duck.x3 - duck.x2);
		float d34 = (x - duck.x3) * (duck.y3 - duck.y4) + (y - duck.y3) * (duck.x4 - duck.x3);
		float d41 = (x - duck.x4) * (duck.y4 - duck.y1) + (y - duck.y4) * (duck.x1 - duck.x4);

		// Inside the bbox
		if ((d12 > 0 && d23 > 0 && d34 > 0 && d41 > 0) || //		 clock-wise winding
			(d12 < 0 && d23 < 0 && d34 < 0 && d41 < 0)) { // counter clock-wise winding
			std::cout << "inside bbox\n";
		}
	}
}


void Hw1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
	// Add mouse button release event
}


void Hw1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}


void Hw1::OnWindowResize(int width, int height) {
}
