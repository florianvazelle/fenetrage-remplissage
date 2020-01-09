#ifndef MESH_H
#define MESH_H

#include <vector>
#include <nanogui/nanogui.h>

class Mesh {
private:
	GLuint _vbo;
	nanogui::Color color;
	std::vector<Eigen::Vector2f> mesh;

public:
	void addVertex(Eigen::Vector2f vec);

	void init();
    void draw(int width, int height, uint32_t shader, bool includeMouse, Eigen::Vector2f mouse);
	void destroy(void);
	
	size_t size() const { return mesh.size(); };
	void setColor(nanogui::Color c);
};

#endif