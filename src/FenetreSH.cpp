#include <iostream>

#include "FenetreSH.h"

// Methods test si intersection segment-droite
bool coupe(const Eigen::Vector2f& S, const Eigen::Vector2f& P, const Eigen::Vector2f& F, const Eigen::Vector2f& F1) {
    Eigen::Vector2f FS, FP, FF1;
    FF1[0] = F1[0] - F[0];
    FF1[1] = F1[1] - F[1];
    FP[0] = P[0] - F[0];
    FP[1] = P[1] - F[1];
    FS[0] = S[0] - F[0];
    FS[1] = S[1] - F[1];
    if ((FF1[0] * FP[1] - FF1[1] * FP[0]) * (FF1[0] * FS[1] - FF1[1] * FS[0]) < 0)
        return true;
    else
        return false;
}

// Produit scalaire
float dot(const Eigen::Vector2f &v1, const Eigen::Vector2f &v2) {
  return v1[0] * v2[0] + v1[1] * v2[1];
}

// Methods retourne le point d'intersection segment-droite
Eigen::Vector2f intersection(const Eigen::Vector2f& S, const Eigen::Vector2f& P, const Eigen::Vector2f& F, const Eigen::Vector2f& F1) {
	std::cout << S << std::endl;

    float x1 = S[0];
    float y1 = S[1];
    float x2 = P[0];
    float y2 = P[1];

    float x3 = F[0];
    float y3 = F[1];
    float x4 = F1[0];
    float y4 = F1[1];

    float a1 = (y4 - y3) / (x4 - x3);
    float b1 = (y3 - a1 * x3);

    float a2 = (y2 - y1) / (x2 - x1);
    float b2 = (y2 - a2 * x2);

    float x = (b2 - b1) / (a1 - a2);
    float y = (a1 * x + b1);

    return { x, y };
}

bool visible(const Eigen::Vector2f& S, const Eigen::Vector2f& F, const Eigen::Vector2f& F1) {
    // Vecteur directeur de F et F1
    Eigen::Vector2f dirFF1(F1[0] - F[0], F1[1] - F[1]);
    // Normal
    Eigen::Vector2f normal(dirFF1[1], -dirFF1[0]);
    normal.normalize();

    // Vecteur directeur de F et S
    Eigen::Vector2f dirFS(S[0] - F[0], S[1] - F[1]);

    // Produit scalaire
    float pscal = dirFS[0] * normal[0] + dirFS[1] * normal[1];
    return pscal > 0;
}

/* PL correspond a un polygone et PW a la fenetre

Dans le pseudo code Pj correspond au points de PL
et Fi correspond au point de PW

PS correspond a out (variable de sortie) polygone finale  */
void Decoupage(std::vector<Eigen::Vector2f> &out,
               const std::vector<Eigen::Vector2f> &entry_poly,
               const std::vector<Eigen::Vector2f> &window) {

	int n2;
	Eigen::Vector2f S, F, I; // points
	std::vector<Eigen::Vector2f> curren_poly = entry_poly;

	int n1 = entry_poly.size();
	int n3 = window.size();

	/* Pour chaque point de la window P W */
	for (int i = 1; i <= n3 - 1; i++) { // n3 -1 car le dernier point correspond aussi au premier
		n2 = 0;
		out.clear();

		/* Pour chaque point du polygone P L */
		for (int j = 0; j <= n1; j++) {
			if (j == 1) {
				F = curren_poly[j]; /* Sauver le 1 er = dernier sommet */
			} else {
				if (coupe(S, curren_poly[j], window[i], window[i + 1])) {
					I = intersection(S, curren_poly[j], window[i], window[i + 1]);
					out.push_back(I);
					n2++;
				}
			}
			S = curren_poly[j];
			if (visible(S, window[i], window[i + 1])) {
				out.push_back(S);
				n2++;
			}
		}
		if (n2 > 0) {
			/* Traitement du dernier côté de P L */
			if (coupe(S, F, window[i], window[i + 1])) {
				I = intersection(S, F, window[i], window[i + 1]);
				out.push_back(I);
				n2++;
			}
			/* Découpage pour chacun des sous polygones */
			curren_poly = out; // On remplace
			n1 = n2;
		}
	}
}

// Decoupe entre deux Mesh
void Decoupage(std::vector<Eigen::Vector2f>& out, const Mesh& PL, const Mesh& PW) {
  std::vector<Eigen::Vector2f> PLAllPoints = PL.getAllPoints();
  PLAllPoints.push_back(PLAllPoints[0]);

  std::vector<Eigen::Vector2f> PWAllPoints = PW.getAllPoints();
  PWAllPoints.push_back(PWAllPoints[0]);

  Decoupage(out, PLAllPoints, PWAllPoints);
}