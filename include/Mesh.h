#include <vector>

// Pour fenetre et polygone
struct Mesh {
	std::vector<Eigen::Vector2f> mesh;

	void addVertex(float x, float y) {
		mesh.push_back({ x, y });
	}
};