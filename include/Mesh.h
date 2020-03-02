#ifndef MESH_H
#define MESH_H

#include <vector>
#include <nanogui/nanogui.h>
#include "Point.h"

typedef std::vector<Point>::iterator iterator_point;
typedef std::vector<Point>::const_iterator const_iterator_point;

class Mesh {
private:
	GLuint _vbo;
	std::vector<Point> mesh;
	nanogui::Color color;
    bool close = false;

public:
    void init() { glGenBuffers(1, &_vbo); }
    void draw(const int width, const int height, const uint32_t& shader, const bool includeMouse, const bool editMode, const Eigen::Vector2f& mouse) const;
    void destroy();

    iterator_point contain(const float x, const float y);
    bool isValid(iterator_point it) const;
    const_iterator_point contain(const float x, const float y) const;
    bool isValid(const_iterator_point it) const;

    void setColor(const nanogui::Color& c) { color = c; }
    void addVertex(const Eigen::Vector2f& vec) { mesh.push_back(Point(vec)); };

    void setClose(const bool c) { close = c; };
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