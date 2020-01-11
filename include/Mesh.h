#ifndef MESH_H
#define MESH_H

#include <vector>
#include <nanogui/nanogui.h>
#include "Point.h"

typedef std::vector<Point>::const_iterator const_iterator_point;

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

    const_iterator_point contain(float x, float y) const;
    bool isValid(const_iterator_point it) const;
    void move(const_iterator_point it, Eigen::Vector2f coordGL);

    void setColor(nanogui::Color c) { color = c; }
    void addVertex(Eigen::Vector2f vec) { mesh.push_back(Point(vec)); };

    void setClose(bool c) { close = c; };
    bool isClose() const { return close; };

    void clear() { mesh.clear(); }
	size_t size() const { return mesh.size(); };
};

#endif