#ifndef HITBOX_H
#define HITBOX_H

#include "Mesh.h"

class Hitbox : public Mesh {
public:
	Hitbox();
	bool contain(float x, float y);
	void draw(uint32_t shader, Eigen::Vector2f mouse);
};

#endif