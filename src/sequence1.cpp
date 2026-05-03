#include <Arduino.h>
#include "moteurs.h"

void seq1_classique() {
  Serial.println("=== TRAÇAGE ESCALIER (Méthode Relative) ===");
  
  // 1er trait de l'escalier
  avancer(20.0);
  delay(500); // Petite pause pour stabiliser le stylo
  
  // Angle droit (Le robot gère la courbe pour faire une pointe nette)
  virage_tractrice(90.0); 
  delay(500);
  
  // 2ème trait de l'escalier
  avancer(10.0);
  delay(500);
  
  // Deuxième angle droit (dans l'autre sens si besoin, modifie selon ton dessin)
  virage_tractrice(-90.0);
  delay(500);
  
  // Dernier trait
  avancer(40.0);
  
  Serial.println("=== ESCALIER TERMINÉ ! ===");
}

void seq1_avancee(int nbr_carres) {
  Serial.println("=== LANCEMENT SÉQUENCE 1 (AVANCÉE) ===");
  Serial.print("Action : Traçage de ");
  Serial.print(nbr_carres);
  Serial.println(" carrés en utilisant la Tractrice...");
  
  // Boucle pour dessiner le nombre de carrés demandé
  for (int i = 0; i < nbr_carres; i++) {
    Serial.print("Traçage du carré n°");
    Serial.println(i + 1);
    
    // Un carré = 4 côtés et 4 angles droits
    for (int cote = 0; cote < 4; cote++) {
      avancer(15.0); // Avance de 15 cm
      delay(500);    // Stabilise le stylo
      
      virage_tractrice(90.0); // L'angle droit parfait
      delay(500);
    }
    
    // Si on veut faire un effet visuel (carrés décalés ou circonscrits)
    // On peut ajouter un petit déplacement avant le prochain carré.
    // Exemple : on tourne un peu pour faire une rosace de carrés
    if (i < nbr_carres - 1) {
      virage_tractrice(15.0); // Décalage de 15 degrés
      delay(500);
    }
  }
  
  Serial.println("=== FIN SÉQUENCE 1 AVANCÉE ===");
}