#ifndef MESH_H
#define MESH_H

#include <vector>
#include <nanogui/nanogui.h>
#include "Point.h"

class Mesh {
private:
	GLuint _vbo;
	std::vector<Point> mesh;
	nanogui::Color color;
    bool close = false;

public:
    void init() { glGenBuffers(1, &_vbo); }
    void draw(int width, int height, uint32_t shader, bool includeMouse, Eigen::Vector2f mouse);
    void destroy();

    std::vector<Point>::const_iterator Mesh::contain(float x, float y) const;
    bool isValid(std::vector<Point>::const_iterator it) const;
    void move(std::vector<Point>::const_iterator it, Eigen::Vector2f coordGL);

    void setColor(nanogui::Color c) { color = c; }
    void addVertex(Eigen::Vector2f vec) { mesh.push_back(Point(vec)); };

    void setClose(bool c) { close = c; };
    bool isClose() const { return close; };

    void clear() { mesh.clear(); }
	size_t size() const { return mesh.size(); };
};

#endif