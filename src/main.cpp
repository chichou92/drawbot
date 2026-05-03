#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "moteurs.h"
#include <Wire.h> // Nécessaire pour lire le capteur ici aussi

// --- DÉCLARATION DES FONCTIONS EXTERNES ---
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

// --- VARIABLES GLOBALES ---
volatile long ticsgauche = 0;
volatile long ticsdroit = 0;

int actionEnAttente = 0; 
int paramVariable = 0; 
bool robotEnMouvement = false; // Le Verrou de sécurité

// --- PARAMÈTRES WI-FI ---
const char* ssid = "Drawbot AR";
const char* password = "Projetinge2.2";
IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

// --- FONCTION ENVOIE DE DONNÉES AU SERVEUR ---
struct Validation {
  float dist_th, dist_mes;
  float ang_th, ang_mes;
  float rayon_th, rayon_mes;
  float erreur_fermeture;
  float nord_th, nord_mes;
} dernierTest;

void handle_GetData() {
  String json = "{";
  json += "\"s1_d_th\":" + String(dernierTest.dist_th) + ",\"s1_d_mes\":" + String(dernierTest.dist_mes) + ",";
  json += "\"s1_a_th\":" + String(dernierTest.ang_th) + ",\"s1_a_mes\":" + String(dernierTest.ang_mes) + ",";
  json += "\"s2_r_th\":" + String(dernierTest.rayon_th) + ",\"s2_r_mes\":" + String(dernierTest.rayon_mes) + ",";
  json += "\"s2_err\":" + String(dernierTest.erreur_fermeture) + ",";
  json += "\"s3_n_th\":" + String(dernierTest.nord_th) + ",\"s3_n_mes\":" + String(dernierTest.nord_mes);
  json += "}";
  server.send(200, "application/json", json);
}

// ==========================================
// INTERFACE HTML
// ==========================================
String pageEntete(String titre) {
  String str = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><meta charset=\"UTF-8\">";
  str += "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>";
  str += "<style>";
  str += "body {font-family: Arial, sans-serif; text-align: center; background-color: white; color: black; margin: 0; padding: 20px;}";
  str += "h1, h2 {color: #007A7B;}";
  str += "button {padding: 15px; font-size: 16px; margin: 10px; border: 2px solid #007A7B; border-radius: 8px; background-color: #007A7B; color: white; width: 90%; max-width: 300px; cursor: pointer; font-weight: bold; transition: 0.3s;}";
  str += "button:hover {background-color: white; color: #007A7B;}";
  str += ".btn-retour, .btn-stop {background-color: black; border-color: black;}";
  str += ".btn-retour:hover, .btn-stop:hover {background-color: white; color: black;}";
  str += ".btn-cmd {background-color: white; color: #007A7B;}";
  str += ".btn-cmd:hover {background-color: #007A7B; color: white;}";
  str += "input[type=number] {padding: 10px; font-size: 16px; width: 120px; text-align: center; margin-bottom: 10px; border-radius: 5px; border: 2px solid #007A7B; color: black; outline: none;}";
  str += ".box {background-color: white; border: 2px solid black; padding: 15px; border-radius: 10px; margin-bottom: 20px;}";
  str += "canvas { background: #f9f9f9; border: 1px solid #ddd; margin-top: 20px; max-width: 100%; }";
  str += "</style></head><body><h1>" + titre + "</h1>";
  return str;
}

String pagePied() { return "</body></html>"; }

void redirigerVers(String url) {
  server.sendHeader("Location", url, true);
  server.send(302, "text/plain", ""); 
}

// ==========================================
// GESTIONNAIRES DE PAGES (HANDLERS)
// ==========================================
void handle_Accueil() {
  String html = pageEntete("Drawbot ECE");
  html += "<h2>Sélectionnez un mode :</h2>";
  html += "<a href=\"/page_seq1\"><button>Séquence 1 (Lignes/Carrés)</button></a><br>";
  html += "<a href=\"/page_seq2\"><button>Séquence 2 (Cercles/Rosaces)</button></a><br>";
  html += "<a href=\"/page_seq3\"><button>Séquence 3 (Boussole)</button></a><br><hr>";
  html += "<a href=\"/page_cmd\"><button class=\"btn-cmd\">Télécommande Manuelle</button></a><br>";
  html += pagePied();
  server.send(200, "text/html", html);
}

