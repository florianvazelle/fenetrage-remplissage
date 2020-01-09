#ifndef ENV_H
#define ENV_H

#include <nanogui/nanogui.h>
#include "Mesh.h"
#include "GUI.h"

class Env {
	public:
		nanogui::ref<GUI> app;

		enum Mode : int { no_Operation_mode, edit_Polygon_mode, edit_Window_mode };
		Mode mode = Mode::no_Operation_mode;

		int width, height;

		Eigen::Vector2f mouse = Eigen::Vector2f(0.0f, 0.0f);
		Mesh polygon, window;
		nanogui::Color currentColor = nanogui::Color(Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

		void changeMode(Mode m) {
			static const char* ModeStrings[] = { "no_Operation_mode", "edit_Polygon_mode", "edit_Window_mode" };
			std::cout << "Changement de mode: " << ModeStrings[m] << std::endl;
			mode = m;
		}

		void init(GLFWwindow *window) {
			app = new GUI(this);

			// https://stackoverflow.com/questions/28283724/use-a-member-function-as-callback
			glfwSetWindowUserPointer(window, this);
			glfwSetCursorPosCallback(window, [](GLFWwindow * window, double x, double y) {
				Env* e = static_cast<Env*>(glfwGetWindowUserPointer(window));
				// Appeler a chaque fois que la souris change de position
				e->app->cursorPosCallbackEvent(x, y);
				// Sauvegarde de la position de la souris
				e->mouse[0] = (float)x;
				e->mouse[1] = (float)y;
			});

			glfwSetMouseButtonCallback(window, [](GLFWwindow * window, int button, int action, int modifiers) {
				Env* e = static_cast<Env*>(glfwGetWindowUserPointer(window));
				bool inWidget = e->app->mouseButtonCallbackEvent(button, action, modifiers);
				if (!inWidget && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
					std::cout << "Mouse pos: (" << e->mouse[0] << ", " << e->mouse[1] << ")" << std::endl;
					std::cout << "Click Left!" << std::endl;
					// Convertion Local-to-World [0,width] -> [-1,1] et [0,height] -> [-1,1]
					float xGL = ((e->mouse[0] + 0.5f) / e->width) * 2.0f - 1.0f;
					float yGL = 1.0f - ((e->mouse[1] + 0.5f) / e->height) * 2.0f;
					if (e->mode == e->Mode::edit_Polygon_mode) {
						e->polygon.addVertex({ xGL, yGL });
					}
					else if (e->mode == e->Mode::edit_Window_mode) {
						e->window.addVertex({ xGL, yGL });
					}
				}
			});

			glfwSetKeyCallback(window, [](GLFWwindow * window, int key, int scancode, int action, int mods) {
				static_cast<Env*>(glfwGetWindowUserPointer(window))->app->keyCallbackEvent(key, scancode, action, mods);
			});

			glfwSetCharCallback(window, [](GLFWwindow * window, unsigned int codepoint) {
				static_cast<Env*>(glfwGetWindowUserPointer(window))->app->charCallbackEvent(codepoint);
			});

			glfwSetDropCallback(window, [](GLFWwindow * window, int count, const char **filenames) {
				static_cast<GUI*>(glfwGetWindowUserPointer(window))->dropCallbackEvent(count, filenames);
			});

			glfwSetScrollCallback(window, [](GLFWwindow * window, double x, double y) {
				static_cast<GUI*>(glfwGetWindowUserPointer(window))->scrollCallbackEvent(x, y);
			});

			glfwSetFramebufferSizeCallback(window, [](GLFWwindow * window, int width, int height) {
				static_cast<GUI*>(glfwGetWindowUserPointer(window))->resizeCallbackEvent(width, height);
			});
		}
};

#endif
