#ifndef POINT_H
#define POINT_H

#include <nanogui/nanogui.h>

class Point {
private:
    GLuint _vbo;
	Eigen::Vector2f position;
    Eigen::Vector2f hitbox[4];
    const float margin = 0.03f;

public:
	Point(Eigen::Vector2f vec) {
		position = vec;

        glGenBuffers(1, &_vbo);

        hitbox[0] = { position[0] - margin, position[1] - margin };
        hitbox[1] = { position[0] - margin, position[1] + margin };
        hitbox[2] = { position[0] + margin, position[1] + margin };
        hitbox[3] = { position[0] + margin, position[1] - margin };
	}

    ~Point() {
        glDeleteBuffers(1, &_vbo);
    }

    bool contain(float x, float y) {
        return ((x >= this->hitbox[0][0]) && (y >= this->hitbox[0][1]) && (x <= this->hitbox[2][0]) && (y <= this->hitbox[2][1]));
    }

    void draw(uint32_t shader, Eigen::Vector2f mouse);

    void setPosition(Eigen::Vector2f coordGL) { 
        position = coordGL;

        hitbox[0] = { position[0] - margin, position[1] - margin };
        hitbox[1] = { position[0] - margin, position[1] + margin };
        hitbox[2] = { position[0] + margin, position[1] + margin };
        hitbox[3] = { position[0] + margin, position[1] - margin };
    };
	Eigen::Vector2f getPosition() const { return position; };
};

#endif