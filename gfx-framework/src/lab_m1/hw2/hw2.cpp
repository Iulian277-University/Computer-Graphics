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
    camera = new cam::Camera();
    camera->Set(glm::vec3(0, 1.5f, -5), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	camera->RotateThirdPerson_OY(RADIANS(-110));

    // Environment
    env = environment::Environment();
    env.generateMeshes();
    env.generateTrack();
    env.generateShaders();

    // Car
    car = car::Car();
    car.generateMeshes();

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


void Hw2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color) {
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

    // Set material property uniforms (shininess, kd, ks, object color)
    GLint locMaterial = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(locMaterial, materialShininess);

    GLint locMaterialKd = glGetUniformLocation(shader->program, "material_kd");  // diffuse light
    glUniform1f(locMaterialKd, materialKd);

    GLint locMaterialKs = glGetUniformLocation(shader->program, "material_ks");  // specular light
    glUniform1f(locMaterialKs, materialKs);

    GLint locObject = glGetUniformLocation(shader->program, "object_color"); // color
    glUniform3fv(locObject, 1, glm::value_ptr(color));

    // Bind `M` matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind `V` matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind `P` matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Hw2::RenderEnvironment(float deltaTimeSeconds) {
    /* Render ground */
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -5, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100, 1, 100));
    RenderSimpleMesh(env.meshes["ground"], env.shaders["LabShader"], modelMatrix, env.groundColor);

    /* Render a big sphere for skybox */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 10, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100, 100, 100));
    RenderSimpleMesh(env.meshes["skybox"], env.shaders["LabShader"], modelMatrix, env.skyColor);

    /* Render light */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, lightPosition);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
    RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);

    /* Render track */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(env.trackScale));
    RenderMesh(env.meshes["track"], shaders["VertexNormal"], modelMatrix);
}


void Hw2::Update(float deltaTimeSeconds) {
    /* Render car */
    // Translate the car based on the camera's position, but keep the y on the ground
    glm::mat4 modelMatrix = glm::mat4(1);
    glm::vec3 targetPosition = camera->GetTargetPosition();
    targetPosition.y = 0.5f;
    modelMatrix = glm::translate(modelMatrix, targetPosition);

    // Rotate the car based on the forward vector of the camera
    glm::vec3 forward = camera->forward * glm::vec3(1, 0, 1);
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
    car.center = targetPosition;
    RenderMesh(car.meshes["car"], shaders["Color"], modelMatrix);

    RenderEnvironment(deltaTimeSeconds);   
}


void Hw2::FrameEnd() {
    // DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Hw2::OnInputUpdate(float deltaTime, int mods) {
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
