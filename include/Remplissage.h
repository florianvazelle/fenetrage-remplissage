#ifndef REMPLISSAGE_H
#define REMPLISSAGE_H

#include <vector>
#include <nanogui/nanogui.h>
#include <math.h>

#include "GLShader.h"
#include "Mesh.h"
#include "Node.h"
#include "CustomList.h"

class Remplissage {
public:
	void initRemplissage(uint32_t shader);
	void displayRemplissage(int width, int height) const;
	void destroyRemplissage();

	void updateTexture(int y, const List<Node>& L, int width);
	void Fill(const std::vector<Eigen::Vector2f>& Poly, int width, int height);
	void Fill(const std::vector<Mesh>& drawPoly, int width, int height);

private:
	GLuint _vao;
	GLuint texId; // id en opengl
	std::vector<GLuint> vecTexture;

};

#endif