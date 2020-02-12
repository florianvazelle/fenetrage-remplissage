#include <iostream>
#include "Remplissage.h"

GLuint VAO;
GLuint VBO;
GLuint texId; // id en opengl
//GLuint _pId; // shader
std::vector<Eigen::Vector2f> tabPointToFill;


struct Cote {
	float ymax;
	float xcurrent;
	float invcoef;
};

struct Maillon {
	float ymax;
	float xcurrent;
	float invcoef;
};

struct Segment {
	Eigen::Vector2f a;
	Eigen::Vector2f b;
};

Eigen::Vector2f world2local(float x, float y, int width, int height) {
	return {
		round(((x + 1.0f) / 2.0f) * width - 0.5f),
		round(((-1.0f * (y - 1.0f)) / 2) * height - 0.5f)
	};
}

void CreatSI(std::vector<Maillon>& SI, const std::vector<Eigen::Vector2f>& Poly) {

}

void InitLCA(std::vector<Cote>& LCA) {

}

void RemplissageLCA(const std::vector<Eigen::Vector2f>& Poly, const std::vector<Eigen::Vector2f>& CR)
{
	std::vector<Maillon> SI;
	CreatSI(SI, Poly);

	  
	std::vector<Cote> LCA;
	InitLCA(LCA);
}

void RemplissageNaif(const std::vector<Eigen::Vector2f>& Poly, int width, int height)
{
	std::vector<Segment> tabSegment;
	std::vector<Segment> tabSegmentToLocal;

	std::cout << "Remplissage" << std::endl;

	// remplir le tableau de segment
	std::cout << "Taille du Polygone: " << Poly.size() << std::endl;
	for (int i = 0; i < Poly.size(); i++)
	{
		Segment segment;
		segment.a = Poly[i];
		if (i < Poly.size() - 1){
			segment.b = Poly[i + 1];
		} else {
			segment.b = Poly[0];
		}
		std::cout << "x: [" << Poly[i][0] << "] y: [" << Poly[i][1] << "]" << std::endl;
		tabSegment.push_back(segment);
	}

	// remplir le tableau de segment en transformant en coordonnées locales
	std::cout << "Taille du tableau de segment: " << tabSegment.size() << std::endl;
	for (int i = 0; i < tabSegment.size(); i++)
	{
		Segment segment;

		std::cout << "A -> x: [" << tabSegment[i].a[0] << "] y: [" << tabSegment[i].a[1] << "]";
		std::cout << " B  -> x: [" << tabSegment[i].b[0] << "] y: [" << tabSegment[i].b[1] << "]" << std::endl;

		segment.a = world2local(tabSegment[i].a[0], tabSegment[i].a[1], width, height);
		segment.b = world2local(tabSegment[i].b[0], tabSegment[i].b[1], width, height);
		tabSegmentToLocal.push_back(segment);
	}

	std::cout << "Conversion en coordonnees locales ..." << std::endl;
	// Afficher le tableau des segments en coordonnées locales
	for (int i = 0; i < tabSegmentToLocal.size(); i++)
	{
		std::cout << "A -> x: [" << tabSegmentToLocal[i].a[0] << "] y: [" << tabSegmentToLocal[i].a[1] << "]";
		std::cout << " B  -> x: [" << tabSegmentToLocal[i].b[0] << "] y: [" << tabSegmentToLocal[i].b[1] << "]" << std::endl;
	}

	// Déterminer la liste des points à remplir dans "tabPointToFill".
	// On lance la demi-droite vers les x positifs et y = 0 comme ca ------------->
	std::cout << "Points a remplir ..." << std::endl;
	//std::cout << width << " " << height << std::endl;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int i = 0; i < tabSegmentToLocal.size(); i++)
			{
				int xa = (int)tabSegmentToLocal[i].a[0];
				int xb = (int)tabSegmentToLocal[i].a[1];
				int ya = (int)tabSegmentToLocal[i].b[0];
				int yb = (int)tabSegmentToLocal[i].b[1];

				if ((xa >= x) || (xb >= x))
				{
					if (((ya >= y) && (yb <= y)) || ((yb >= y) && (ya <= y)))
					{
						Eigen::Vector2f vector;
						vector[0] = x;
						vector[1] = y;
						tabPointToFill.push_back(vector);
						//std::cout << "[" << x << "][" << y << "] ";
					}
				}
				//std::cout << std::endl;
			}
		}
	}
	std::cout << "finish calculating" << std::endl;

	//Afficher les points à colorier
	std::cout << "size tab des points : " << tabPointToFill.size() << std::endl;
	/*
	for (int i = 0; i < tabPointToFill.size(); i++)
	{
		std::cout << "[" << tabPointToFill[i][0] << "][" << tabPointToFill[i][1] << "] " << std::endl;
	}
	*/
	std::cout << "finish display" << std::endl;
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

	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tabPointToFill[0]);
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