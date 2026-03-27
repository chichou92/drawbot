#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Drawbot AR";
const char* password = "Projetinge2.2";

int sequence = 0;

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

// Prototypes
void handle_OnConnect();
void handle_NotFound();
void handle_seq1();
void handle_seq2();
void handle_seq3();
String createHTML();

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/", handle_OnConnect);
  server.on("/seq1", handle_seq1);
  server.on("/seq2", handle_seq2);
  server.on("/seq3", handle_seq3);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("Wifi accessible");
}

void loop() {
  server.handleClient();

  if (sequence == 1) {
    Serial.println("Joue séquence 1 (Escalier)");
    sequence = 0;
  }
  
  if (sequence == 2) {
    Serial.println("Joue séquence 2 (Cercle)");
    sequence = 0; 
  }
  
  if (sequence == 3) {
    Serial.println("Joue séquence 3 (Rose des vents)");
    sequence = 0;
  }
}

void handle_OnConnect() {
  server.send(200, "text/html", createHTML());
}

void handle_seq1() {
  sequence = 1;
  server.send(200, "text/html", createHTML());
}

void handle_seq2() {
  sequence = 2;
  server.send(200, "text/html", createHTML());
}

void handle_seq3() {
  sequence = 3;
  server.send(200, "text/html", createHTML());
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String createHTML() {
  String str = "<!DOCTYPE html><html>";
  
  str += "<head>";
  str += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  
  str += "<style>";
  str += "body {font-family: Arial; text-align: center; background-color: #f5f5f5;}";
  str += "h1 {color: #007A7B;}";
  str += "button {padding: 15px 30px; font-size: 18px; margin: 10px; border: none; border-radius: 10px; background-color: #007A7B; color: white;}";
  str += "button:hover {background-color: #005f60;}";
  str += "</style>";
  
  str += "</head>";

  str += "<body>";
  str += "<h1>Projet Drawbot</h1>";

  str += "<p>Sequence actuelle : " + String(sequence) + "</p>";

  str += "<a href=\"/seq1\"><button>Sequence 1</button></a><br>";
  str += "<a href=\"/seq2\"><button>Sequence 2</button></a><br>";
  str += "<a href=\"/seq3\"><button>Sequence 3</button></a><br>";

  str += "</body>";
  str += "</html>";

  return str;
}