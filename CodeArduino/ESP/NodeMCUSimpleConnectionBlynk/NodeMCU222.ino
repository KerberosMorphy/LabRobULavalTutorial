/*************************************************************
  Lien pour la librairie Blynk:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk est une plateform et une application mobile pour
  controler un Arduino, Raspberry et autre appareil
  connecté sur internet.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

 *************************************************************
  Exemple fonctionnant sur l'ESP de NodeMCU.

  Note: Néssecite le packge ESP8266:
    https://github.com/esp8266/Arduino

  Dans l'IDE du Arduino, bien choisir le bon module
  il y a 2 version du NodeMCU, il s'agit normalement de la 
  plus récente
  configuer dans: Tools -> Board

  Pour des configuration avancer voir les exemple suivant
  disponible sur Blynk :
   - ESP8266_Standalone_Manual_IP.ino
   - ESP8266_Standalone_SmartConfig.ino
   - ESP8266_Standalone_SSL.ino

  Modifier vore WiFi ssid, pass, et VOTRE token 
  d'authentification Blynk
 *************************************************************/

/* Mettre en commentaire pour désactiver l'affichage console
et sauver de l'espace */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>



int msg[1];


// Vous devrez vous créez votre token sur votre application Blynk Mobile.
// Pour ce faire aller dans les paramètre de votre projet.
char auth[] = "VotreTokenDAuthetification";

// YVos information d'Accès WiFi.
// Si réseau n'a pas de mot de passe inscrivez ""
char ssid[] = "VotreSSID";
char pass[] = "VotreMotDePasse";

void setup()
{
  // Console de débogage
  Serial.begin(115200);


  Blynk.begin(auth, ssid, pass);
  //Vous pouvez aussi identifier le serveur:
  //Vous pouvez remplacer l'adresse par votre propre serveur back-end sur le web
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Vous pouvez utiliser votre propre serveur back-end sur votre réseau local
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  delay(1000);

}

void loop()
{
  Blynk.run();

}
