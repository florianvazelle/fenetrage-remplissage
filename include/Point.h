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
	Point(const Eigen::Vector2f& vec) {
		position = vec;

        glGenBuffers(1, &_vbo);

        float marginY = (1080 * margin) / 1920; 

        hitbox[0] = { position[0] - margin, position[1] - marginY };
        hitbox[1] = { position[0] - margin, position[1] + marginY };
        hitbox[2] = { position[0] + margin, position[1] + marginY };
        hitbox[3] = { position[0] + margin, position[1] - marginY };
	}

    ~Point() {
        glDeleteBuffers(1, &_vbo);
    }

    bool contain(float x, float y) const {
        return ((x >= this->hitbox[0][0]) && (y >= this->hitbox[0][1]) && (x <= this->hitbox[2][0]) && (y <= this->hitbox[2][1]));
    }

    void draw(const uint32_t& shader, const bool editMode, const Eigen::Vector2f& mouse) const;

    void setPosition(const Eigen::Vector2f& coordGL) { 
        position = coordGL;

        float marginY = (1080 * margin) / 1920; 

        hitbox[0] = { position[0] - margin, position[1] - marginY };
        hitbox[1] = { position[0] - margin, position[1] + marginY };
        hitbox[2] = { position[0] + margin, position[1] + marginY };
        hitbox[3] = { position[0] + margin, position[1] - marginY };
    };
	Eigen::Vector2f getPosition() const { return position; };
};

#endif