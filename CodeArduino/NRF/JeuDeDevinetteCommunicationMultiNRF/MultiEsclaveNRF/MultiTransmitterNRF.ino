// CODE POUR LES SLAVES

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

/*
 *
 * IMPORTANT: modifier le node, si vous utilisez 4 récepteur chacun devrait avoir un NODE différent.
 * le nombre doit être entre 1 et 6 puisqu'un module NRF ne support que 6 pipe de communication
 * 
 */
#define WHICH_NODE 2

// Liste des adresse de communication, seul 6 canal son possible, le "LL" à la fin signifi LongLong Type
// on pourrait aisément créer un réseau plus étendu exemple : quand un récepteur trouve le nombre il devient émetteur et essaie de le faire deviner à 5 autre module
// ainsi de suite jusqu'à avoir atteint le nombre maximal d'Adresse possible
const uint64_t wAddress[] = {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL, 0xB3B4B5B60FLL, 0xB3B4B5B605LL};

// l'adresse du Pipe utiliser sera défini grace au numéro du node (qui représentera l'Mindex (Node-1) dans la liste des adresse)
const uint64_t PTXpipe = wAddress[ WHICH_NODE - 1 ];

// utilisé pour compter les paquets envoyé
byte counter = 1; 

// Utilisé pour définir l'arrêt d'envoie de paquet
bool done = false; 


void setup()  

{
  // Démarre la communication série au baudrate indiqué
  Serial.begin(115200);   

  // pour la génération aléatoire de nombre
  randomSeed(analogRead(0));

  // Démarre le module nRF
  radio.begin();            
  
  // Défini la communicaiton comme étant de court distance
  // Vous devrez modifier cette valeur pour l'utiliser sur de plus longue distance
  radio.setPALevel(RF24_PA_LOW);  
  // Tout comme sur WiFi il y a des canaux, les canaux plus élèvé tendent à être plus "ouvert"
  // un canal saturé aura plusd e risque d'interférence
  radio.setChannel(108);          

  // On démare la lecture sur le Pipe choisi
  radio.openReadingPipe(0,PTXpipe);  

  // On arrête la lecture pour transférer vers l'écriture
  radio.stopListening(); 

}

void loop()  

{
   // Lorsqu'on aura deviné le bon nomvre, cette condition ne sera pas exécuté
   if(!done) { 

     byte randNumber = (byte)random(11); // on choisi un nombre aléatoire entre 0 et 10

     radio.openWritingPipe(PTXpipe);        // on ouvre le Pipe pour communiquer

     if (!radio.write( &randNumber, 1 )){  // si on arrive pas a émettre les données

         Serial.println("L'envoie de notre suposition à échoué");      

     }

     else { //si l'émission de donnée est réussie

          Serial.print("La suposition qui a été envoyé avec succès : ");

          Serial.println(randNumber);

       

        radio.startListening(); // on retourne en mode écoute

        unsigned long startTimer = millis(); //un timer pour ne par rester indéfiniment en attente de réception

        bool timeout = false; 

        while ( !radio.available() && !timeout ) { // si aucune donnée n'est reçu mais que le timeout n'Est pas écoulé

          if (millis() - startTimer > 200 ) timeout = true; 
          // lorsque le timeout à dépassé les 200ms timeout == True
          // cela signifi que nous n'avons pas trouvé le bon nombre

        }

    

        if (timeout) Serial.println("Le dernière essaie était mauvais, réessayez"); // simple retour d'information si le timeout est atteint

        else  { //Si on recoit une donnée

          byte daNumber; //variable qui détiendra la donnée

          radio.read( &daNumber,1); //on lis la donnée que l'on store dans daNumber

          if(daNumber == randNumber) { // Si daNumber est la valeur retourné nous avons trouvé le bon nombre

            Serial.println("Vous avez trouvé le bon nombre, vous avez terminé votre travail");

            done = true; //on indique que le travail est terminé

          }

          else Serial.println("Quelque chose d'inatendu c'est produit, continuez d'essayer"); //Si cela s'Affiche, un problème est survenu avec le code, ceci ne devrait jamais apparaitre

        }

        radio.stopListening(); //arrête l'écoute et retourne à l'écriture

     }

   }

    delay(1000);

}
