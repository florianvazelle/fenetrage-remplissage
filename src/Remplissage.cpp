#include <iostream>
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

void Remplissage::initRemplissage(uint32_t program, int width, int height) {
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

	glUniform1i(glGetUniformLocation(program, "u_TextureSampler"), 0); // Tu indique a quelle variable du fragment shader est associé la texture du canal 0

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

void Remplissage::displayRemplissage(int width, int height) {
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

class Node
{
public:
	int ymax;
	float xmin, invertCoef;
	Node() : ymax(-1), xmin(0.0), invertCoef(0.0) { };
};

class SI {
public:
	// Contruit la structure
	void buildSI(const std::vector<Eigen::Vector2f>&);
	// Cherche le point suivant
	int yNext(int, const std::vector<Eigen::Vector2f>&);
	// Enregistre les cotés
	void recordOfSide(const Eigen::Vector2f&, const Eigen::Vector2f&, int);

	std::vector<std::list<Node>> _list;
};


void insertInTheList(std::list<Node>& orderedList, const Node& item) {
	std::list<Node>::iterator currentNode = orderedList.begin();
	while ((currentNode != orderedList.end()) && ((*currentNode).xmin < item.xmin)) {
		currentNode++;
	}
	orderedList.insert(currentNode, item);
}

int SI::yNext(int k, const std::vector<Eigen::Vector2f>& p) {
	int j; 

	if ((k + 1) > (p.size() - 1))
		j = 0;
	else
		j = k + 1;
	while (p[k][1] == p[j][1])
		if ((j + 1) > (p.size() - 1))
			j = 0;
		else
			j++;
	return (p[j][1]);
}

void SI::recordOfSide(const Eigen::Vector2f& minPoint, const Eigen::Vector2f& maxPoint, int yComp) {
	Node n;
	n.invertCoef = (float)(maxPoint[0] - minPoint[0]) / (maxPoint[1] - minPoint[1]);
	n.xmin = minPoint[0];

	if (maxPoint[1] < yComp)
		n.ymax = maxPoint[1] - 1;
	else
		n.ymax = maxPoint[1];
	insertInTheList(_list[minPoint[1]], n);
}

void SI::buildSI(const std::vector<Eigen::Vector2f>& Poly) {
	Eigen::Vector2f v1, v2;
	int i, yPrev;

	yPrev = Poly[Poly.size() - 2][1];
	v1[0] = Poly[Poly.size() - 1][0];
	v1[1] = Poly[Poly.size() - 1][1]; 

	for (i = 0; i < Poly.size(); i++) {
		v2 = Poly[i];
		if (v1[1] != v2[1]) {
			if (v1[1] < v2[1])
				recordOfSide(v1, v2, yNext(i, Poly)); 
			else
				recordOfSide(v2, v1, yPrev); 
		}
		yPrev = v1[1];
		v1 = v2;
	}
}

void updateTexture(std::vector<GLuint>& vecTexture, const std::list<Node>& L, int width) {

	int x1, x2;

	bool found;
	for (int x = 0; x < width; x++) {
		found = false;
		std::list<Node>::const_iterator iter1 = L.begin();
		while (iter1 != L.end()) {
			std::list<Node>::const_iterator iter2 = iter1;
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
		vecTexture.push_back(found ? -1 : 0);
	}
}

void buildLCA(std::list<Node>& LCA, const std::list<Node>& Element) {
	std::list<Node>::const_iterator iter;
	iter = Element.begin();
	iter++;

	while (iter != Element.end()) {
		insertInTheList(LCA, *iter);
		iter++;
	}
}

void updateLCA(int y, std::list<Node>& L) {

	std::list<Node>::iterator iter = L.begin();
	while (iter != L.end()) {
		if (y >= (*iter).ymax) {
			L.erase(iter++);
		} else {
			(*iter).xmin += (*iter).invertCoef;
			iter++;
		}
	}
}

void sortLCA(std::list<Node>& L) {
	std::list<Node> L1;
	std::list<Node>::iterator iter = L.begin();
	while (iter != L.end())
	{
		insertInTheList(L1, *iter);
		L.erase(iter++);
	}
	L = L1;
}

void Remplissage::Fill(const std::vector<Eigen::Vector2f>& Poly, int width, int height) {

	std::vector<Eigen::Vector2f> P(Poly);
	for (auto& p : P) {
		world2local(p, width, height);
	}

	SI _SI;
	std::list<Node> LCA;

	std::list<Node> EmptyList;

	EmptyList.push_front(Node());
	for (int i = 0; i < height; i++) {
		_SI._list.push_back(EmptyList);
	}

	_SI.buildSI(P);

	vecTexture.clear();
	for (int yOfLine = 0; yOfLine < height; yOfLine++) {

		buildLCA(LCA, _SI._list[yOfLine]);
		if (!LCA.empty()) {

			updateTexture(vecTexture, LCA, width);
			updateLCA(yOfLine, LCA);
			sortLCA(LCA);
		} else {
			updateTexture(vecTexture, std::list<Node>(), width);
		}
	}
}