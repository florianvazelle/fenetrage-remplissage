#ifndef ENV_H
#define ENV_H

#include <nanogui/nanogui.h>
#include "Mesh.h"

struct Env {
	enum Mode : int { no_Operation_mode, edit_Polygon_mode, edit_Window_mode };
	Mode mode = Mode::no_Operation_mode;

	int width, height;

	Eigen::Vector2f mouse = Eigen::Vector2f(0.0f, 0.0f);
	Mesh polygon, cutWindow;
	nanogui::Color currentColor = nanogui::Color(Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

	void changeMode(Mode m) {
		static const char* ModeStrings[] = { "no_Operation_mode", "edit_Polygon_mode", "edit_Window_mode" };
		std::cout << "Changement de mode: " << ModeStrings[m] << std::endl;
		mode = m;
	}
};

extern struct Env myenv;

#endif
