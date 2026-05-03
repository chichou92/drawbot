#include <Arduino.h>
#include "moteurs.h" 

void cmd_avancer() {
  Serial.println("CMD: Avancer 20 cm");
  avancer(20.0);
}

void cmd_reculer() {
  Serial.println("CMD: Reculer 20 cm");
  avancer(-20.0);
}

void cmd_gauche() {
  Serial.println("CMD: Virage Tractrice Gauche");
  virage_tractrice(-90.0);
}

void cmd_droite() {
  Serial.println("CMD: Virage Tractrice Droite");
  virage_tractrice(90.0);
}

void cmd_stop() {
  Serial.println("CMD: STOP");
  setMoteurGauche(0);
  setMoteurDroit(0);
}