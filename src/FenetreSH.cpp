#include <iostream>

#include "FenetreSH.h"

// Methods test si intersection segment-droite
bool coupe(Eigen::Vector2f S, Eigen::Vector2f P, Eigen::Vector2f F, Eigen::Vector2f F1) {
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
// Methods retourne le point d'intersection segment-droite
Eigen::Vector2f intersection(Eigen::Vector2f S, Eigen::Vector2f P, Eigen::Vector2f F, Eigen::Vector2f F1) {
    float x1 = S[0];
    float y1 = S[1];
    float x2 = P[0];
    float y2 = P[1];

    float x3 = F[0];
    float y3 = F[1];
    float x4 = F1[0];
    float y4 = F1[1];

    float numA = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
    float numB = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);
    float deNom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);

    if (deNom == 0) {
        std::cout << "intersection mauvais algo" << std::endl;
    }

    float uA = numA / deNom;
    float uB = numB / deNom;

    Eigen::Vector2f out;

    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
        out[0] = x1 + (uA * (x2 - x1));
        out[1] = y1 + (uA * (y2 - y1));
    } else {
        std::cout << "intersection mauvais algo" << std::endl;
    }

    return out;
}

bool visible(Eigen::Vector2f S, Eigen::Vector2f F, Eigen::Vector2f F1) {
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

void charger(Eigen::Vector2f I, std::vector<Eigen::Vector2f> PS) {
    PS.push_back(I);
}

/* PL correspond a un polygone et PW a la fenetre

Dans le pseudo code Pj correspond au points de PL
et Fi correspond au point de PW */
std::vector<Eigen::Vector2f> Decoupage(std::vector<Eigen::Vector2f> PL, std::vector<Eigen::Vector2f> PW) {

  int n2;
  Eigen::Vector2f S, F, I; // points
  std::vector<Eigen::Vector2f> PS; // polygone finale

  int n1 = PL.size();
  int n3 = PW.size();

  /* Pour chaque point de la window P W */
  for (int i = 1; i <= n3 - 1; i++) { // n3 -1 car le dernier point correspond aussi au premier
    n2 = 0;
    PS.clear();

    /* Pour chaque point du polygone P L */
    for (int j = 0; j <= n1; j++) {
      if (j == 1) {
        F = PL[j]; /* Sauver le 1 er = dernier sommet */
      } else {
        if (coupe(S, PL[j], PW[i], PW[i + 1])) {
          I = intersection(S, PL[j], PW[i], PW[i + 1]);
          charger(I, PS);
          n2++;
        }
      }
      S = PL[j];
      if (visible(S, PW[i], PW[i + 1])) {
        charger(S, PS);
        n2++;
      }
    }
    if (n2 > 0) {
      /* Traitement du dernier côté de P L */
      if (coupe(S, F, PW[i], PW[i + 1])) {
        I = intersection(S, F, PW[i], PW[i + 1]);
        charger(I, PS);
        n2++;
      }
      /* Découpage pour chacun des sous polygones */
      PL = PS; // On remplace
      n1 = n2;
    }
  }

  return PS;
}

std::vector<Eigen::Vector2f> Decoupage(Mesh PL, Mesh PW) {
    std::vector<Eigen::Vector2f> PLAllPoints = PL.getAllPoints();
    PLAllPoints.push_back(PLAllPoints[0]);

    std::vector<Eigen::Vector2f> PWAllPoints = PW.getAllPoints();
    PWAllPoints.push_back(PWAllPoints[0]);
    return Decoupage(PLAllPoints, PWAllPoints);
}