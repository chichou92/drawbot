#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ==========================================
// LIENS VERS VOS 3 FICHIERS DE SÉQUENCE
// ==========================================
extern void executeSequence1();
extern void executeSequence2();
extern void executeSequence3();

// ==========================================
// PARAMÈTRES WI-FI & SERVEUR
// ==========================================
const char* ssid = "Drawbot AR";
const char* password = "Projetinge2.2";

int sequence = 0;         // La "Gâchette"
int derniereSequence = 0; // La "Mémoire" pour l'affichage web

IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

// ==========================================
// PINS ET PARAMÈTRES DU GYROBOT
// ==========================================
const int EN_D = 23; 
const int EN_G = 4;  
const int IN_1_D = 19;
const int IN_2_D = 18;
const int IN_1_G = 17;
const int IN_2_G = 16;
const int ENC_G_CH_A = 32; 
const int ENC_D_CH_A = 27; 

const float WHEEL_DIAMETER_CM = 9.0;  
const float WHEEL_BASE_CM = 11.5;     
const float ENCODER_PPR = 700.0;      
const float TICKS_PER_CM = ENCODER_PPR / (PI * WHEEL_DIAMETER_CM);

volatile long leftTicks = 0;
volatile long rightTicks = 0;

// ==========================================
// INTERRUPTIONS & FONCTIONS MOTEURS
// ==========================================
void IRAM_ATTR countLeft() { leftTicks++; }
void IRAM_ATTR countRight() { rightTicks++; }

void setMotors(int leftDir, int rightDir) {
  if (leftDir == 1)      { digitalWrite(IN_1_G, HIGH); digitalWrite(IN_2_G, LOW); }
  else if (leftDir == -1){ digitalWrite(IN_1_G, LOW); digitalWrite(IN_2_G, HIGH); }
  else                   { digitalWrite(IN_1_G, LOW); digitalWrite(IN_2_G, LOW); }

  if (rightDir == 1)      { digitalWrite(IN_1_D, LOW); digitalWrite(IN_2_D, HIGH); }
  else if (rightDir == -1){ digitalWrite(IN_1_D, HIGH); digitalWrite(IN_2_D, LOW); }
  else                    { digitalWrite(IN_1_D, LOW); digitalWrite(IN_2_D, LOW); }
}

void driveDistance(float cm) {
  long target = abs(cm * TICKS_PER_CM);
  leftTicks = 0; rightTicks = 0;
  int dir = (cm > 0) ? 1 : -1;

  while (leftTicks < target || rightTicks < target) {
    int l = (leftTicks < target) ? dir : 0;
    int r = (rightTicks < target) ? dir : 0;
    setMotors(l, r);
  }
  setMotors(0, 0);
  delay(500); 
}

void turnAngle(float degrees) {
  float distancePerWheel = (abs(degrees) / 360.0) * (PI * WHEEL_BASE_CM);
  long target = distancePerWheel * TICKS_PER_CM;
  leftTicks = 0; rightTicks = 0;

  int lDir = (degrees > 0) ? -1 : 1;
  int rDir = (degrees > 0) ? 1 : -1;

  while (leftTicks < target || rightTicks < target) {
    int l = (leftTicks < target) ? lDir : 0;
    int r = (rightTicks < target) ? rDir : 0;
    setMotors(l, r);
  }
  setMotors(0, 0);
  delay(500);
}

// ==========================================
// SERVEUR WEB : GESTION DES REQUÊTES
// ==========================================
String createHTML();

void handle_OnConnect() { server.send(200, "text/html", createHTML()); }
void handle_NotFound()  { server.send(404, "text/plain", "Not found"); }

void handle_seq1() { sequence = 1; derniereSequence = 1; server.send(200, "text/html", createHTML()); }
void handle_seq2() { sequence = 2; derniereSequence = 2; server.send(200, "text/html", createHTML()); }
void handle_seq3() { sequence = 3; derniereSequence = 3; server.send(200, "text/html", createHTML()); }

// ==========================================
// SETUP & LOOP
// ==========================================
void setup() {
  Serial.begin(115200);

  // Moteurs et Encodeurs
  pinMode(EN_D, OUTPUT); pinMode(EN_G, OUTPUT);
  pinMode(IN_1_D, OUTPUT); pinMode(IN_2_D, OUTPUT);
  pinMode(IN_1_G, OUTPUT); pinMode(IN_2_G, OUTPUT);
  pinMode(ENC_G_CH_A, INPUT_PULLUP); pinMode(ENC_D_CH_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_G_CH_A), countLeft, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_D_CH_A), countRight, RISING);
  digitalWrite(EN_D, HIGH); digitalWrite(EN_G, HIGH);

  // Wi-Fi et Serveur
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/", handle_OnConnect);
  server.on("/seq1", handle_seq1);
  server.on("/seq2", handle_seq2);
  server.on("/seq3", handle_seq3);
  server.onNotFound(handle_NotFound);
  server.begin();
  
  Serial.println("Robot prêt et Serveur Wi-Fi démarré !");
}

void loop() {
  server.handleClient(); // Écoute le téléphone

  if (sequence == 1) {
    executeSequence1();
    sequence = 0;          // Désarme la gâchette
    derniereSequence = 0;  // Remet l'affichage à zéro (il faudra rafraîchir la page web)
  }
  
  if (sequence == 2) {
    executeSequence2();
    sequence = 0; 
    derniereSequence = 0; 
  }
  
  if (sequence == 3) {
    executeSequence3();
    sequence = 0; 
    derniereSequence = 0; 
  }
}

// ==========================================
// CODE HTML DE LA PAGE WEB
// ==========================================
String createHTML() {
  String str = "<!DOCTYPE html><html>";
  str += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  str += "<meta charset=\"UTF-8\">"; // Pour les accents
  str += "<style>";
  str += "body {font-family: Arial; text-align: center; background-color: #f5f5f5;}";
  str += "h1 {color: #007A7B;}";
  str += "button {padding: 15px 30px; font-size: 18px; margin: 10px; border: none; border-radius: 10px; background-color: #007A7B; color: white; width: 80%; max-width: 300px;}";
  str += "button:hover {background-color: #005f60;}";
  str += "</style></head>";

  str += "<body>";
  str += "<h1>Drawbot ECE</h1>";
  
  if (derniereSequence == 0) {
    str += "<p>En attente d'un ordre...</p>";
  } else {
    str += "<p>Exécution de la séquence : <b>" + String(derniereSequence) + "</b> en cours...</p>";
    str += "<p><i>(Rafraîchissez la page à la fin du tracé)</i></p>";
  }

  str += "<a href=\"/seq1\"><button>Sequence 1 (Escalier)</button></a><br>";
  str += "<a href=\"/seq2\"><button>Sequence 2 (Cercle)</button></a><br>";
  str += "<a href=\"/seq3\"><button>Sequence 3 (Rose des vents)</button></a><br>";

  str += "</body></html>";
  return str;
}