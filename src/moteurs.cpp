#include "moteurs.h"
#include <Wire.h> // Nécessaire pour l'IMU

extern volatile long ticsgauche;
extern volatile long ticsdroit;

// A VERIFIER AVEC TES ROUES : (700 tics pour 1 tour de roue de 9cm de diamètre)
const float TICS_PAR_CM = 700.0 / (PI * 9.0);

float propor = 5.0; 
float integr = 0.1; 
float deriv  = 2.0; 

#define IN_1_G 17
#define IN_2_G 16
#define IN_1_D 19
#define IN_2_D 18

void setMoteurGauche(int pwm) {
  if (pwm > 0) {
    ledcWrite(0, pwm);
    ledcWrite(1, 0);
  } else if (pwm < 0) {
    ledcWrite(0, 0);
    ledcWrite(1, -pwm);
  } else {
    ledcWrite(0, 0);
    ledcWrite(1, 0);
  }
}

void setMoteurDroit(int pwm) {
  if (pwm > 0) {
    ledcWrite(2, 0);
    ledcWrite(3, pwm);
  } else if (pwm < 0) {
    ledcWrite(2, -pwm);
    ledcWrite(3, 0);
  } else {
    ledcWrite(2, 0);
    ledcWrite(3, 0);
  }
}

// Fonction utilitaire pour lire le Gyroscope Z en degrés/seconde
float lire_gyro_z() {
  Wire.beginTransmission(0x6B); 
  Wire.write(0x26); 
  Wire.endTransmission(false);
  Wire.requestFrom((uint16_t)0x6B, (uint8_t)2, true); 
  if (Wire.available() == 2) {
    int16_t z_brut = Wire.read() | (Wire.read() << 8);
    return (float)z_brut * 8.75 / 1000.0; 
  }
  return 0.0;
}

// --- LA FONCTION FINALE : VIRAGE TRACTRICE PURE ---
void virage_tractrice(float angle_cible_deg) {
  float theta = 0.0; 
  

  // On ment sur 'd' (16 au lieu de 13) pour forcer le robot 
  // à reculer plus fort et casser l'arc de cercle du stylo !
  float d = 18.0; 
  float E = 8.80; // Ton écartement de roues
  float Vs = 12.0; // Vitesse de manœuvre (assez rapide pour vaincre les engrenages)
  
  // Paramètres de contrôle Moteur (Feedforward + PID)
  float K_pwm_base = 12.0; 
  float Kp_vit = 5.0; 
  float Ki_vit = 2.0;  
  
  float somme_err_D = 0, somme_err_G = 0;
  long last_tics_G = ticsgauche;
  long last_tics_D = ticsdroit;

  unsigned long dernier_temps = millis();
  int sens = (angle_cible_deg > 0) ? 1 : -1;

  Serial.println("Début du virage tractrice !");

  while (abs(theta) < abs(angle_cible_deg)) {
    unsigned long temps_actuel = millis();
    float dt = (temps_actuel - dernier_temps) / 1000.0; 
    if (dt <= 0.001) { yield(); continue; } 
    dernier_temps = temps_actuel;

    // 1. LECTURE DU CAP REEL
    float coeff_gyro = 2.0; // Ajuste si le robot fait plus ou moins que 90° physiquement
    float vitesse_angulaire_reelle = lire_gyro_z() * coeff_gyro; 
    theta += vitesse_angulaire_reelle * dt; 

    // 2. CONSIGNES TRACTRICE
    float alpha = abs(theta) * PI / 180.0; 
    float V_robot = Vs * sin(alpha);
    float omega_robot = (Vs / d) * cos(alpha) * sens;

    float V_D_cible = V_robot + (E / 2.0) * omega_robot;
    float V_G_cible = V_robot - (E / 2.0) * omega_robot;

    // 3. BOUCLE INTERNE (VITESSE ROUES)
    long delta_tics_G = ticsgauche - last_tics_G;
    long delta_tics_D = ticsdroit - last_tics_D;
    last_tics_G = ticsgauche;
    last_tics_D = ticsdroit;

    float vitesse_G_reelle = (delta_tics_G / TICS_PAR_CM) / dt;
    float vitesse_D_reelle = (delta_tics_D / TICS_PAR_CM) / dt;

    // LE CORRECTIF MAGIQUE (sens de rotation des encodeurs)
    if (V_G_cible < 0) vitesse_G_reelle = -vitesse_G_reelle;
    if (V_D_cible < 0) vitesse_D_reelle = -vitesse_D_reelle;

    float err_G = V_G_cible - vitesse_G_reelle;
    float err_D = V_D_cible - vitesse_D_reelle;

    somme_err_G = constrain(somme_err_G + (err_G * dt), -100, 100);
    somme_err_D = constrain(somme_err_D + (err_D * dt), -100, 100);

    // FEEDFORWARD (Pour effacer la zone morte et le bruit sans attendre)
    int pwm_seuil = 70; // Monte à 70 ou 80 si le robot siffle au démarrage du virage

    float FF_D = 0;
    if (V_D_cible > 0.1) FF_D = pwm_seuil + (V_D_cible * K_pwm_base);
    else if (V_D_cible < -0.1) FF_D = -pwm_seuil + (V_D_cible * K_pwm_base);

    float FF_G = 0;
    if (V_G_cible > 0.1) FF_G = pwm_seuil + (V_G_cible * K_pwm_base);
    else if (V_G_cible < -0.1) FF_G = -pwm_seuil + (V_G_cible * K_pwm_base);

    int pwm_D = FF_D + (Kp_vit * err_D) + (Ki_vit * somme_err_D);
    int pwm_G = FF_G + (Kp_vit * err_G) + (Ki_vit * somme_err_G);

    pwm_D = constrain(pwm_D, -255, 255);
    pwm_G = constrain(pwm_G, -255, 255);

    setMoteurDroit(pwm_D);
    setMoteurGauche(pwm_G);

    delay(50); 
    yield();
  }

  setMoteurDroit(0);
  setMoteurGauche(0);
  Serial.println("Fin du virage !");
}

// --- FONCTION AVANCER CLASSIQUE ---
void avancer(float distance_demande) {
  long cible_tics = abs(distance_demande * TICS_PAR_CM); 
  ticsgauche = 0;
  ticsdroit = 0;
  
  int sens = (distance_demande > 0) ? 1 : -1;
  int puissance_base = 150 * sens; 

  long somme_erreurs = 0; 
  long erreur_precedente = 0; 

  while (abs(ticsgauche) < cible_tics || abs(ticsdroit) < cible_tics) {
    long erreur = ticsgauche - ticsdroit;
    
    long P = erreur * propor;
    somme_erreurs = constrain(somme_erreurs + erreur, -1000, 1000);
    long I = somme_erreurs * integr;
    long D = (erreur - erreur_precedente) * deriv;
    erreur_precedente = erreur;

    int correction = P + I + D;
    
    int puissance_gauche = puissance_base - (correction * sens);
    int puissance_droit  = puissance_base + (correction * sens);

    puissance_gauche = constrain(puissance_gauche, -255, 255);
    puissance_droit  = constrain(puissance_droit, -255, 255);

    setMoteurGauche(puissance_gauche);
    setMoteurDroit(puissance_droit);

    delay(1); 
    yield();
  }
  
  setMoteurGauche(0);
  setMoteurDroit(0);
}