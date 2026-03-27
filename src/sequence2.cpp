#include <Arduino.h>

// extern void setMotors(int leftDir, int rightDir);
// extern void driveDistance(float cm);

void seq2_classique(int rayon) {
  Serial.println("=== LANCEMENT SÉQUENCE 2 (CLASSIQUE) ===");
  Serial.print("Action : Traçage d'un cercle de rayon ");
  Serial.print(rayon);
  Serial.println(" cm");
  
  delay(3000); // À remplacer par votre calcul de cinématique (vitesse des deux roues)
  
  Serial.println("=== FIN SÉQUENCE 2 ===");
}

void seq2_avancee(int nbr_petales) {
  Serial.println("=== LANCEMENT SÉQUENCE 2 (AVANCÉE) ===");
  Serial.print("Action : Traçage d'une rosace à ");
  Serial.print(nbr_petales);
  Serial.println(" pétales");
  
  delay(3000); // À remplacer par votre boucle de rosace
  
  Serial.println("=== FIN SÉQUENCE 2 ===");
}