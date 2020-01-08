#ifndef MESH_H
#define MESH_H

#include <vector>
#include <nanogui/nanogui.h>

class Mesh {
private:
	std::vector<Eigen::Vector2f> mesh;
	nanogui::Color color;
	GLuint _vbo;

public:
	void addVertex(Eigen::Vector2f vec);

	void init(void);
    void destroy(void);
    void draw(int width, int height, uint32_t shader, bool includeMouse, Eigen::Vector2f mouse);
	
	int size() const { return mesh.size(); };
	void setColor(nanogui::Color c);
};

#endif