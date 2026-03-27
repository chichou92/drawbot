#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ==========================================
// LIENS VERS VOS FICHIERS DE SÉQUENCE (À coder plus tard)
// ==========================================
extern void seq1_classique();
extern void seq1_avancee(int nbr_carres);
extern void seq2_classique(int rayon);
extern void seq2_avancee(int nbr_petales);
extern void seq3_classique();
extern void seq3_avancee();
extern void cmd_avancer();
extern void cmd_reculer();
extern void cmd_gauche();
extern void cmd_droite();
extern void cmd_stop();

// ==========================================
// PARAMÈTRES WI-FI & SERVEUR
// ==========================================
const char* ssid = "Drawbot AR";
const char* password = "Projetinge2.2";

IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

int actionEnAttente = 0; 
int paramVariable = 0;   

// ==========================================
// FONCTIONS HTML (Le design du site)
// ==========================================
String pageEntete(String titre) {
  String str = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><meta charset=\"UTF-8\">";
  str += "<style>";
  // Fond blanc, texte noir
  str += "body {font-family: Arial, sans-serif; text-align: center; background-color: white; color: black; margin: 0; padding: 20px;}";
  
  // Titres en couleur ECE
  str += "h1, h2 {color: #007A7B;}";
  
  // Boutons classiques : Fond ECE, texte blanc. Au survol : Fond blanc, texte ECE
  str += "button {padding: 15px; font-size: 16px; margin: 10px; border: 2px solid #007A7B; border-radius: 8px; background-color: #007A7B; color: white; width: 90%; max-width: 300px; cursor: pointer; font-weight: bold; transition: 0.3s;}";
  str += "button:hover {background-color: white; color: #007A7B;}";
  
  // Boutons secondaires (Retour, Stop) : Fond noir, texte blanc. Au survol : Fond blanc, texte noir
  str += ".btn-retour, .btn-stop {background-color: black; border-color: black;}";
  str += ".btn-retour:hover, .btn-stop:hover {background-color: white; color: black;}";
  
  // Bouton Télécommande (Inversé par défaut pour se démarquer)
  str += ".btn-cmd {background-color: white; color: #007A7B;}";
  str += ".btn-cmd:hover {background-color: #007A7B; color: white;}";
  
  // Champs de texte (Rayon, Pétales)
  str += "input[type=number] {padding: 10px; font-size: 16px; width: 120px; text-align: center; margin-bottom: 10px; border-radius: 5px; border: 2px solid #007A7B; color: black; outline: none;}";
  
  // Boîtes autour des séquences : Bordure noire, fond blanc
  str += ".box {background-color: white; border: 2px solid black; padding: 15px; border-radius: 10px; margin-bottom: 20px;}";
  
  // Lignes de séparation
  str += "hr {border: 1px solid #007A7B;}";
  
  str += "</style></head><body>";
  str += "<h1>" + titre + "</h1>";
  return str;
}

String pagePied() {
  return "</body></html>";
}

// ==========================================
// PAGES DU SITE
// ==========================================
void handle_Accueil() {
  String html = pageEntete("Drawbot ECE");
  html += "<h2>Sélectionnez un mode :</h2>";
  html += "<a href=\"/page_seq1\"><button>Séquence 1 (Lignes/Carrés)</button></a><br>";
  html += "<a href=\"/page_seq2\"><button>Séquence 2 (Cercles/Rosaces)</button></a><br>";
  html += "<a href=\"/page_seq3\"><button>Séquence 3 (Boussole)</button></a><br>";
  html += "<hr style=\"border: 1px solid #7F8C8D; margin: 30px 0;\">";
  html += "<a href=\"/page_cmd\"><button class=\"btn-cmd\">Télécommande Manuelle</button></a><br>";
  html += pagePied();
  server.send(200, "text/html", html);
}

void handle_PageSeq1() {
  String html = pageEntete("Séquence 1 - Carrés");
  html += "<div class=\"box\"><h2>Mode Classique (Escalier)</h2>";
  html += "<form action=\"/run_seq1_classique\" method=\"GET\">";
  html += "<button type=\"submit\">Lancer l'escalier</button></form></div>";

  html += "<div class=\"box\"><h2>Mode Avancé (Carrés circonscrits)</h2>";
  html += "<form action=\"/run_seq1_avancee\" method=\"GET\">";
  html += "<label>Nombre de carrés :</label><br>";
  html += "<input type=\"number\" name=\"carres\" value=\"3\" min=\"1\" max=\"10\"><br>";
  html += "<button type=\"submit\">Tracer les carrés</button></form></div>";

  html += "<a href=\"/\"><button class=\"btn-retour\">⬅️ Retour Accueil</button></a>";
  html += pagePied();
  server.send(200, "text/html", html);
}

void handle_PageSeq2() {
  String html = pageEntete("Séquence 2 - Cercles");
  html += "<div class=\"box\"><h2>Mode Classique (Cercle simple)</h2>";
  html += "<form action=\"/run_seq2_classique\" method=\"GET\">";
  html += "<label>Rayon (cm) :</label><br>";
  html += "<input type=\"number\" name=\"rayon\" value=\"10\" min=\"5\" max=\"50\"><br>";
  html += "<button type=\"submit\">Tracer le cercle</button></form></div>";

  html += "<div class=\"box\"><h2>Mode Avancé (Rosace)</h2>";
  html += "<form action=\"/run_seq2_avancee\" method=\"GET\">";
  html += "<label>Nombre de pétales :</label><br>";
  html += "<input type=\"number\" name=\"petales\" value=\"4\" min=\"4\" max=\"12\"><br>";
  html += "<button type=\"submit\">Tracer la Rosace</button></form></div>";

  html += "<a href=\"/\"><button class=\"btn-retour\">⬅️ Retour Accueil</button></a>";
  html += pagePied();
  server.send(200, "text/html", html);
}

