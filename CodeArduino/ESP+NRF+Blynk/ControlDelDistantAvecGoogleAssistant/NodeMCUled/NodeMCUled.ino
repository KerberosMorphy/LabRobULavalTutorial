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

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Configuration des pin du NRF
#define CE_PIN   2
#define CSN_PIN 15

//Une pour émettre, l'autre pour recevoir
//IMPORTANT l'autre NRF devra avoir les configuration INVERSÉ
// CEST A DIRE que l'adresse slave et master seront interchangé
// on pourrait utiliser 3 adresse d'envoi et 3 adresse d'écoute pour contrôler
// 3 nrf distant
const byte slaveAddress[5] = {'R','x','A','A','A'}; // Correspond à l'adresse 0x5278414141LL en HEX
const byte masterAddress[5] = {'T','X','a','a','a'};// Correspond à l'adresse 0x5458616161LL en HEX

RF24 radio(CE_PIN, CSN_PIN);

int dataToSend[1];
int dataReceived[1]; // to hold the data from the slave - must match replyData[] in the slave

// La PIN de la switch connecté au NodeMCU
// Dans ce cas çi, notre switch amène +3.3V à la PIN 0
int SW1 = 0;
// Représente le status de la DEL contrôlé par un NRF distant
int ledStatus1 = LOW;

// Vous devrez vous créez votre token sur votre application Blynk Mobile.
// Pour ce faire aller dans les paramètre de votre projet.
char auth[] = "VotreTokenDAuthetification";

// YVos information d'Accès WiFi.
// Si réseau n'a pas de mot de passe inscrivez ""
char ssid[] = "VotreSSID";
char pass[] = "VotreMotDePasse";

/*========================================================*
 *            Configuration de départ
/*========================================================*/

