#ifndef GUI_H
#define GUI_H

#include <nanogui/nanogui.h>
#include "Env.h"

class GUI : public nanogui::Screen {
	private:
		Env* env;
	public:
		GUI(Env* e) : nanogui::Screen() { env = e };
		void init(GLFWwindow *window);
		void draw();
};

#endif