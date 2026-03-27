#include <Arduino.h>

extern void driveDistance(float cm);
extern void turnAngle(float degrees);

void executeSequence2() {
  Serial.println("--- LANCEMENT SEQUENCE 2 (Cercle) ---");
  
  // Simulation de 3 secondes
  delay(3000); 

  Serial.println("--- FIN SEQUENCE 2 ---");
}