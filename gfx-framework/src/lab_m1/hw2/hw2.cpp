#include <iostream>

#include "lab_m1/hw2/hw2.h"
#include "lab_m1/hw2/environment.h"

using namespace std;
using namespace m1;

Hw2::Hw2() {}
Hw2::~Hw2() {}

void Hw2::Init() {
    // Third person camera
    mainCamera = new cam::Camera();
    mainCamera->Set(glm::vec3(1, 2.8f, -10), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	mainCamera->RotateThirdPerson_OY(RADIANS(-110));
    mainCamera->RotateThirdPerson_OX(RADIANS(-15));

    // Minimap camera
    miniCamera = new cam::Camera();
    miniCamera->Set(glm::vec3(0, 30, -5), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));

    // Environment
    env = environment::Environment();
    env.generateMeshes();
    env.generateTrack();
    env.generateCube("trunk");
    env.generateCube("crown");

    // Obstacles
    env.generateObstacles();

    // Car
    car = car::Car();
    car.generateMeshes();

    // Sets the resolution of the minimap
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(resolution.x - resolution.x / 4 - minimapPadding, // x
                                    minimapPadding, // y
                                    resolution.x / 4, // width
                                    resolution.y / 4); // height

    // Projection matrix
	projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, nearZ, farZ);

    // Create a shader
    Shader* shader = new Shader("CustomShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "hw2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "hw2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;
}


void Hw2::FrameStart() {
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(env.skyColor.r, env.skyColor.g, env.skyColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Hw2::RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, glm::vec3 color) {
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix,       1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix,      1, GL_FALSE, glm::value_ptr(modelMatrix));
    
	// Send for `shaders["Color"]`
    GLint loc_color = glGetUniformLocation(shader->program, "color");
    glUniform3fv(loc_color, 1, glm::value_ptr(color));

	// Send for `shaders["CustomShader"]`
    loc_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(loc_color, 1, glm::value_ptr(color));

    GLint loc_car_position = glGetUniformLocation(shader->program, "car_position");
    glUniform3fv(loc_car_position, 1, glm::value_ptr(car.center));

    mesh->Render();
}


void Hw2::RenderObstacle(float deltaTimeSeconds, int i) {
    int obstacleIdx = env.obstacleIdxs[i];

    // Next obstacle position
    glm::vec3 nextObstaclePosition = env.trackMiddlePoints[(obstacleIdx + 1) % env.trackMiddlePoints.size()] * env.trackScale;

    // Choose between the initial position of the obstacle and the updated one
    if (glm::distance(env.obstaclePositions[i], nextObstaclePosition) >
        glm::distance(env.trackMiddlePoints[obstacleIdx] * env.trackScale, nextObstaclePosition)) {
            env.obstaclePositions[i] = env.trackMiddlePoints[obstacleIdx] * env.trackScale;
    }

    // Obstacle matrix
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.75f, 0));
    modelMatrix = glm::translate(modelMatrix, env.obstacleOffsets[i]);
    modelMatrix = glm::translate(modelMatrix, env.obstaclePositions[i]);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(env.obstacleScales[i]));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.5f, 0));

    // Rotate the obstacle to be parallel to the vector between the current and the next point
    glm::vec3 obstacleDirection = glm::normalize(nextObstaclePosition - env.obstaclePositions[i]);
    glm::vec3 obstacleUp = glm::vec3(0, 1, 0);
    glm::vec3 obstacleRight = glm::normalize(glm::cross(obstacleDirection, obstacleUp));
    glm::vec3 obstacleFront = glm::normalize(glm::cross(obstacleRight, obstacleUp));
    glm::mat4 obstacleRotationMatrix = glm::mat4(1);
    obstacleRotationMatrix[0] = glm::vec4(obstacleRight, 0);
    obstacleRotationMatrix[1] = glm::vec4(obstacleUp, 0);
    obstacleRotationMatrix[2] = glm::vec4(obstacleFront, 0);
    modelMatrix = modelMatrix * obstacleRotationMatrix;


    // Move the obstacle towards the nextObstaclePosition with the same speed between the two points
    float distance = glm::distance(env.obstaclePositions[i], nextObstaclePosition);
    float speed = cameraSpeed * 0.75f;
    float step = speed * deltaTimeSeconds;
    if (step > distance) {
        env.obstaclePositions[i] = nextObstaclePosition;
        env.obstacleIdxs[i] = (env.obstacleIdxs[i] + 1) % env.trackMiddlePoints.size();
    } else {
        glm::vec3 direction = glm::normalize(nextObstaclePosition - env.obstaclePositions[i]);
        env.obstaclePositions[i] += direction * step;
    }

    // Render the obstacle
    RenderMesh(env.meshes["obstacle"], shader, modelMatrix, env.obstacleColors[i]);
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
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -1.2f, 0));

        // Render the trunk
        RenderMesh(env.meshes["trunk"], shader, modelMatrix, env.trunkColor);

        // Crown matrix
        modelMatrix = glm::mat4(1); 
        modelMatrix = glm::translate(modelMatrix, treePosition);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.1f, 0));
        float scale = env.treeSizes[i];
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale, scale, scale));

        // Render the crown
        RenderMesh(env.meshes["crown"], shader, modelMatrix, env.crownColor);
    }
    
    /* Render ground */
    // Render n*n tiles of "ground-plane50"
    float num_tiles = 5; 
    for (int i = -num_tiles; i < num_tiles; i++) {
        for (int j = -num_tiles; j < num_tiles; j++) {
            modelMatrix = glm::translate(glm::mat4(1), glm::vec3(i * num_tiles, -1.0f, j * num_tiles));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(5, 1, 5));
            RenderMesh(env.meshes["ground-plane50"], shader, modelMatrix, env.groundColor);
        }
    }

    /* Render track */
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(env.trackScale));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.1f, 0));
    RenderMesh(env.meshes["track"], shader, modelMatrix, env.trackColor);

    /* Render obstacles */
    for (int i = 0; i < env.obstacleIdxs.size(); i++)
        RenderObstacle(deltaTimeSeconds, i);
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
    if (updateCarCenter)
        car.center = targetPosition;
    RenderMesh(car.meshes["car"], shader, modelMatrix, car.color);

    /* Render the environment */
    RenderEnvironment(deltaTimeSeconds);
}


