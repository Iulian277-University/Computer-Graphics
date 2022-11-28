#pragma once
#include "components/simple_scene.h"
#include "components/transform.h"
#include "lab_m1/hw2/camera.h"
#include "lab_m1/hw2/environment.h"
#include "lab_m1/hw2/car.h"

namespace m1 {
   class Hw2: public gfxc::SimpleScene {
      public:
         Hw2();
         ~Hw2();

         void Init() override;

     private:
         void FrameStart() override;
         void Update(float deltaTimeSeconds) override;
         void FrameEnd() override;

         void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
         void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);

         void OnInputUpdate(float deltaTime, int mods) override;
         void OnKeyPress(int key, int mods) override;
         void OnKeyRelease(int key, int mods) override;
         void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
         void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
         void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
         void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
         void OnWindowResize(int width, int height) override;

         void RenderEnvironment(float deltaTimeSeconds);

         bool IsInsideTriangle(glm::vec3 center, std::vector<glm::vec3> triangle);

     protected:
         environment::Environment env;
         car::Car car;

         cam::Camera *camera;
         glm::mat4 projectionMatrix;

         float fov    =  60.0f;
         float nearZ  =  0.01f;
         float farZ   = 200.0f;
         float right  =  3.5f;
         float left   = -3.5f;
         float bottom = 0.01f;
         float top    = 10.0f;
         bool  perspectiveType = true;

         float cameraSpeed = 5.0f;

         glm::vec3 lightPosition = glm::vec3(0, 20, 0);
         unsigned int materialShininess = 30;
         float materialKd = 0.5f;
         float materialKs = 0.5f;
   };
}
