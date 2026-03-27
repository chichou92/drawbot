#include <Arduino.h>

extern void driveDistance(float cm);
extern void turnAngle(float degrees);

void executeSequence3() {
  Serial.println("--- LANCEMENT SEQUENCE 3 (Rose des vents) ---");
  
  // Simulation de 3 secondes
  delay(3000); 

  Serial.println("--- FIN SEQUENCE 3 ---");
}