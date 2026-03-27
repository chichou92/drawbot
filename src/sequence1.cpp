#include <Arduino.h>

// On prévient ce fichier que les fonctions moteurs existent dans main.cpp
extern void driveDistance(float cm);
extern void turnAngle(float degrees);

void executeSequence1() {
  Serial.println("--- LANCEMENT SEQUENCE 1 (Escalier) ---");
  
  // Simulation de 3 secondes
  delay(3000); 

  Serial.println("--- FIN SEQUENCE 1 ---");
}