#include <iostream>
#include "Remplissage.h"
#include "FenetreSH.h"

GLuint VAO;
GLuint VBO;
GLuint texId; // id en opengl
//GLuint _pId; // shader
std::vector<Eigen::Vector2f> tabPointToFill;
std::vector<GLuint> vecTexture;

struct Segment {
	Eigen::Vector2f a;
	Eigen::Vector2f b;
};

Eigen::Vector2f world2local(float x, float y, int width, int height) {

	float x2 = x + 1.0f;
	x2 /= 2.0f;
	x2 *= width;
	x2 -= 0.5f;

	float y2 = (-y + 1.0f);
	y2 /= 2.0f;
	y2 *= height;
	y2 -= 0.5f;

	return { round(x2), round(y2) };
}

void RemplissageNaif(const std::vector<Eigen::Vector2f>& Poly, int width, int height)
{
	std::vector<Segment> tabSegment;
	std::vector<Segment> tabSegmentToLocal;

	int lengthPoly = Poly.size();

	vecTexture.clear();
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int i = 0; i < lengthPoly; i++)
			{
				Segment segment;
				segment.a = world2local(Poly[i][0], Poly[i][1], width, height);
				segment.b = world2local(Poly[(i + 1) % lengthPoly][0], Poly[(i + 1) % lengthPoly][1], width, height);

				if (segment.a[1] == segment.b[1]) {
					vecTexture.push_back(0);
					continue;
				}
				if (std::min(segment.a[1], segment.b[1]) < y && y <= std::max(segment.a[1], segment.b[1]) && x <= std::max(segment.a[0], segment.b[0])) {

					if (segment.a[1] == segment.b[1]) {
						vecTexture.push_back(-1);
					} else if (coupe(segment.a, segment.b, { 5, y }, { 7, y })) {
						Eigen::Vector2f tmp = intersection(segment.a, segment.b, { 5, y }, { 7, y });
						if (tmp[0] >= x) {
							vecTexture.push_back(-1);
						} else {
							vecTexture.push_back(0);
						}
					}
				} else {
					vecTexture.push_back(0);
				}
			}
		}
	}

}

void initRemplissage(uint32_t program, int width, int height) {
	const float triangles[] = {
		-1.0f, -1.0f, /*uv*/0.f, 0.f,
		-1.0f, +1.0f, /*uv*/0.f, 1.f,
		+1.0f, +1.0f, /*uv*/1.f, 1.f,
		// second triangle
		-1.0f, -1.0f, /*uv*/0.f, 0.f,
		+1.0f, +1.0f, /*uv*/1.f, 1.f,
		+1.0f, -1.0f, /*uv*/1.f, 0.f,
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

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

void displayRemplissage(int width, int height) {
	if (tabPointToFill.size() == 0)
		return;

	assert(vecTexture.size() == width * height);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, vecTexture.data());
	// Donc pas besoin de delete l'image a chaque fois

	/* Pour draw */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void destroyRemplissage() {
	glDeleteTextures(1, &texId);
}