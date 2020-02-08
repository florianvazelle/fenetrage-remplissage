#include <iostream>

#include "FenetreSH.h"

// Methods test si intersection segment-droite
bool coupe(const Eigen::Vector2f &S, const Eigen::Vector2f &P, const Eigen::Vector2f &F, const Eigen::Vector2f &F1)
{
	Eigen::Vector2f FF1 = {F1[0] - F[0], F1[1] - F[1]};
	Eigen::Vector2f FS = {P[0] - F[0], P[1] - F[1]};
	Eigen::Vector2f FP = {S[0] - F[0], S[1] - F[1]};

	return ((FF1[0] * FP[1] - FF1[1] * FP[0]) * (FF1[0] * FS[1] - FF1[1] * FS[0]) < 0);
}

void display(const std::string &s, const Eigen::Vector2f &S)
{
	std::cout << s << "(" << S[0] << ", " << S[1] << ")" << std::endl;
}

// Methods retourne le point d'intersection droite-droite
// Mais pas important car juste avant il aura tester l'équation segment droite
Eigen::Vector2f intersection(const Eigen::Vector2f &S, const Eigen::Vector2f &P, const Eigen::Vector2f &F, const Eigen::Vector2f &F1)
{
	typedef Eigen::Hyperplane<float, 2> Line2f;

	Line2f SP = Line2f::Through(S, P);
	Line2f FF1 = Line2f::Through(F, F1);

	return SP.intersection(FF1);
}

bool visible(const Eigen::Vector2f &S, const Eigen::Vector2f &F, const Eigen::Vector2f &F1)
{
	// Vecteur directeur de F et F1
	Eigen::Vector2f dirFF1 = F1 - F;

	// Normal
	Eigen::Vector2f normal(dirFF1[1], -dirFF1[0]);
	normal.normalized();

	// Vecteur directeur de F et S
	Eigen::Vector2f dirFS = S - F;

	// Produit scalaire
	float pscal = dirFS.dot(normal);
	return pscal > 0;
}

/* PL correspond a un polygone et PW a la fenetre

Dans le pseudo code Pj correspond au points de PL
et Fi correspond au point de PW

PS correspond a out (variable de sortie) polygone finale  */
void Decoupage(std::vector<Eigen::Vector2f> &out,
			   const std::vector<Eigen::Vector2f> &PL,
			   const std::vector<Eigen::Vector2f> &PW)
{

	std::vector<Eigen::Vector2f> window(PW);
	if (PW.front() != PW.back())
	{
		window.push_back(PW.front());
	}

	std::vector<Eigen::Vector2f> poly(PL);
	if (PL.front() != PL.back())
	{
		poly.push_back(PL.front());
	}

	Eigen::Vector2f S; // points

	/* Pour chaque point de la window PW */
	for (int i = 0; i < window.size() - 1; i++) // n3 - 1 car le dernier point correspond aussi au premier
	{
		out.clear();

		/* Pour chaque point du polygone PL */
		for (std::vector<Eigen::Vector2f>::iterator it = poly.begin() ; it != poly.end(); ++it)
		{
			// Si c'est le premier
			if (it != poly.begin())
			{
				// Sinon, on regarde si il y a intersection
				if (coupe(S, *it, window[i], window[i + 1]))
				{
					// Si oui, on recupère le points et on l'ajoute
					out.push_back(intersection(S, *it, window[i], window[i + 1]));
				}
			}
			S = *it;
			if (visible(S, window[i], window[i + 1]))
			{
				out.push_back(S);
			}
		}
		if (out.size() > 0)
		{
			/* Traitement du dernier côté de P L */
			if (coupe(S, poly[0], window[i], window[i + 1]))
			{
				out.push_back(intersection(S, poly[0], window[i], window[i + 1]));
			}
			/* Découpage pour chacun des sous polygones */
			poly = out; // On remplace
		}
	}
}

// Decoupe entre deux Mesh
void Decoupage(std::vector<Eigen::Vector2f> &out, const Mesh &PL, const Mesh &PW)
{
	std::vector<Eigen::Vector2f> PLAllPoints = PL.getAllPoints();
	PLAllPoints.push_back(PLAllPoints[0]);

	std::vector<Eigen::Vector2f> PWAllPoints = PW.getAllPoints();
	PWAllPoints.push_back(PWAllPoints[0]);

	Decoupage(out, PLAllPoints, PWAllPoints);
}