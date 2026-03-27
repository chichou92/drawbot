#include <Arduino.h>

// Décommentez ces lignes quand vous aurez créé votre fichier moteurs.cpp !
// extern void driveDistance(float cm);
// extern void turnAngle(float degrees);

void seq1_classique() {
  Serial.println("=== LANCEMENT SÉQUENCE 1 (CLASSIQUE) ===");
  Serial.println("Action : Traçage de l'escalier...");
  
  delay(3000); // À remplacer par votre suite de driveDistance et turnAngle
  
  Serial.println("=== FIN SÉQUENCE 1 ===");
}

void seq1_avancee(int nbr_carres) {
  Serial.println("=== LANCEMENT SÉQUENCE 1 (AVANCÉE) ===");
  Serial.print("Action : Traçage de ");
  Serial.print(nbr_carres);
  Serial.println(" carrés circonscrits...");
  
  delay(3000); // À remplacer par votre algorithme géométrique
  
  Serial.println("=== FIN SÉQUENCE 1 ===");
}