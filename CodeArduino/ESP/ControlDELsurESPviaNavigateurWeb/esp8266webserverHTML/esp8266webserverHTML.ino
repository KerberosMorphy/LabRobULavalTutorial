/*
 * Gérer une DEL connecté à un ESP via un navigateur web sur le même réseau local
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Page html envoyé au nagivateur lorsqu'un utilisateur se connect
#include "index.h" 

// Led déjà intégré au ESP (modele NodeMCU)
#define LED 12  

// SSID et Mot de Passe de votre Wifi
const char* ssid = "NomSSID";
const char* password = "MotDePasse";

ESP8266WebServer server(80); //Server on port 80

//===============================================================
//     Cette routine est exécuté quand vous vous connectez
//     Au IP de cette AP via votre navigateur
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
 
void handleADC() {
 int a = analogRead(A0);
 String adcValue = String(a);
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}
 
void handleLED() {
 String ledState = "OFF";
 String t_state = server.arg("LEDstate"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
 Serial.println(t_state);
 if(t_state == "1")
 {
  digitalWrite(LED,HIGH); //LED ON
  ledState = "ON"; //Feedback parameter
 }
 else
 {
  digitalWrite(LED,LOW); //LED OFF
  ledState = "OFF"; //Feedback parameter  
 }
 
 server.send(200, "text/plane", ledState); //Send web page
}
//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  //Onboard LED port Direction output
  pinMode(LED,OUTPUT); 
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/setLED", handleLED);
  server.on("/readADC", handleADC);
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  server.handleClient();          //Handle client requests
}
