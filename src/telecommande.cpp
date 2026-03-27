#include <Arduino.h>

// extern void setMotors(int leftDir, int rightDir);

void cmd_avancer() {
  Serial.println("CMD: Avance");
  // setMotors(1, 1);
}

void cmd_reculer() {
  Serial.println("CMD: Recule");
  // setMotors(-1, -1);
}

void cmd_gauche() {
  Serial.println("CMD: Gauche");
  // setMotors(-1, 1);
}

void cmd_droite() {
  Serial.println("CMD: Droite");
  // setMotors(1, -1);
}

void cmd_stop() {
  Serial.println("CMD: STOP");
  // setMotors(0, 0);
}