void handle_PageSeq3() {
  String html = pageEntete("Séquence 3 - Boussole");
  html += "<div class=\"box\"><h2>Mode Classique (Ligne vers le Nord)</h2>";
  html += "<form action=\"/run_seq3_classique\" method=\"GET\">";
  html += "<button type=\"submit\">Chercher le Nord</button></form></div>";

  html += "<div class=\"box\"><h2>Mode Avancé (Rose des Vents)</h2>";
  html += "<form action=\"/run_seq3_avancee\" method=\"GET\">";
  html += "<button type=\"submit\">Tracer l'étoile 8 branches</button></form></div>";

  html += "<a href=\"/\"><button class=\"btn-retour\">⬅️ Retour Accueil</button></a>";
  html += pagePied();
  server.send(200, "text/html", html);
}

void handle_PageCmd() {
  String html = pageEntete("Télécommande");
  html += "<div class=\"box\">";
  html += "<a href=\"/cmd?dir=avancer\"><button>⬆️</button></a><br>";
  html += "<a href=\"/cmd?dir=gauche\"><button style=\"width:40%;\">⬅️</button></a>";
  html += "<a href=\"/cmd?dir=droite\"><button style=\"width:40%;\">➡️</button></a><br>";
  html += "<a href=\"/cmd?dir=reculer\"><button>⬇️</button></a><br>";
  html += "<a href=\"/cmd?dir=stop\"><button class=\"btn-stop\">STOP</button></a>";
  html += "</div>";
  html += "<a href=\"/\"><button class=\"btn-retour\">⬅️ Retour Accueil</button></a>";
  html += pagePied();
  server.send(200, "text/html", html);
}

// ==========================================
// ROUTAGE DES GÂCHETTES (Redirections transparentes)
// ==========================================
void redirigerVers(String url) {
  server.sendHeader("Location", url, true);
  server.send(302, "text/plain", ""); 
}

void handle_RunSeq1Classique() { actionEnAttente = 11; redirigerVers("/page_seq1"); }
void handle_RunSeq1Avancee()   { if(server.hasArg("carres")) paramVariable = server.arg("carres").toInt(); actionEnAttente = 12; redirigerVers("/page_seq1"); }
void handle_RunSeq2Classique() { if(server.hasArg("rayon")) paramVariable = server.arg("rayon").toInt(); actionEnAttente = 21; redirigerVers("/page_seq2"); }
void handle_RunSeq2Avancee()   { if(server.hasArg("petales")) paramVariable = server.arg("petales").toInt(); actionEnAttente = 22; redirigerVers("/page_seq2"); }
void handle_RunSeq3Classique() { actionEnAttente = 31; redirigerVers("/page_seq3"); }
void handle_RunSeq3Avancee()   { actionEnAttente = 32; redirigerVers("/page_seq3"); }

void handle_Command() {
  if (server.hasArg("dir")) {
    String d = server.arg("dir");
    if(d == "avancer") actionEnAttente = 91;
    if(d == "reculer") actionEnAttente = 92;
    if(d == "gauche")  actionEnAttente = 93;
    if(d == "droite")  actionEnAttente = 94;
    if(d == "stop")    actionEnAttente = 95;
  }
  redirigerVers("/page_cmd");
}

void handle_NotFound() {
  // Ignore les requêtes silencieuses des navigateurs pour les icônes
  if (server.uri() == "/favicon.ico") { server.send(404, "text/plain", ""); return; }
  server.send(404, "text/plain", "Page introuvable !");
}

// ==========================================
// SETUP & LOOP
// ==========================================
void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  // Pages
  server.on("/", handle_Accueil);
  server.on("/page_seq1", handle_PageSeq1);
  server.on("/page_seq2", handle_PageSeq2);
  server.on("/page_seq3", handle_PageSeq3);
  server.on("/page_cmd", handle_PageCmd);

  // Actions
  server.on("/run_seq1_classique", handle_RunSeq1Classique);
  server.on("/run_seq1_avancee", handle_RunSeq1Avancee);
  server.on("/run_seq2_classique", handle_RunSeq2Classique);
  server.on("/run_seq2_avancee", handle_RunSeq2Avancee);
  server.on("/run_seq3_classique", handle_RunSeq3Classique);
  server.on("/run_seq3_avancee", handle_RunSeq3Avancee);
  server.on("/cmd", handle_Command);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("Serveur prêt. Connectez-vous à 192.168.4.1");
}

void loop() {
  server.handleClient();

  if (actionEnAttente != 0) {
    switch (actionEnAttente) {
      case 11: seq1_classique(); break;
      case 12: seq1_avancee(paramVariable); break;
      case 21: seq2_classique(paramVariable); break;
      case 22: seq2_avancee(paramVariable); break;
      case 31: seq3_classique(); break;
      case 32: seq3_avancee(); break;
      case 91: cmd_avancer(); break;
      case 92: cmd_reculer(); break;
      case 93: cmd_gauche(); break;
      case 94: cmd_droite(); break;
      case 95: cmd_stop(); break;
    }
    actionEnAttente = 0; // Réarmement
  }
}