void handle_PageSeq1() {
  String html = pageEntete("Séquence 1 - Carrés");
  html += "<div class=\"box\"><h2>Mode Classique (Escalier)</h2><form action=\"/run_seq1_classique\"><button type=\"submit\">Lancer</button></form></div>";
  html += "<div class=\"box\"><h2>Mode Avancé</h2><form action=\"/run_seq1_avancee\">";
  html += "<input type=\"number\" name=\"carres\" value=\"3\"><br><button type=\"submit\">Tracer</button></form></div>";
  html += "<a href=\"/\"><button class=\"btn-retour\">⬅️ Retour</button></a>";
  server.send(200, "text/html", html + pagePied());
}

void handle_PageSeq2() {
  String html = pageEntete("Séquence 2 - Cercles");
  html += "<div class=\"box\"><h2>Mode Classique (Cercle simple)</h2><form action=\"/run_seq2_classique\">";
  html += "<label>Rayon (cm) :</label><br><input type=\"number\" name=\"rayon\" value=\"10\" min=\"5\" max=\"50\"><br><button type=\"submit\">Tracer le cercle</button></form></div>";
  html += "<div class=\"box\"><h2>Mode Avancé (Rosace)</h2><form action=\"/run_seq2_avancee\">";
  html += "<label>Nombre de pétales :</label><br><input type=\"number\" name=\"petales\" value=\"4\" min=\"4\" max=\"12\"><br><button type=\"submit\">Tracer la Rosace</button></form></div>";
  html += "<a href=\"/\"><button class=\"btn-retour\">⬅️ Retour</button></a>";
  server.send(200, "text/html", html + pagePied());
}

void handle_PageSeq3() {
  String html = pageEntete("Séquence 3 - Boussole");
  html += "<div class=\"box\"><h2>Mode Classique (Ligne vers le Nord)</h2><form action=\"/run_seq3_classique\"><button type=\"submit\">Chercher le Nord</button></form></div>";
  html += "<div class=\"box\"><h2>Mode Avancé (Rose des Vents)</h2><form action=\"/run_seq3_avancee\"><button type=\"submit\">Tracer l'étoile 8 branches</button></form></div>";
  html += "<a href=\"/\"><button class=\"btn-retour\">⬅️ Retour</button></a>";
  server.send(200, "text/html", html + pagePied());
}

void handle_PageGraph() {
  String html = pageEntete("Performances Drawbot");
  html += "<div class=\"box\">";
  html += "<h2>Suivi des Encodeurs</h2>";
  html += "<canvas id=\"myChart\" width=\"400\" height=\"200\"></canvas>";
  html += "</div>";

  html += "<script>";
  html += "var ctx = document.getElementById('myChart').getContext('2d');";
  html += "var chart = new Chart(ctx, {";
  html += "  type: 'line',";
  html += "  data: { labels: [], datasets: [";
  html += "    { label: 'Tics Gauche', borderColor: 'red', data: [], fill: false },";
  html += "    { label: 'Tics Droit', borderColor: 'blue', data: [], fill: false }";
  html += "  ]},";
  html += "  options: { scales: { y: { beginAtZero: true } } }";
  html += "});";

  html += "setInterval(function() {";
  html += "  fetch('/data').then(response => response.json()).then(data => {";
  html += "    if(chart.data.labels.length > 20) { chart.data.labels.shift(); chart.data.datasets[0].data.shift(); chart.data.datasets[1].data.shift(); }";
  html += "    chart.data.labels.push(new Date().toLocaleTimeString());";
  html += "    chart.data.datasets[0].data.push(data.ticsG);";
  html += "    chart.data.datasets[1].data.push(data.ticsD);";
  html += "    chart.update();";
  html += "  });";
  html += "}, 500);";
  html += "</script>";

  html += "<a href=\"/\"><button class=\"btn-retour\">⬅️ Retour</button></a>";
  server.send(200, "text/html", html + pagePied());
}

void handle_PageCmd() {
  String html = pageEntete("Télécommande");
  html += "<div class=\"box\">";
  html += "<a href=\"/cmd?dir=avancer\"><button>⬆️</button></a><br>";
  html += "<a href=\"/cmd?dir=gauche\"><button style=\"width:40%;\">⬅️</button></a>";
  html += "<a href=\"/cmd?dir=droite\"><button style=\"width:40%;\">➡️</button></a><br>";
  html += "<a href=\"/cmd?dir=reculer\"><button>⬇️</button></a><br>";
  html += "<a href=\"/cmd?dir=stop\"><button class=\"btn-stop\">STOP</button></a></div>";
  html += "<a href=\"/\"><button class=\"btn-retour\">⬅️ Retour</button></a>";
  server.send(200, "text/html", html + pagePied());
}

