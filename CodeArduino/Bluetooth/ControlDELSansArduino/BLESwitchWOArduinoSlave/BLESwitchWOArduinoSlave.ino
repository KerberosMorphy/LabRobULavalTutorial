//  CODE SLAVE
//  Liaison entre 2 HM-10
//
//  Une fois configuré le slave n'a besoin d'un arduino
//  Il est possible de connecter le PushButton directement sur le HM-10
//  Nous utiliseront les pin I/O controlé directement pas le HM-10
//
//  Interface de communication entre 2 module BLE
//
//  Contrôle d'une DEL
//
 
#include <AltSoftSerial.h>
AltSoftSerial BTserial; 
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
 
 
char c=' ';
boolean NL = true;

// À changer si vous modifiez le baudrate
int baudrate = 9600;
 
void setup() 
{
    Serial.begin(baudrate);
    Serial.println("Slave Module");
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");

    BTserial.begin(baudrate);
    Serial.println("BTserial started at " + baudrate);
    Serial.println(" ");

    // Configure la pin de la DEL comme un OUTPUT
    pinMode(LEDpin, OUTPUT); 
    digitalWrite(LEDpin,LOW); 
}
 
void loop()
{
    // Lire ce que le module BLE reçois et l'écrire
    // si c'est une commande AT il sera exécuté
    if (BTserial.available())
    {
        c = BTserial.read();
        Serial.write(c);
    }
 
 
    // Interface pour lire et écrire via le moniteur série
    if (Serial.available())
    {
        c = Serial.read();
 
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
