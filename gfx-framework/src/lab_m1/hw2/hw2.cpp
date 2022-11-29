#include <iostream>
#include <vector>

#include "lab_m1/hw2/hw2.h"
#include "lab_m1/hw2/environment.h"

using namespace std;
using namespace m1;

Hw2::Hw2() {}
Hw2::~Hw2() {}

void Hw2::Init() {
    // Camera
    mainCamera = new cam::Camera();
    mainCamera->Set(glm::vec3(0, 1.5f, -5), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	mainCamera->RotateThirdPerson_OY(RADIANS(-110));

    miniCamera = new cam::Camera();
    miniCamera->Set(glm::vec3(0, 20, -5), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));

    // Environment
    env = environment::Environment();
    env.generateMeshes();
    env.generateTrack();
    env.generateShaders();
    env.generateCube("trunk", env.trunkColor);
    env.generateCube("crown", env.crownColor);

    // Car
    car = car::Car();
    car.generateMeshes();

    // Sets the resolution of the minimap
    glm::ivec2 resolution = window->GetResolution();
    // place the minimap in the bottom rigth corner
    float padding = 20;
    miniViewportArea = ViewportArea(resolution.x - resolution.x / 4 - padding,
                                    padding,
                                    resolution.x / 4,
                                    resolution.y / 4);

    // Projection matrix
	projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, nearZ, farZ);
}


void Hw2::FrameStart() {
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
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


void Hw2::RenderEnvironment(float deltaTimeSeconds) {
    glm::mat4 modelMatrix;
    /* Render trees */
    for (int i = 0; i < env.treePositions.size(); i++) {
        auto treePosition = env.treePositions[i];
        // Trunk matrix
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, treePosition);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.7f, 0.5f));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, 0, -0.5f));

        // Render the trunk
        glUseProgram(shaders["Color"]->program);
        GLint loc_color = glGetUniformLocation(shaders["Color"]->program, "color");
        glUniform3fv(loc_color, 1, glm::value_ptr(env.trunkColor));
        RenderMesh(env.meshes["trunk"], shaders["Color"], modelMatrix);

        // Crown matrix
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, treePosition);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.7f, 0));
        float scale = env.treeSizes[i];
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale, scale, scale));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, 0, -0.5f));

        // Render the crown
        glUseProgram(shaders["Color"]->program);
        loc_color = glGetUniformLocation(shaders["Color"]->program, "color");
        glUniform3fv(loc_color, 1, glm::value_ptr(env.crownColor));
        RenderMesh(env.meshes["crown"], shaders["Color"], modelMatrix);
    }
    
    /* Render ground */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -1.2f, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100, 1, 100));
    glUseProgram(shaders["Color"]->program);
    GLint loc_color = glGetUniformLocation(shaders["Color"]->program, "color");
    glUniform3fv(loc_color, 1, glm::value_ptr(env.groundColor));
    RenderMesh(env.meshes["ground"], shaders["Color"], modelMatrix);

    /* Render a big sphere for skybox */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 10, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100, 100, 100));
    glUseProgram(shaders["Color"]->program);
    loc_color = glGetUniformLocation(shaders["Color"]->program, "color");
    glUniform3fv(loc_color, 1, glm::value_ptr(env.skyColor));
    RenderMesh(env.meshes["skybox"], shaders["Color"], modelMatrix);

    /* Render track */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(env.trackScale));
    glUseProgram(shaders["Color"]->program);
    loc_color = glGetUniformLocation(shaders["Color"]->program, "color");
    glUniform3fv(loc_color, 1, glm::value_ptr(env.trackColor));
    RenderMesh(env.meshes["track"], shaders["Color"], modelMatrix);
}


void Hw2::RenderScene(float deltaTimeSeconds, bool updateCarCenter = true) {
    /* Render the car */
    // Translate the car based on the camera's position, but keep the y on the ground
    glm::mat4 modelMatrix = glm::mat4(1);
    glm::vec3 targetPosition = mainCamera->GetTargetPosition();
    targetPosition.y = 0.5f;
    modelMatrix = glm::translate(modelMatrix, targetPosition);

    // Rotate the car based on the forward vector of the camera
    glm::vec3 forward = mainCamera->forward * glm::vec3(1, 0, 1);
    forward = glm::normalize(forward);
    float angle = glm::acos(glm::dot(forward, glm::vec3(0, 0, 1))); // angle = arccos(cos_angle(forward, OZ))
    if (forward.x < 0)
        angle = -angle;
    modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 1, 0)); // rotate in plane XOZ (around OY)

    // Scale the car
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
    // Render the car with blue color
    glUseProgram(shaders["Color"]->program);
    GLint loc_color = glGetUniformLocation(shaders["Color"]->program, "color");
    glUniform3fv(loc_color, 1, glm::value_ptr(car.color));
    if (updateCarCenter)
        car.center = targetPosition;
    RenderMesh(car.meshes["car"], shaders["Color"], modelMatrix);

    /* Render the environment */
    RenderEnvironment(deltaTimeSeconds);
}


void Hw2::Update(float deltaTimeSeconds) {
    // Third person camera
    // projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, nearZ, farZ);
    camera = mainCamera;
    RenderScene(deltaTimeSeconds);

    // Switch to the second viewport
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    // Minimap
    // projectionMatrix = glm::ortho(left, right, bottom, top, nearZ, farZ);
    miniCamera->Set(car.center + glm::vec3(0, 20, -5), car.center, glm::vec3(0, 0, -1));
    camera = miniCamera;
    miniCamera->RotateFirstPerson_OY(RADIANS(90));
    RenderScene(deltaTimeSeconds, false);
}


void Hw2::FrameEnd() {
	// DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Hw2::OnInputUpdate(float deltaTime, int mods) {
    camera = mainCamera;
    if (window->KeyHold(GLFW_KEY_W)) {
        // Check if the car is on the track
        if (env.IsOnTrack(car.center)) {
            // Simulate a move forward of the camera
            glm::vec3 nextPosition = car.center + glm::normalize(camera->forward) * cameraSpeed * deltaTime * glm::vec3(1, 0, 1);
            if (env.IsOnTrack(nextPosition)) {
                camera->TranslateForward(cameraSpeed * deltaTime);
            }
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        // Check if the car is on the track
        if (env.IsOnTrack(car.center)) {
            // Simulate a move forward of the camera
            glm::vec3 nextPosition = car.center + glm::normalize(camera->forward) * -cameraSpeed * deltaTime * glm::vec3(1, 0, 1);
            if (env.IsOnTrack(nextPosition)) {
                camera->TranslateForward(-cameraSpeed * deltaTime);
            }
        }
    }

    if (window->KeyHold(GLFW_KEY_A))
        camera->RotateThirdPerson_OY(cameraSpeed * deltaTime);
    if (window->KeyHold(GLFW_KEY_D))
        camera->RotateThirdPerson_OY(-cameraSpeed * deltaTime);


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
