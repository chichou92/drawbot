#include <Arduino.h>

// extern void driveDistance(float cm);
// extern void turnAngle(float degrees);

void seq3_classique() {
  Serial.println("=== LANCEMENT SÉQUENCE 3 (CLASSIQUE) ===");
  Serial.println("Action : Recherche du Nord magnétique et ligne droite...");
  
  delay(3000); // À remplacer par la lecture I2C du LIS3MDL
  
  Serial.println("=== FIN SÉQUENCE 3 ===");
}

void seq3_avancee() {
  Serial.println("=== LANCEMENT SÉQUENCE 3 (AVANCÉE) ===");
  Serial.println("Action : Traçage de la Rose des Vents (8 branches)");
  
  delay(3000); // À remplacer par l'algorithme de dessin de l'étoile
  
  Serial.println("=== FIN SÉQUENCE 3 ===");
}