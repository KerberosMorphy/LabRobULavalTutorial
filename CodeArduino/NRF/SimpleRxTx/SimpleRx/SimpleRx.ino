

// SimpleRx - NRF Slave récepteur

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

//===========

void setup() {

    Serial.begin(9600);

    Serial.println("SimpleRx Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    // on va lire  le pipe 1 de la liste d'Adresse thisSlaveAddress
    radio.openReadingPipe(1, thisSlaveAddress);
    radio.startListening();
}

//=============

void loop() {
    getData();
    showData();
}

//==============

// Lorsqu'on recoit de nouvelle donnée, on met le bool à vrai
void getData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        newData = true;
    }
}

// si le bool est a vrai on affiche la donnée sur le moniteur série et remet la bool à faux
void showData() {
    if (newData == true) {
        Serial.print("Data received ");
        Serial.println(dataReceived);
        newData = false;
    }
}
