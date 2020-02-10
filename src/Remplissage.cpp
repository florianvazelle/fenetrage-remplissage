#include <iostream>
#include "Remplissage.h"

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
