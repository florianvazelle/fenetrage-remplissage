#ifndef MESH_H
#define MESH_H

#include <vector>

// Pour fenetre et polygone
struct Mesh {
	std::vector<Eigen::Vector2f> mesh;

	void addVertex(Eigen::Vector2f vec) {
		mesh.push_back(vec);
	}
};

#endif