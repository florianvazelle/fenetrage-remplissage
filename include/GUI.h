#ifndef GUI_H
#define GUI_H

#include <nanogui/nanogui.h>
#include "Mesh.h"

class GUI : public nanogui::Screen {
	private:
		enum class Mode : int { no_Operation_mode, edit_Polygon_mode, edit_Window_mode };
		Mode mode = Mode::no_Operation_mode;

		int width, height;

		Eigen::Vector2f mouse = Eigen::Vector2f(0.0f, 0.0f);
		Mesh polygon, cutWindow;
		nanogui::Color currentColor = nanogui::Color(Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

	public:
		GUI() : nanogui::Screen() {};
		void init(GLFWwindow *window);
		void draw(uint32_t shader);
		void destroy();

		void changeMode(Mode m);
		void defineCallbacks(GLFWwindow* window);

		int getWidth() const { return width; };
		void setWidth(int w) { width = w; };
		int getHeight() const { return height; };
		void setHeight(int h) { height = h; };
};

#endif