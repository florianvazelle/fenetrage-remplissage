#ifndef GUI_H
#define GUI_H

#include <nanogui/nanogui.h>
#include "Mesh.h"
#include "Hitbox.h"

class GUI : public nanogui::Screen {
	private:
		enum class Mode : int { no_Operation_mode, edit_Polygon_mode, edit_Window_mode };
		Mode mode;

		int width, height;

		Eigen::Vector2f mouse;
		Mesh polygon, cutWindow;
		std::vector<Mesh> polygons;
		std::vector<Hitbox> polygonHitboxes;

		nanogui::Color currentColor;

		bool wantToEditPolygon = false;
		int indicePolygonToModify = 0;

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

		Eigen::Vector2f local2world(float x, float y) const {
			// Convertion Local-to-World [0,width] -> [-1,1] et [0,height] -> [-1,1]
			return {
				((x + 0.5f) / width) * 2.0f - 1.0f,
				1.0f - ((y + 0.5f) / height) * 2.0f
			};
		}
};

#endif