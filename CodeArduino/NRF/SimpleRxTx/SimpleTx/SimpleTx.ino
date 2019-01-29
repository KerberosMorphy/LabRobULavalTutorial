

// SimpleTx - NRF Master Émetteur

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define CE_PIN   9
#define CSN_PIN 10

// Notez qu'il s'agit ici de 5 adresse ou pipe de communication
// chacun des 5 adresse pourrait être un récepteur différent
const byte slaveAddress[5] = {'R','x','A','A','A'};


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

// Array doit avoir la même longueur que le dataReceived dans le RX code
char dataToSend[10] = "Message 0";
char txNum = '0';


unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second


void setup() {

    Serial.begin(9600);

    Serial.println("SimpleTx Starting");

    radio.begin();
    radio.setDataRate( RF24_250KBPS );

    // Le premier réprente le délai, si le récepteur n'a pas confirmé la réception dans ce délai on renvoie la donné
    // Le second est le nombre de fois qu'on renvoie une donné avant d'abbandonné l'envoi
    radio.setRetries(3,5); 

    // on va écrire la même information pour tous les slaves
    radio.openWritingPipe(slaveAddress);
}

//====================

void loop() {
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
        prevMillis = millis();
    }
}

//====================

void send() {

    bool rslt;
    rslt = radio.write( &dataToSend, sizeof(dataToSend) );
        // Le premier est la donné à envoyer
        // Le second est la taille de la donné envoyer
        // il est conseillé de toujours utiliser sizeof()

    Serial.print("Data Sent ");
    Serial.print(dataToSend);
    if (rslt) {
        Serial.println("  Acknowledge received");
        updateMessage();
    }
    else {
        Serial.println("  Tx failed");
    }
}

//================

void updateMessage() {
        // Petite routine pour envoyer de nouvelle données.
    txNum += 1;
    if (txNum > '9') {
        txNum = '0';
    }
    dataToSend[8] = txNum;
}
