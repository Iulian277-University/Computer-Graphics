#pragma once

#include "components/simple_scene.h"
#include "components/text_renderer.h"
#include "duck.h"
#include "ui.h"

using namespace duck;
using namespace ui;

namespace m1 {
	class Hw1 : public gfxc::SimpleScene {
	public:
		Hw1();
		~Hw1();
		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		void RenderDuck(float deltaTimeSeconds);
		void RenderUi(float deltaTimeSeconds);

	protected:
		float window_wid = 1280.0f;
		float window_hei = 720.0f;
		
		bool ended_game = false;
		glm::ivec2 resolution;
		Duck duck;
		Ui ui;
		gfxc::TextRenderer *text_renderer;
	};
}
