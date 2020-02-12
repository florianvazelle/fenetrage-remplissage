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
				segment.a;// = world2local(Poly[i][0], Poly[i][1], width, height);
				segment.b;// = world2local(Poly[(i + 1) % lengthPoly][0], Poly[(i + 1) % lengthPoly][1], width, height);

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
	if (vecTexture.size() > 0) {
		std::cout << "displayRemplissage" << std::endl;

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

		std::cout << "Fin displayRemplissage" << std::endl;
	}

}

void destroyRemplissage() {
	glDeleteTextures(1, &texId);
}

/*void drawLine(const std::list<Node>& L, int width) 
{
	bool found;
	for (int x = 0; x < width; x++) {
		found = false;
		// Tant qu'il reste des noeuds
		for (const Node& item : L) {
			if (item.xmin == x) {
				found = true;
				break;
			}
		}
		vecTexture.push_back(found ? -1 : 0);
	}

}*/


///////Structures 
class Node
{
public:
	int ymax;
	float xmin, invertCoef;
	// Constructeur
	Node() : ymax(-1), xmin(0.0), invertCoef(0.0) { };
};

class SI
{
public:
	// Contruit la structure
	void buildSI(std::vector<Eigen::Vector2f>&);
	// Cherche le point suivant
	int yNext(int, std::vector<Eigen::Vector2f>);
	// Enregistre les cotés
	void recordOfSide(Eigen::Vector2f, Eigen::Vector2f, int);

	std::vector<std::list<Node>> SIList; // tableau de list de noeuds, soit la structure principale de SI
};


////////////Méthodes pour le SI
// Insertion avec classement de même ordre pour le SI et LCA
void insertInTheList(std::list<Node>& orderedList, const Node& item)
{
	// On prend le premier élément
	std::list<Node>::iterator currentNode = orderedList.begin();
	// Tant qu'on a un y plus petit
	while ((currentNode != orderedList.end()) && ((*currentNode).xmin < item.xmin)) {
		//On va chercher un noeud suivant
		currentNode++;
	}
	// On peut placer notre noeud en bout de chaine
	orderedList.insert(currentNode, item);
}

// Trouve le point suivant
int SI::yNext(int k, std::vector<Eigen::Vector2f> p)
{
	int j; // Point suivant

		   // Retourne au premier point
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

// Enregistre les données sur les cotés en créant les nodes
void SI::recordOfSide(Eigen::Vector2f minPoint, Eigen::Vector2f maxPoint, int yComp)
{
	Node n;
	n.invertCoef = (float)(maxPoint[0] - minPoint[0]) / (maxPoint[1] - minPoint[1]);
	n.xmin = minPoint[0];

	if (maxPoint[1] < yComp)
		n.ymax = maxPoint[1] - 1;
	else
		n.ymax = maxPoint[1];
	insertInTheList(SIList[minPoint[1]], n);
}

// Construction de la structure intermediaire
void SI::buildSI(std::vector<Eigen::Vector2f>& Poly) //const
{
	Eigen::Vector2f v1, v2;
	int i, yPrev;

	yPrev = Poly[Poly.size() - 2][1];
	v1[0] = Poly[Poly.size() - 1][0]; // x du dernier point
	v1[1] = Poly[Poly.size() - 1][1]; // y du dernier point

	// On compare chaque point chacun des autres. Lors de la première boucle, on compare le premier et le dernier.
	for (i = 0; i < Poly.size(); i++)
	{
		v2 = Poly[i];
		if (v1[1] != v2[1]) // Si on detecte une droite horizontale, on ne l'ajoute pas
		{
			if (v1[1] < v2[1])
				recordOfSide(v1, v2, yNext(i, Poly)); //up edge
			else
				recordOfSide(v2, v1, yPrev); // down edge
		}
		yPrev = v1[1];
		v1 = v2;
	}
}


//////////////LCA
void buildLCA(std::list<Node>& LCA, std::list<Node> Element)
{
	// On défini un iterator
	std::list<Node>::iterator iter;
	iter = Element.begin();
	iter++;

	// Tant qu'on est pas au bout de la sous list du SI
	while (iter != Element.end())
	{
		// On ajoute tous les éléments d'un y du SI dans le LCA
		insertInTheList(LCA, *iter);
		iter++;
	}
}

void drawLine(std::list<Node> L, int width)
{

	int x1, x2;

	bool found;
	for (int x = 0; x < width; x++) {
		found = false;
		std::list<Node>::iterator iter1 = L.begin();
		// Tant qu'il reste des noeuds
		while (iter1 != L.end())
		{
			std::list<Node>::iterator iter2 = iter1;
			iter2++;
			// On récupère les x
			x1 = (int)(*iter1).xmin; // xmin qui a evolué avec le coef
			x2 = (int)(*iter2).xmin;

			if (x1 < x2) {
				if (x1 <= x && x <= x2) {
					found = true;
					break;
				}
			}
			else {
				if (x1 >= x && x >= x2) {
					found = true;
					break;
				}
			}

			// On traite les points deux à deux donc on incrémente de deux points ajdacents
			iter1++;
			iter1++;
		}
		vecTexture.push_back(found ? -1 : 0);
	}
}

// Mise à jour du LCA
void outLCA(int y, std::list<Node>& L)
{
	std::list<Node>::iterator iter = L.begin();
	while (iter != L.end()) {
		// On retire les points pour lesquel ont a dépassé le y
		if (y >= (*iter).ymax)
			L.erase(iter++);
		else
		{
			// On incrémente le x (y min) avec le coef calculé (pour les entre deux points)
			(*iter).xmin += (*iter).invertCoef;
			iter++;
		}
	}
}

// Réordonne les noeuds
void resortLCA(std::list<Node>& L)
{
	Node n;
	std::list<Node> L1;
	std::list<Node>::iterator iter = L.begin();
	while (iter != L.end())
	{
		// Permet de retrier les noeuds
		insertInTheList(L1, *iter);
		L.erase(iter++);
	}
	L = L1;
}

//////////////////Fonction de filling
void Fill(std::vector<Eigen::Vector2f> Poly, int width, int height)
{
	std::vector<Eigen::Vector2f> P(Poly);
	for (auto& p : P) {
		world2local(p, width, height);
		std::cout << "--------" << p << std::endl;
	}

	// Définission des listes
	SI _SI;
	std::list<Node> LCA;

	// Pour la création d'un tableau (SIList) vide.
	Node EmptyNode;
	std::list<Node> EmptyList;

	EmptyList.push_front(EmptyNode); // initialisation à vide

	for (int i = 0; i < height; i++) // Trouver ici le y max
		_SI.SIList.push_back(EmptyList); // Autant de liste de noeud vide que de y

	// Construit la SI
	_SI.buildSI(P);

	vecTexture.clear();
	for (int yOfLine = 0; yOfLine < height; yOfLine++)
	{
		// Création d'un nouveau LCA en fonction du Y en cours
		buildLCA(LCA, _SI.SIList[yOfLine]);
		if (!LCA.empty())
		{
			// Dessin du remplissage
			drawLine(LCA, width);
			// Mise à jour du LCA
			outLCA(yOfLine, LCA);
			// On reordonne le LCA
			resortLCA(LCA);
		} else {
			drawLine(std::list<Node>(), width);
		}
	}
}