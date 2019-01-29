// CODE POUR LE MASTER

// Exemple démontrant la capacité de multicommunication, 
// Il s'Agit d'un pattern étoile (1 transmetteur plusieur récepteur indépendant)

// Un émetteur choisi un nombre au hasard et demande au récepteur de le deviner
// les récepteur choisissent nombre au hasard et renvoi la réponse,
// l'émetteur leur retourne si la réponse est bonne ou non, 
// lorsqu'un récepteur trouve la bone réponse il arrête sa communication.

#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

// CE est utilisé pour configuer le nRF en mode standby(0) ou actif(1)
const int pinCE = 9;

// CSN est utilisé pour dire au nRF si la communication SPI est une commande ou un message
const int pinCSN = 10; 

//Créer un objet RF24 avec les pin correspondant au CE et CSN
RF24 radio(pinCE, pinCSN); 

// Liste des adresse de communication, seul 6 canal son possible, le "LL" à la fin signifi LongLong Type
const uint64_t rAddress[] = {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL, 0xB3B4B5B60FLL, 0xB3B4B5B605LL };

// le nombre que les esclave tentent de deviner
byte daNumber = 0; 

void setup()   
{
  // pour la génération aléatoire de nombre
  randomSeed(analogRead(0));

  // on choisi un nombre aléatoire entre 0 et 10
  daNumber = (byte)random(11);

  // Démarre la communication série au baudrate indiqué
  Serial.begin(115200);   

  Serial.print("The number they are trying to guess is: "); 
  Serial.println(daNumber); //print the number that they have to guess

  Serial.println();
  radio.begin();  //Start the nRF24 module

    // Défini la communicaiton comme étant de court distance
  // Vous devrez modifier cette valeur pour l'utiliser sur de plus longue distance
  radio.setPALevel(RF24_PA_LOW);  
  // Tout comme sur WiFi il y a des canaux, les canaux plus élèvé tendent à être plus "ouvert"
  // un canal saturé aura plusd e risque d'interférence
  radio.setChannel(108);   

  // Ouvre les 6 canaux de communication PIPE qui représente les 6 modules qui tenteront de deviner le nombre
  // On pourrait très bien en faire une boucle
  radio.openReadingPipe(0,rAddress[0]);
  radio.openReadingPipe(1,rAddress[1]);
  radio.openReadingPipe(2,rAddress[2]);
  radio.openReadingPipe(3,rAddress[3]);
  radio.openReadingPipe(4,rAddress[4]);
  radio.openReadingPipe(5,rAddress[5]);
  
  // on début l'écoute
  radio.startListening();
}

void loop()  
{   
    // Variable qui contient le PIPE de communication a qui on veut envoyer une donnée
    byte pipeNum = 0;
    //Utiliser pour conserver le payload du module qui répond(la suposition)
    byte gotByte = 0; 
    
    while(radio.available(&pipeNum)){ //Si des donnée arrive enregistrer l'adresse du PIPE
     radio.read( &gotByte, 1 ); //lire 1 byte de donnée et le stocker dans gotByte
     Serial.print("Suposition reçu de l'esclave numéro : "); 
     Serial.println(pipeNum + 1); //Affiche le numéro de l'Esclave
     Serial.print("Sa suposition est : ");
     Serial.println(gotByte); //affiche le payload qui correspond à la suposition de l'esclave
     if(gotByte != daNumber) { //si ça ne correspond pas on bon nombre
      Serial.println("Échoué!! essayez à nouveau."); 
     }
     else { //sinon ça signifi que ça correspond
      if(sendCorrectNumber(pipeNum)) Serial.println("Super! Tu as terminé."); //on retourne la bonne valeur à l'esclave pour lui indiquer qu'il à terminé
      else Serial.println("Lenvoie à échoué"); //si ça ce produit, un erreur c'Est produit dans l'envoie
     }
     Serial.println();
    }

   delay(200);    
}


// Cette fonction modifi le Master pour qu'il puisse écrire à un esclave et ensuite le remet en mode écoute
// ceci ce produit uniquement si l'Esclave à envoyer la bonne réponse
bool sendCorrectNumber(byte xMitter) {
    bool worked; //Variable pour faire un suivi de l'envoie et valider son succès
    radio.stopListening(); //Arrête l'écoute
    radio.openWritingPipe(rAddress[xMitter]); //Ouvre un canal pour écrire à l'Adresse indiqué
    if(!radio.write(&daNumber, 1))  worked = false; //si l'envoie a échoué worked devient faux
    else worked = true; //si l'Envoie réussis
    radio.startListening(); //on retourne en mode écoute
    return worked;  //on retourne le résultat de l'envoie
}