void Hw2::Update(float deltaTimeSeconds) {
    /* Third person camera */
    camera = mainCamera;
    shader = shaders["CustomShader"];
    projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, nearZ, farZ);
    RenderScene(deltaTimeSeconds);

    /* Switch to the second viewport */
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(resolution.x - resolution.x / 4 - minimapPadding, // x
									minimapPadding, // y
                                    resolution.x / 4, // width
                                    resolution.y / 4); // height
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    /* Minimap */
    miniCamera->Set(car.center + glm::vec3(0, 30, -5), car.center, glm::vec3(0, 0, -1));
    miniCamera->RotateFirstPerson_OY(RADIANS(90));
    camera = miniCamera;
	shader = shaders["Color"];
    projectionMatrix = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, nearZ, farZ);
    RenderScene(deltaTimeSeconds, false);
}

void Hw2::FrameEnd() {}

void Hw2::OnInputUpdate(float deltaTime, int mods) {
    camera = mainCamera;
    if (window->KeyHold(GLFW_KEY_W)) {
        if (env.isOnCollision(car.center))
            return;

        // Check if the car is on the track
        if (env.isOnTrack(car.center)) {
            // Simulate a move forward of the camera
            glm::vec3 nextPosition = car.center + glm::normalize(camera->forward) * cameraSpeed * deltaTime * glm::vec3(1, 0, 1);
            if (env.isOnTrack(nextPosition)) {
                camera->TranslateForward(cameraSpeed * deltaTime);
            }
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        if (env.isOnCollision(car.center))
            return;

        // Check if the car is on the track
        if (env.isOnTrack(car.center)) {
            // Simulate a move backward of the camera
            glm::vec3 nextPosition = car.center + glm::normalize(camera->forward) * -cameraSpeed * deltaTime * glm::vec3(1, 0, 1);
            if (env.isOnTrack(nextPosition)) {
                camera->TranslateForward(-cameraSpeed * deltaTime);
            }
        }
    }

    if (window->KeyHold(GLFW_KEY_A))
        camera->RotateThirdPerson_OY((float) cameraSpeed / 2 * deltaTime);
    if (window->KeyHold(GLFW_KEY_D))
        camera->RotateThirdPerson_OY(-(float) cameraSpeed / 2 * deltaTime);
}


void Hw2::OnKeyPress(int key, int mods) {}

void Hw2::OnKeyRelease(int key, int mods) {}

void Hw2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}

void Hw2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}

void Hw2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}

void Hw2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void Hw2::OnWindowResize(int width, int height) {}
