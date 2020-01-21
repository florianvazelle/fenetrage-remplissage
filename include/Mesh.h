#ifndef MESH_H
#define MESH_H

#include <vector>
#include <nanogui/nanogui.h>
#include "Point.h"

typedef std::vector<Point>::iterator iterator_point;

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

    iterator_point contain(float x, float y);
    bool isValid(iterator_point it) const;

    void setColor(nanogui::Color c) { color = c; }
    void addVertex(Eigen::Vector2f vec) { mesh.push_back(Point(vec)); };

    void setClose(bool c) { close = c; };
    bool isClose() const { return close; };

    std::vector<Eigen::Vector2f> getAllPoints() const {
        std::vector<Eigen::Vector2f> allPoints;
        for (Point p : mesh) {
            allPoints.push_back(p.getPosition());
        }
        return allPoints;
    }

    void clear() { mesh.clear(); }
	size_t size() const { return mesh.size(); };
};

#endif