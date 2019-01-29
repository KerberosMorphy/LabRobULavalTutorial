/*
 * CODE SLAVE 
 * Le code est pratiquement identique au code Slave à l'exeption de l'inversion de l'adresse slave et master
 * 
 * Nous avons donc 2 NRF ayant chacun une DEL et une Switch,
 * Le code permet que les 2 NRF aient toujours la DEL allumé ou éteint en même temps
 * et que d'appuyer sur une switch change l'état des 2 NRF.
*/


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

//Une pour émettre, l'autre pour recevoir
//IMPORTANT l'autre NRF devra avoir les configuration INVERSÉ
// CEST A DIRE que l'adresse slave et master seront interchangé
const byte masterAddress[5] = {'R','x','A','A','A'}; // Correspond à l'adresse 0x5278414141LL en HEX
const byte slaveAddress[5] = {'T','X','a','a','a'};// Correspond à l'adresse 0x5458616161LL en HEX

RF24 radio(CE_PIN, CSN_PIN);

int dataReceived[1];
int replyData[1];
// IL s'agit d'un FLAG pour informer que les donnée ne sont pas synchronisé
bool newData = false;

// PIN de la DEL
int LED1 = 3;
// PIN de la Switch
int SW1 = 7;

/*========================================================*
 *            Configuration de départ
/*========================================================*/

void setup()
{
  Serial.begin(115200);
  // DEL est LOW au démarrage
  dataReceived[0] = LOW;
  replyData[0] = dataReceived[0];

  Serial.println("SlaveSwapRoles Starting");
  radio.begin();
  radio.setDataRate( RF24_250KBPS );

  //IMPORTANT: puisque les 2 module NRF auront l'Adresse slave et master interchangé
  //           on a pas a toucher à cette section mais comprenez qu'un NRF écrit dans
  //           dans un PIPE auquel l'autre NRF écoutera
  //On ouvre un PIPE pour l'écriture
  radio.openWritingPipe(slaveAddress);
  // On ouvre un PIPE pour la lecture
  radio.openReadingPipe(1, masterAddress);

  radio.setRetries(3,5); // délai, répétition de l'envoie
  radio.startListening();
  pinMode(LED1, OUTPUT);
}

/*========================================================*
 *                   Routine principale 
 * - On écoute le NRF distant pour savoir s'il y a du 
 *   changement
 * - On écoute notre propre Switch s'il a du changement
 *   et modifie le NRF distant
 * - Option de déboguage
/*========================================================*/

void loop() {
    getData();
    getButton();
    showData();
}

/*========================================================*
 *                      Écoute Du NRF
/*========================================================*/

void getData()
{
  if (radio.available()) // si quelqu'un parle
  {
    radio.read(&dataReceived, sizeof(dataReceived)); // on récolte les données
    Serial.print("Données reçus : ");
    Serial.println(dataReceived[0]);
    newData = true;  // On Lève le FLAG
    if (dataReceived[0] == HIGH){
      Serial.println("HIGH");
      switchLed(HIGH); // on change l'état de la DEL
    } else if (dataReceived[0] == LOW){
      Serial.println("LOW");
      switchLed(LOW); // on change l'état de la DEL
    }
    replyData[0] = dataReceived[0]; // On synchronise la varaible d'envoie de donnée avec la réception
    newData = false; // on baisse le FLAG
  }
}

/*========================================================*
 *                Écoute De la Switch
/*========================================================*/

void getButton(){
  // si la switch est HIGH donc on pèse actuellement dessus
  if (digitalRead(SW1) == HIGH){
    int temp = digitalRead(LED1); // on stock le dernière état de la DEL
    if (temp == LOW) // si elle était low
    {
      delay(100);
      replyData[0] = HIGH; // donnée envoyé deviendra la nouvelle donnée
      newData = true; // on lève le FLAG car les donnée on changé
      delay(100);
    }
    else
    if (temp == HIGH)
    {
      delay(100);
      replyData[0] = LOW; // donnée envoyé deviendra la nouvelle donnée
      newData = true; // on lève le FLAG car les donnée on changé
      delay(100);
    }
    if (newData == true){
      // On envoie les donnée au NRF distant
      // HIGH == 1 et LOW == 0
      // le modul permet d'inversé la valeur
      send((temp+1)%2);
    }
  }
}

/*========================================================*
 *            Envoie d'information au NRF distant
/*========================================================*/

void send(int state) {
  radio.stopListening();
    bool rslt;
    rslt = radio.write( &replyData, sizeof(replyData) );
  radio.startListening();

  Serial.print("Réponse envoyé, ");
  Serial.print(replyData[0]);

  if (rslt) {
    Serial.println(" a été reçu");
    dataReceived[0] = replyData[0];
    newData = false; // Puisque reçu la donnée n'est plus nouvelle, on baisse le FLAG
    switchLed(state);
  }
  else {
    Serial.println(" a échoué");
  }
  Serial.println();
}

/*========================================================*
 *                  Mettre à jour la DEL
/*========================================================*/

void switchLed(int state)
{ 
  delay(10);
  digitalWrite(LED1, state);
  delay(10);
}

/*========================================================*
 *                 Fonction pour déboguage
/*========================================================*/

void showData() {
        Serial.print("Dernière donnée reçu : ");
        Serial.println(dataReceived[0]);
        Serial.print("Dernière donnée envoyé : ");
        Serial.println(replyData[0]);
        Serial.print("État de la Switch : ");
        Serial.println(digitalRead(SW1));
        Serial.print("ÉTat de la DEL : ");
        Serial.println(digitalRead(LED1));
        Serial.print("La donnée est nouvelle : ");
        Serial.println("%b",newData);
}