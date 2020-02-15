#include <iostream>

#include "LCA.h"
#include "Node.h"
#include "SI.h"
#include "CustomList.h"
#include "Remplissage.h"
#include "FenetreSH.h"

void world2local(Eigen::Vector2f& p, int width, int height) {

	float x2 = p[0] + 1.0f;
	x2 /= 2.0f;
	x2 *= width;
	x2 -= 0.5f;

	float y2 = (-p[1] + 1.0f);
	y2 /= 2.0f;
	y2 *= height;
	y2 -= 0.5f;

	p = { round(x2), round(y2) };
}

void Remplissage::initRemplissage(uint32_t program) {
	const float triangles[] = {
		-1.0f, -1.0f, /*uv*/0.f, 0.f,
		-1.0f, +1.0f, /*uv*/0.f, 1.f,
		+1.0f, +1.0f, /*uv*/1.f, 1.f,
		// second triangle
		-1.0f, -1.0f, /*uv*/0.f, 0.f,
		+1.0f, +1.0f, /*uv*/1.f, 1.f,
		+1.0f, -1.0f, /*uv*/1.f, 0.f,
	};

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

	glUniform1i(glGetUniformLocation(program, "u_TextureSampler"), 0); // Tu indique a quelle variable du fragment shader est associ� la texture du canal 0

	int position_location = glGetAttribLocation(program, "a_position");
	glEnableVertexAttribArray(position_location);
	glVertexAttribPointer(position_location, 2, GL_FLOAT, false, sizeof(float) * 4, 0);

	int texcoords_location = glGetAttribLocation(program, "a_texcoords");
	glEnableVertexAttribArray(texcoords_location);
	glVertexAttribPointer(texcoords_location, 2, GL_FLOAT, false, sizeof(float) * 4, (const void *)(sizeof(float) * 2));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Texture
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tabPointToFill[0]);
}

void Remplissage::displayRemplissage(int width, int height) const {
	if (vecTexture.size() > 0) {
		assert(vecTexture.size() == width * height);

		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, vecTexture.data());
		// Donc pas besoin de delete l'image a chaque fois

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texId);

		glBindVertexArray(_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Remplissage::destroyRemplissage() {
	glDeleteTextures(1, &texId);
}

void updateTexture(int y, std::vector<GLuint>& vecTexture, const List<Node>& L, int width) {

	int x1, x2;

	bool found;
	for (int x = 0; x < width; x++) {
		found = false;
		List<Node>::const_iterator iter1 = L.begin();
		while (iter1 != L.end()) {
			List<Node>::const_iterator iter2 = iter1;
			iter2++;
			x1 = (int)(*iter1).xmin; 
			x2 = (int)(*iter2).xmin;

			if (x1 < x2) {
				if (x1 <= x && x <= x2) {
					found = true;
					break;
				}
			} else {
				if (x1 >= x && x >= x2) {
					found = true;
					break;
				}
			}

			iter1++;
			iter1++;
		}
		vecTexture[y * width + x] = (found ? -1 : (vecTexture[y * width + x] != 0) ? -1 : 0);
	}
}

void Remplissage::Fill(const std::vector<Eigen::Vector2f>& Poly, int width, int height) {

	std::vector<Eigen::Vector2f> P(Poly);
	for (auto& p : P) {
		world2local(p, width, height);
	}

	SI _si(height);
	LCA _lca;

	_si.build(P);

	// Pour chaque lignes de l'ecran
	for (int yOfLine = 0; yOfLine < height; yOfLine++) {

		_lca.build(_si[yOfLine]);
		if (!_lca.empty()) {

			updateTexture(yOfLine, vecTexture, _lca.getList(), width);
			_lca.update(yOfLine);
			_lca.sort();
		} else {
			// Si lca est vide, on update la ligne, dans la texture, a 0 
			// updateTexture(yOfLine, vecTexture, List<Node>(), width);
			for (int x = 0; x < width; x++) {
				vecTexture[yOfLine * width + x] = (vecTexture[yOfLine * width + x] != 0) ? -1 : 0;
			}
		}
	}
}

void Remplissage::Fill(const std::vector<Mesh>& drawPoly, int width, int height) {
	// On prepare la nouvelle texture
	vecTexture.clear();
	vecTexture.resize(width * height);

	for (const Mesh& poly : drawPoly) {
		Fill(poly.getAllPoints(), width, height);
	}
}