#ifndef GUI_H
#define GUI_H

#include <nanogui/nanogui.h>
#include "Mesh.h"

class GUI : public nanogui::Screen {
	private:
		enum class Mode : int { no_Operation_mode, edit_Polygon_mode, edit_Window_mode };
		Mode mode;

		int width, height;

		Eigen::Vector2f mouse;
		Mesh polygon, cutWindow;
		nanogui::Color currentColor;

	public:
		GUI();

		void init(GLFWwindow* window);
		void draw(uint32_t shader);
		void destroy(void);

		void changeMode(Mode m);
		void defineCallbacks(GLFWwindow* window);

		int getWidth(void) const { return width; };
		void setWidth(int w) { width = w; };
		int getHeight(void) const { return height; };
		void setHeight(int h) { height = h; };
};

#endif