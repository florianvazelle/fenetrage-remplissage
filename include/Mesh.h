#include <vector>
#include "Vector2f.h"

// Pour fenetre et polygone
struct Mesh {
	std::vector<Vector2f> mesh;

	void addVertex(float x, float y) {
		mesh.push_back({ x, y });
	}
};