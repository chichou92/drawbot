#ifndef MOTEURS_H
#define MOTEURS_H

#include <Arduino.h>

// Déclaration des fonctions pour qu'elles soient visibles partout
void avancer(float distance_demande);
void virage_tractrice(float angle_cible_deg);
void setMoteurGauche(int pwm);
void setMoteurDroit(int pwm);
void seq1_classique();
void seq1_avancee(int nbr_carres);
#endif