void setup()
{
  // Debug console
  Serial.begin(115200);
  dataToSend[0] = LOW;

  Serial.println("MasterSwapRoles Starting");

  radio.begin();
  radio.setDataRate( RF24_250KBPS );

  //IMPORTANT: puisque les 2 module NRF auront l'Adresse slave et master interchangé
  //           on a pas a toucher à cette section mais comprenez qu'un NRF écrit dans
  //           dans un PIPE auquel l'autre NRF écoutera
  //On ouvre un PIPE pour l'écriture
  radio.openWritingPipe(slaveAddress);
  // On ouvre un PIPE pour la lecture
  radio.openReadingPipe(1, masterAddress);

  radio.setRetries(3,5); // délai, répétition d'envoie

  Blynk.begin(auth, ssid, pass);
  //Vous pouvez aussi identifier le serveur:
  //Vous pouvez remplacer l'adresse par votre propre serveur back-end sur le web
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Vous pouvez utiliser votre propre serveur back-end sur votre réseau local
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

/*========================================================*
 *                   Routine principale 
 * - On écoute le Cloud pour savoir s'il y a du changement
 * - On écoute le NRF distant pour savoir s'il y a du 
 *   changement
 * - On écoute notre propre Switch s'il a du changement
 *   et modifie le NRF distant
/*========================================================*/
void loop()
{
  getCloud();
  getData();
  switchLed();
}

/*========================================================*
 *            Envoie d'information au NRF distant
/*========================================================*/

void send(int ledStatus) {
  //on arrête l'écoute
  radio.stopListening(); 
    //bool de validation d'envoi
    bool rslt; 
    // on envoie la donnée, retournera True si l'Envoie est un succès
    rslt = radio.write( &dataToSend, sizeof(dataToSend) );

  //on recommence l'écoute
  radio.startListening();
  Serial.print("Donnée envoyé ");
  Serial.print(dataToSend[0]);
  if (rslt) {
    Serial.println("  et réception comfirmé");
    //l'Envoie étant réussis, le NRF distant changera l'état de sa DEL
    //nous pouvons donc enregistrer officiellement cette état dans notre
    //NodeMCU
    ledStatus1 = ledStatus;
    //On met à jour l'état réel de la DEL dans l'Application Blynk
    updateBlynk();
  }
  else {
    Serial.println("  mais la réception a échoué");
  }
}

/*========================================================*
 *                      Écoute Du NRF
/*========================================================*/

void getData()
{
  if (radio.available(&masterAddress)) //Si quelqu'un parle
  {
    radio.read(&dataReceived, sizeof(dataReceived)); //On lit les données
    switchSW(); //On modifie l'état de notre switch
  }
}

/*========================================================*
 *                      Écoute Du Cloud 
 * Pris en charge par la librairie de Blynk
/*========================================================*/

void getCloud()
{
    Blynk.run();
}

/*========================================================*
 *          Écoute De la Switch et modifie le NRF
 * Il s'agit principalement de modifier l'état du NRF 
 * distant si celui du NodeMCU à changé
/*========================================================*/

void switchLed(){
  // Si la Switch est HIGH et que le status de la DEL est sur LOW
  // ça signifie que la led et la switch ne sont pas synchronisé
  if (digitalRead(SW1) != ledStatus1 && ledStatus1 == LOW)
  {
    dataToSend[0] = HIGH; // On enregistre le nouveau status pour le partager
    send(HIGH); // On partage notre nouveau status
    delay(10);
  }
  else
  // Si la Switch est LOW et que le status de la DEL est sur HIGH
  // ça signifie que la led et la switch ne sont pas synchronisé
  if (digitalRead(SW1) != ledStatus1 && ledStatus1 == HIGH)
  {
    dataToSend[0] = LOW; // On enregistre le nouveau status pour le partager
    send(LOW); // On partage notre nouveau status
    delay(10);
  }
  //Pour le reste la switch et la LED sont synchornisé donc pas besoin de rien faire
}

/*========================================================*
 *          Écoute du NRF et modifie la Switch
 * Il s'agit principalement de modifier l'état du NodeMCU
 * si celui du NRF distant à changé
/*========================================================*/

void switchSW(){
  // Si la Switch est HIGH et que la DEL distant est LOW
  // ça signifie que la led et la switch ne sont pas synchronisé
  if (digitalRead(SW1) == HIGH && dataReceived[0] == LOW)
  {
    dataToSend[0] = dataReceived[0];
    digitalWrite(SW1, LOW);
    ledStatus1 = LOW;
    updateBlynk();
    delay(10);
  }
  else
  // Si la Switch est LOW et que la DEL distant est HIGH
  // ça signifie que la led et la switch ne sont pas synchronisé
  if (digitalRead(SW1) == LOW && dataReceived[0] == HIGH)
  {
    dataToSend[0] = dataReceived[0];
    digitalWrite(SW1, HIGH);
    ledStatus1 = HIGH;
    updateBlynk();
    delay(10);
  }
}

/*========================================================*
 *             Update de l'Applicaiton Blynk
 * Actuellement sur mon application Blynk j'utilise 
 * 2 bontons, un servant à envoyer une commande ON/OFF au 
 * NodeMCU qui est directement lié à la PIN 0 (SW1) et 
 * un autre représentant une PIN virtuel (V3), on ne peut 
 * pas updater une PIN réel sur l'application Blynk, on 
 * peut uniquement le faire avec lest PIN virtuel, avec un 
 * peu de code on pourrait utiliser uniquement des PIN 
 * virtuelles.
 * Lien qui pourrait être utile pour utiliser uniquement
 * une pin virtuelle :
 * https://community.blynk.cc/t/virtual-pin-reading/1185/3
/*========================================================*/

void updateBlynk() {
  // envoie à Blynk le nouveau stats de la pin virtuel V3
  Blynk.virtualWrite(V3, ledStatus1);
}

/*========================================================*
 *                 Fonction pour déboguage
/*========================================================*/

void showData() {
        Serial.print("Dernière donnée reçu : ");
        Serial.println(dataReceived[0]);
        Serial.print("Dernière donnée envoyé : ");
        Serial.println(dataToSend[0]);
        Serial.print("État de la Switch : ");
        Serial.println(digitalRead(SW1));
        Serial.print("ÉTat de la DEL : ");
        Serial.println(ledStatus1);
}
