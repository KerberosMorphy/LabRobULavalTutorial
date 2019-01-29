//  CODE SLAVE
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
//  Arduino D13 - DEL
//
 
#include <AltSoftSerial.h>
AltSoftSerial BTSerial; 
 
char c=' ';

// À changer si vous modifiez le baudrate
int baudrate = 9600;

// configuration de la pin pour la DEL
byte LEDpin = 13;
 
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
    // Lire ce que le module BLE reçois et défini si c'est une commande
    if (BTSerial.available())
    {
        c = BTSerial.read();
 
        // 49 == "1" en ASCII
        // 48 == "0" en ASCII
        if (c==49)   { digitalWrite(LEDpin,HIGH);   }
        if (c==48)   { digitalWrite(LEDpin,LOW);    }
        Serial.println(c);
    }
 
}
