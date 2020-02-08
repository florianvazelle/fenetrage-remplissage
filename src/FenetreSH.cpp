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

	std::vector<Eigen::Vector2f> curren_poly(PL);
	if (PL.front() != PL.back())
	{
		curren_poly.push_back(PL.front());
	}

	int n2;
	Eigen::Vector2f S, front; // points

	int n1 = curren_poly.size();

	/* Pour chaque point de la window PW */
	for (int i = 0; i < window.size() - 1; i++) // n3 - 1 car le dernier point correspond aussi au premier
	{
		n2 = 0;
		out.clear();

		/* Pour chaque point du polygone PL */
		for (int j = 0; j < curren_poly.size(); j++)
		{
			const Eigen::Vector2f current_point = curren_poly[j];

			// Si c'est le premier
			if (j == 0)
			{
				// On le sauve
				front = current_point;
			}
			else
			{
				// Sinon, on regarde si il y a intersection
				if (coupe(S, current_point, window[i], window[i + 1]))
				{
					// Si oui, on recupère le points et on l'ajoute
					out.push_back(intersection(S, current_point, window[i], window[i + 1]));
					n2++;
				}
			}
			S = current_point;
			if (visible(S, window[i], window[i + 1]))
			{
				out.push_back(S);
				n2++;
			}
		}
		if (n2 > 0)
		{
			/* Traitement du dernier côté de P L */
			if (coupe(S, front, window[i], window[i + 1]))
			{
				out.push_back(intersection(S, front, window[i], window[i + 1]));
				n2++;
			}
			/* Découpage pour chacun des sous polygones */
			curren_poly = out; // On remplace
			assert(out.size() == n2);
			n1 = n2;
			assert(curren_poly.size() == n1);
		}
	}

	out = curren_poly;
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