// ==========================================
// LOGIQUE DE COMMANDE (AVEC VERROU)
// ==========================================
void handle_RunSeq1Classique() { if(!robotEnMouvement) actionEnAttente = 11; redirigerVers("/page_seq1"); }
void handle_RunSeq1Avancee()   { if(!robotEnMouvement && server.hasArg("carres")) { paramVariable = server.arg("carres").toInt(); actionEnAttente = 12; } redirigerVers("/page_seq1"); }
void handle_RunSeq2Classique() { if(!robotEnMouvement && server.hasArg("rayon")) { paramVariable = server.arg("rayon").toInt(); actionEnAttente = 21; } redirigerVers("/page_seq2"); }
void handle_RunSeq2Avancee()   { if(!robotEnMouvement && server.hasArg("petales")) { paramVariable = server.arg("petales").toInt(); actionEnAttente = 22; } redirigerVers("/page_seq2"); }
void handle_RunSeq3Classique() { if(!robotEnMouvement) actionEnAttente = 31; redirigerVers("/page_seq3"); }
void handle_RunSeq3Avancee()   { if(!robotEnMouvement) actionEnAttente = 32; redirigerVers("/page_seq3"); }

void handle_Command() {
  if (robotEnMouvement) {
    server.send(200, "text/html", "<h2>Robot occupe...</h2><script>setTimeout(function(){window.history.back();}, 1500);</script>");
    return;
  }
  if (server.hasArg("dir")) {
    String d = server.arg("dir");
    if(d == "avancer") actionEnAttente = 91;
    else if(d == "reculer") actionEnAttente = 92;
    else if(d == "gauche")  actionEnAttente = 93;
    else if(d == "droite")  actionEnAttente = 94;
    else if(d == "stop")    actionEnAttente = 95;
    redirigerVers("/page_cmd"); 
  }
}

void init_IMU() {
  Wire.begin(21, 22); // SDA = 21, SCL = 22 d'après ton PDF
  
  Wire.beginTransmission(0x6B); // Adresse de l'IMU d'après ton PDF
  Wire.write(0x11); // Registre CTRL2_G (Configuration du Gyroscope)
  Wire.write(0x40); // On le règle à 104 Hz et 250 dps (degrés par seconde)
  byte erreur = Wire.endTransmission();
  
  if (erreur == 0) {
    Serial.println("IMU LSM6DS3 initialisé avec succès !");
  } else {
    Serial.println("ERREUR : IMU introuvable. Vérifie tes soudures !");
  }
}

// --- INTERRUPTIONS ENCODEURS ---
void IRAM_ATTR isr_gauche() { ticsgauche++; }
void IRAM_ATTR isr_droit() { ticsdroit++; }

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);

  init_IMU();

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  
  server.on("/", handle_Accueil);
  server.on("/page_seq1", handle_PageSeq1);
  server.on("/page_seq2", handle_PageSeq2);
  server.on("/page_seq3", handle_PageSeq3);
  server.on("/page_graph", handle_PageGraph);
  server.on("/page_cmd", handle_PageCmd);
  server.on("/run_seq1_classique", handle_RunSeq1Classique);
  server.on("/run_seq1_avancee", handle_RunSeq1Avancee);
  server.on("/run_seq2_classique", handle_RunSeq2Classique);
  server.on("/run_seq2_avancee", handle_RunSeq2Avancee);
  server.on("/run_seq3_classique", handle_RunSeq3Classique);
  server.on("/run_seq3_avancee", handle_RunSeq3Avancee);
  server.on("/data", handle_GetData);
  server.on("/cmd", handle_Command);
  server.begin();

  // PWM Configuration
  for(int i=0; i<4; i++) ledcSetup(i, 5000, 8);
  ledcAttachPin(17, 0); ledcAttachPin(16, 1); // Gauche
  ledcAttachPin(19, 2); ledcAttachPin(18, 3); // Droit
  
  pinMode(4, OUTPUT); pinMode(23, OUTPUT);
  digitalWrite(4, HIGH); digitalWrite(23, HIGH);

  pinMode(32, INPUT_PULLUP); pinMode(27, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(32), isr_gauche, RISING);
  attachInterrupt(digitalPinToInterrupt(27), isr_droit, RISING);

  Serial.println("Drawbot pret sur 192.168.4.1");
}

// ==========================================
// LOOP PRINCIPALE
// ==========================================
void loop() {
  server.handleClient();

  if (actionEnAttente != 0 && !robotEnMouvement) {
    robotEnMouvement = true; // Verrouillage
    Serial.println("Execution de l'action...");

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

    actionEnAttente = 0; 
    robotEnMouvement = false; // Déverrouillage
    Serial.println("Robot pret pour l'ordre suivant.");
  }
}
