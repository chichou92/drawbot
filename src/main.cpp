#include <Arduino.h>

// ==========================================
// PINS DU GYROBOT (D'après la documentation ECE)
// ==========================================
const int EN_D = 23; 
const int EN_G = 4;  
const int IN_1_D = 19;
const int IN_2_D = 18;
const int IN_1_G = 17;
const int IN_2_G = 16;
const int ENC_G_CH_A = 32; // On utilise le canal A pour compter la distance
const int ENC_D_CH_A = 27; 

// ==========================================
// PARAMÈTRES PHYSIQUES (À CALIBRER IMPÉRATIVEMENT)
// ==========================================
const float WHEEL_DIAMETER_CM = 9.0;  // Diamètre des roues bleues (90mm)
const float WHEEL_BASE_CM = 11.5;     // Écartement entre les deux roues (À mesurer précisément !)
const float PEN_OFFSET_CM = 6.0;      // Distance entre l'axe des roues et la pointe du stylo (À mesurer !)
const float ENCODER_PPR = 700.0;      // Impulsions par tour (À ajuster si les distances sont fausses)

// Calcul du nombre d'impulsions (ticks) pour faire 1 cm
const float TICKS_PER_CM = ENCODER_PPR / (PI * WHEEL_DIAMETER_CM);

// ==========================================
// VARIABLES GLOBALES
// ==========================================
volatile long leftTicks = 0;
volatile long rightTicks = 0;

// ==========================================
// INTERRUPTIONS (Pour lire les encodeurs)
// ==========================================
void IRAM_ATTR countLeft() { leftTicks++; }
void IRAM_ATTR countRight() { rightTicks++; }

// ==========================================
// FONCTION DE CONTRÔLE DES MOTEURS
// ==========================================
void setMotors(int leftDir, int rightDir) {
  // leftDir / rightDir : 1 (avant), -1 (arrière), 0 (stop)
  
  // Moteur Gauche
  if (leftDir == 1)      { digitalWrite(IN_1_G, HIGH); digitalWrite(IN_2_G, LOW); }
  else if (leftDir == -1){ digitalWrite(IN_1_G, LOW); digitalWrite(IN_2_G, HIGH); }
  else                   { digitalWrite(IN_1_G, LOW); digitalWrite(IN_2_G, LOW); }

  // Moteur Droit (Inversé mécaniquement pour l'effet miroir)
  if (rightDir == 1)      { digitalWrite(IN_1_D, LOW); digitalWrite(IN_2_D, HIGH); }
  else if (rightDir == -1){ digitalWrite(IN_1_D, HIGH); digitalWrite(IN_2_D, LOW); }
  else                    { digitalWrite(IN_1_D, LOW); digitalWrite(IN_2_D, LOW); }
}

// ==========================================
// FONCTIONS DE DÉPLACEMENT
// ==========================================
void driveDistance(float cm) {
  long target = abs(cm * TICKS_PER_CM);
  leftTicks = 0; 
  rightTicks = 0;
  
  int dir = (cm > 0) ? 1 : -1; // Avance si positif, recule si négatif

  // On arrête chaque roue indépendamment quand elle atteint sa cible pour rouler bien droit
  while (leftTicks < target || rightTicks < target) {
    int l = (leftTicks < target) ? dir : 0;
    int r = (rightTicks < target) ? dir : 0;
    setMotors(l, r);
  }
  setMotors(0, 0);
  delay(300); // Petite pause pour stabiliser le robot
}

void turnAngle(float degrees) {
  // degrees > 0 : Tourne à gauche | degrees < 0 : Tourne à droite
  float distancePerWheel = (abs(degrees) / 360.0) * (PI * WHEEL_BASE_CM);
  long target = distancePerWheel * TICKS_PER_CM;
  
  leftTicks = 0; 
  rightTicks = 0;

  int lDir = (degrees > 0) ? -1 : 1;
  int rDir = (degrees > 0) ? 1 : -1;

  while (leftTicks < target || rightTicks < target) {
    int l = (leftTicks < target) ? lDir : 0;
    int r = (rightTicks < target) ? rDir : 0;
    setMotors(l, r);
  }
  setMotors(0, 0);
  delay(300);
}

// L'astuce pour avoir un angle droit parfait malgré le décalage du stylo
void sharpTurn(float angle) {
  driveDistance(PEN_OFFSET_CM);  // 1. On avance l'axe des roues sur le coin
  turnAngle(angle);              // 2. On pivote (le stylo trace un grand arc à l'extérieur)
  driveDistance(-PEN_OFFSET_CM); // 3. On recule pour replacer exactement le stylo sur le coin
}

// Fonction pour marquer les points de départ et d'arrivée (Requis ET1.1)
void drawMarker() {
  turnAngle(90);
  driveDistance(1.5);
  driveDistance(-3.0);
  driveDistance(1.5);
  turnAngle(-90);
}

// ==========================================
// SETUP & LOOP
// ==========================================
void setup() {
  Serial.begin(115200);

  // Initialisation des pins
  pinMode(EN_D, OUTPUT); pinMode(EN_G, OUTPUT);
  pinMode(IN_1_D, OUTPUT); pinMode(IN_2_D, OUTPUT);
  pinMode(IN_1_G, OUTPUT); pinMode(IN_2_G, OUTPUT);
  pinMode(ENC_G_CH_A, INPUT_PULLUP);
  pinMode(ENC_D_CH_A, INPUT_PULLUP);

  // Activation des interruptions pour les encodeurs
  attachInterrupt(digitalPinToInterrupt(ENC_G_CH_A), countLeft, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_D_CH_A), countRight, RISING);

  // Activation de la puissance des moteurs
  digitalWrite(EN_D, HIGH);
  digitalWrite(EN_G, HIGH);

  Serial.println("Robot prêt. Lancement dans 3 secondes...");
  delay(3000); // Vous laisse le temps de poser le robot par terre

  // --- EXÉCUTION DE LA SÉQUENCE 1 ---
  
  drawMarker();           // Dessine une petite croix pour marquer le départ

  driveDistance(20.0);    // Ligne droite de 20 cm
  sharpTurn(90.0);        // Virage parfait de 90° à gauche

  driveDistance(10.0);    // Ligne droite de 10 cm
  sharpTurn(-90.0);       // Virage parfait de 90° à droite

  driveDistance(40.0);    // Ligne droite de 40 cm

  drawMarker();           // Dessine une petite croix pour marquer l'arrivée
  
  Serial.println("Séquence 1 terminée.");
}

void loop() {
  // Rien à faire ici, le robot s'arrête après la séquence.
}