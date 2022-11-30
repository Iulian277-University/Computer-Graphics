#pragma once
#include "components/simple_scene.h"
#include "components/transform.h"
#include "lab_m1/hw2/camera.h"
#include "lab_m1/hw2/environment.h"
#include "lab_m1/hw2/car.h"

namespace m1 {
   class Hw2: public gfxc::SimpleScene {
      public:
         struct ViewportArea
         {
             ViewportArea() : x(0), y(0), width(1), height(1) {}
             ViewportArea(int x, int y, int width, int height)
                 : x(x), y(y), width(width), height(height) {}
             int x;
             int y;
             int width;
             int height;
         };

         Hw2();
         ~Hw2();

         void Init() override;

     private:
         void FrameStart() override;
         void Update(float deltaTimeSeconds) override;
         void FrameEnd() override;

         void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

         void OnInputUpdate(float deltaTime, int mods) override;
         void OnKeyPress(int key, int mods) override;
         void OnKeyRelease(int key, int mods) override;
         void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
         void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
         void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
         void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
         void OnWindowResize(int width, int height) override;

         void RenderEnvironment(float deltaTimeSeconds);
         void RenderObstacle(float deltaTimeSeconds, int ithObstacle);
         void RenderScene(float deltaTimeSeconds, bool updateCarCenter);

     protected:
         environment::Environment env;
         car::Car car;
         cam::Camera *camera;
         cam::Camera *mainCamera;
         cam::Camera *miniCamera;
         ViewportArea miniViewportArea;
         glm::mat4 projectionMatrix;

         float minimapPadding = 20.0f;
         
         float window_wid = 1280.0f;
		 float window_hei = 720.0f;

         float fov    =  60.0f;
         float nearZ  =  0.01f;
         float farZ   = 300.0f;

         float cameraSpeed = 7.0f;

         glm::vec3 lightPosition = glm::vec3(0, 20, 0);
         unsigned int materialShininess = 30;
         float materialKd = 0.5f;
         float materialKs = 0.5f;
   };
}