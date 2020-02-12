#ifndef REMPLISSAGE_H
#define REMPLISSAGE_H

#include <vector>
#include <nanogui/nanogui.h>
#include <math.h>
#include "GLShader.h"

class Remplissage {
public:
	void initRemplissage(uint32_t shader, int width, int height);
	void displayRemplissage(int width, int height);
	void destroyRemplissage();

	void Fill(const std::vector<Eigen::Vector2f>& Poly, int width, int height);

private:
	GLuint _vao;
	GLuint texId; // id en opengl
	std::vector<GLuint> vecTexture;

};

#endif