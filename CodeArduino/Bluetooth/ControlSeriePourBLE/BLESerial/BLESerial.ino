//  Communication serial avec module Bluetooth
//
//  La communication ce fait via un port COMM avec l'interface de l'IDE d'Arduino, PuTTy ou autre
//
//  Tous ce qui est entré via le moniteur série est transféré au module
//  Tous ce qui est reçu par le module est copié dans le moniteur série
//  N'envoie pas de retour de linge au module BLE
//
//  Pins
//  BT VCC vers Arduino 5V. 
//  BT GND vers GND
//  Arduino D8 (SS RX) - BT TX pas besoin d'un diviseur de tension
//  Arduino D9 (SS TX) - BT RX BESOIN D'UN DIVISEUR DE TENSION (5v vers 3.3v)
//
 
#include <AltSoftSerial.h>
AltSoftSerial BTserial; 
// Libraire disponible à cette adresse
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
 
 
char c=' ';
boolean NL = true;

// À changer si vous modifiez le baudrate
int baudrate = 9600;
 
void setup() 
{
    Serial.begin(baudrate);
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");
 
    BTserial.begin(baudrate);
    Serial.println("BTserial started at " + baudrate);
}
 
void loop()
{
    // Lire ce que le module BLE reçois et le copier sur le moniteur série.
    if (BTserial.available())
    {
        c = BTserial.read();
        Serial.write(c);
    }
 
 
    // Lire ce que le moniteur série reçois et l'envoyer au module BLE.
    if (Serial.available())
    {
        c = Serial.read();
 
        // Caractère à ne pas considérer lors de l'envoie :
        // 10 == LF (Line feed, saut de ligne)
        // 13 == CR (Carriage Return, retour de ligne)
        // Vous pouvez en ajouter d'autre si pertinent.
        if (c!=10 & c!=13 ) 
        {  
             BTserial.write(c);
        }
 
        // Copie votre input dans le moniteur série 
        // Si une nouvelle ligne est détecté affiche le caractère ">"
        if (NL) { Serial.print("\r\n>");  NL = false; }
        Serial.write(c);
        if (c==10) { NL = true; }
    }
}
