//  CODE MASTER
//  Liaison entre 2 HM-10
//
//  Interface de communication entre 2 module BLE
//
//  Contrôle d'une DEL
//
//  Pins
//  BT VCC vers Arduino 5V. 
//  BT GND vers GND
//  Arduino D8 (SS RX) - BT TX pas besoin d'un diviseur de tension
//  Arduino D9 (SS TX) - BT RX BESOIN D'UN DIVISEUR DE TENSION (5v vers 3.3v)
//  Arduino D10 - PushButton
//

#include <AltSoftSerial.h>
AltSoftSerial BTserial;
// Libraire disponible à cette adresse
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html

char input = ' ';
boolean NL = true;

// À changer si vous modifiez le baudrate
int baudrate = 9600;

// configuration de la pin pour le PushButton
byte switchPin = 10;
boolean switch_State = LOW;
boolean oldswitch_State = LOW;

void connect()
{
  // IMME1 == Attendre AT+START et AT+CONN pour établir connection (Master)
  // IMME0 == mode par défaut (Slave)
  BTserial.print("AT+IMME1");
  delay(1000);  
  // ROLE1 == Central (Master)
  // ROLE0 == Peripherique (Slave)
  BTserial.print("AT+ROLE1");
  delay(1000);  
  // Dépendemment de la version du firmware,
  // un RESET est nécessaire pour que le module change de role
  BTserial.print("AT+RESET");
  delay(1000);  
  // AT+CONN[Peripherique] ce connect à l'adresse du périphérique demandé
  BTserial.print("AT+CONNA810871BFB8F");
  delay(1000);
  // Débute sa communication
  BTserial.print("AT+START");
  delay(1000);
}

void setup()
{
  Serial.begin(baudrate);
  Serial.println("Master Module");
  Serial.print("Sketch:   ");   Serial.println(__FILE__);
  Serial.print("Uploaded: ");   Serial.println(__DATE__);
  Serial.println(" ");

  BTserial.begin(baudrate);
  Serial.println("BTserial started at " + baudrate);
  Serial.println(" ");

  // Configure la pin du PushButton comme un INPUT
  pinMode(switchPin, INPUT);
  connect();
}

void loop()
{
    // Simple vérification de la validité de la lecture de l'Input.
    boolean state1 = digitalRead(switchPin); delay(1);
    boolean state2 = digitalRead(switchPin); delay(1);
    boolean state3 = digitalRead(switchPin); delay(1);
    if ((state1 == state2) && (state1==state3))  
    { 
        switch_State = state1;  
 
        if (switch_State != oldswitch_State)
        {
              // Envoie 1 ou 0 au module esclave pour ouvrir ou fermer la DEL
              // Affiche 1 ou 0 au moniteur série
              if ( switch_State == HIGH) { 
                BTserial.print("1" );  
                Serial.println("Signal ON envoyé");
              }
              else { 
                BTserial.print("0" );  
                Serial.println("Signal OFF envoyé");
              }
 
              oldswitch_State = switch_State;
        }
    }
}
