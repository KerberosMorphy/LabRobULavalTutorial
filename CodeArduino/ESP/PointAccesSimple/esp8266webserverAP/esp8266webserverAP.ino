/*
 * Server d'Accès Wifi
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//SSID et Mot de Passe du point d'accès que vous créez
const char* ssid = "ESPWebServer";
const char* password = "12345678";

ESP8266WebServer server(80); //Server on port 80

//==============================================================
//     Cette routine est exécuté quand vous vous connectez
//     Au IP de cette AP via votre navigateur
//==============================================================
void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

//===============================================================
//                  SETUP
//===============================================================
void setup(void){
  Serial.begin(9600);
  Serial.println("");
  WiFi.mode(WIFI_AP);           //Configure l'ESP en mode AP
  WiFi.softAP(ssid, password);  //Cémare l'AP avec le SSID et le mot de passe défini

  IPAddress myIP = WiFi.softAPIP(); //Se défini un IP
  Serial.print("HotSpt IP:");
  Serial.println(myIP);
 
  server.on("/", handleRoot);      //Routine à exécuté à la racine de l'adresse

  // Cette exemple exécutera la fonction handlePage2 lorsque l'utilisateur essaiera d'atteindre IPdeAP/page2
  // Ceci n'est qu'un example, la fonction handlePage2 n'existe pas
  // server.on("/page2, handlePage2"); 

  server.begin();                  //Démare le serveur
  Serial.println("HTTP server started");
}
//===============================================================
//                     LOOP
//===============================================================
void loop(void){
  server.handleClient();          //Gère les clients
}
