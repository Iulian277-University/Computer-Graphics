#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab1::Lab1()
{
    // TODO(student): Never forget to initialize class variables!
    clear_colors	 = glm::vec3(0, 0, 0);
	mesh_name_rand   = "box";
	mesh_coordinates = glm::vec3(0, 0, 0);
}


Lab1::~Lab1()
{
}


void Lab1::Init()
{
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
    {
        Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
    }

    // TODO(student): Load some more meshes. The value of RESOURCE_PATH::MODELS
    // is actually a path on disk, go there and you will find more meshes.
	{
		Mesh *mesh = new Mesh("sphere");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh *mesh = new Mesh("teapot");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
}


void Lab1::FrameStart()
{
}


void Lab1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffer

    // TODO(student): Generalize the arguments of `glClearColor`.
    // You can, for example, declare three variables in the class header,
    // that will store the color components (red, green, blue).
    glClearColor(clear_colors.r, clear_colors.g, clear_colors.b, 1);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    // Render the object
    RenderMesh(meshes["box"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));

    // Render the object again but with different properties
	RenderMesh(meshes["box"], glm::vec3(-1, 0.5f, 0));

    // TODO(student): We need to render (a.k.a. draw) the mesh that
    // was previously loaded. We do this using `RenderMesh`. Check the
    // signature of this function to see the meaning of its parameters.
    // You can draw the same mesh any number of times.

	// [TODO]: Move this `sphere` (generalize this vec3)
	RenderMesh(meshes["sphere"], glm::vec3(-2, 0.5f, 0));
	
	RenderMesh(meshes[mesh_name_rand], mesh_coordinates);
}


void Lab1::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    // TODO(student): Add some key hold events that will let you move
    // a mesh instance on all three axes. You will also need to
    // generalize the position used by `RenderMesh`.
	if (window->KeyHold(GLFW_KEY_0)) {
		std::cout << "Holding 0\n";
	}

	// [TODO]: Move one object
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
		return;

	if (window->KeyHold(GLFW_KEY_A))
		mesh_coordinates.x -= 1.0f * deltaTime;
	if (window->KeyHold(GLFW_KEY_D))
		mesh_coordinates.x += 1.0f * deltaTime;

	if (window->KeyHold(GLFW_KEY_W))
		mesh_coordinates.y += 1.0f * deltaTime;
	if (window->KeyHold(GLFW_KEY_S))
		mesh_coordinates.y -= 1.0f * deltaTime;

	if (window->KeyHold(GLFW_KEY_Q))
		mesh_coordinates.z -= 1.0f * deltaTime;
	if (window->KeyHold(GLFW_KEY_E))
		mesh_coordinates.z += 1.0f * deltaTime;
}


void Lab1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F) {
        // TODO(student): Change the values of the color components.
		float rand_r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float rand_g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float rand_b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		clear_colors = { rand_r, rand_g, rand_b };
    }

    // TODO(student): Add a key press event that will let you cycle
    // through at least two meshes, rendered at the same position.
    // You will also need to generalize the mesh name used by `RenderMesh`.
	if (key == GLFW_KEY_R) {
		// Get a random index of mesh
		int rand_idx_mesh = rand() % meshes.size();

		// Get all mesh names
		std::vector<string> mesh_names;
		mesh_names.reserve(meshes.size());
		for (auto kv: meshes)
			mesh_names.push_back(kv.first);

		mesh_name_rand = mesh_names[rand_idx_mesh];
	}
}


void Lab1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
