#include <iostream>

#include "FenetreSH.h"

bool coupe(Eigen::Vector2f S, Eigen::Vector2f P, Eigen::Vector2f F, Eigen::Vector2f F1) {

}

Eigen::Vector2f intersection(Eigen::Vector2f S, Eigen::Vector2f P, Eigen::Vector2f F, Eigen::Vector2f F1) {

}

bool visible(Eigen::Vector2f S, Eigen::Vector2f F, Eigen::Vector2f F1) {

}

void charger(Eigen::Vector2f I, std::vector<Eigen::Vector2f> PS) {

}

/* Dans le pseudo code Pj correspond au points de PL
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
}
