

// SimpleRxAckPayload- NRF Slave récepteur
// Il s'agit de la même chose que sans le payload, la différence est que le récepteur va
// écrire à l'émetteur comme retour via les fonction payload de la librairie

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

// Notez qu'il s'agit ici de 5 adresse ou pipe de communication
const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

// Array doit avoir la même longueur que le dataToSend dans le TX code
char dataReceived[10]; 
bool newData = false;

 // the two values to be sent to the master
int ackData[2] = {109, -4000};

//==============

void setup() {

    Serial.begin(9600);

    Serial.println("SimpleRxAckPayload Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, thisSlaveAddress);

    radio.enableAckPayload();
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // pre-load data

    radio.startListening();
}

//==========

void loop() {
    getData();
    showData();
}

//============

void getData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        updateReplyData();
        newData = true;
    }
}

//================

void showData() {
    if (newData == true) {
        Serial.print("Data received ");
        Serial.println(dataReceived);
        Serial.print(" ackPayload sent ");
        Serial.print(ackData[0]);
        Serial.print(", ");
        Serial.println(ackData[1]);
        newData = false;
    }
}

//================

void updateReplyData() {
    ackData[0] -= 1;
    ackData[1] -= 1;
    if (ackData[0] < 100) {
        ackData[0] = 109;
    }
    if (ackData[1] < -4009) {
        ackData[1] = -4000;
    }
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // load the payload for the next time
}
