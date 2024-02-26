# Tp_IoT_2022_LoRa_Bluetooth

# Documentation du Code

## Introduction
Ce code est conçu pour être exécuté sur un ESP32. Il permet la connexion à un réseau WiFi, la communication avec un broker MQTT, et l'envoi de données à la fois via WiFi et LoRa.

## WiFi et MQTT
1. **WiFi.h** et **PubSubClient.h** :
   - Ces bibliothèques permettent respectivement d'établir une connexion WiFi avec un point d'accès et de communiquer avec un broker MQTT.
   - `WiFi.begin(ssid, password)` est utilisé pour se connecter au réseau WiFi spécifié avec le SSID et le mot de passe fournis.
   - La classe `PubSubClient` permet au dispositif de publier et de souscrire à des sujets sur un broker MQTT.

## LoRa

2. **LoRa.h** :
   - Cette bibliothèque est utilisée pour la communication sans fil longue portée (LoRa).
   - Les broches SPI (SCK, MISO, MOSI, SS) sont définies pour la communication avec le module LoRa.
   - `LoRa.begin(frequence)` est utilisé pour démarrer la communication LoRa à la fréquence spécifiée.

3. **Union `pack`** :
   - Cette union est utilisée pour représenter les données à envoyer par LoRa.
   - Elle contient deux structures :
     - `frame[16]` : Un tableau d'octets pour les trames.
     - `data[4]` : Un tableau de 4 valeurs en virgule flottante.

4. **Fonctions `callback` et `lora_init`** :
   - `callback` est une fonction de rappel appelée lorsqu'un message MQTT est reçu.
     - Elle extrait les données du message reçu et les utilise pour initialiser la configuration LoRa.
   - `lora_init` est utilisée pour initialiser la communication LoRa avec les paramètres spécifiés.

## Sender
La boucle principale du code se compose des étapes suivantes :
1. **Envoi des données MQTT**: Le code publie des données sur un topic MQTT.
2. **Envoi des données LoRa**: Le code envoie un paquet LoRa contenant des valeurs en virgule flottante.
### Etapes :
1. Connexion au réseau WiFi.
2. Connexion au broker MQTT avec les identifiants spécifiés.
3. Publication des données de configuration LoRa (fréquence, facteur d'étalement, bande passante) sur le topic MQTT.
4. Envoi d'un paquet LoRa contenant deux valeurs (d1 et d2) sur lesquelles un incrément est effectué à chaque itération de la boucle. 

## Receiver
1. **`setup()`** : Initialisation la communication série, connexion au réseau WiFi, configuration des broches SPI et LoRa, connexion au broker MQTT et abonnement au topic MQTT sélectionné. Ensuite nous initialisons la connexion Lora avec les paramètres récupérés du topic.

2. **`loop()`** : Cette fonction gère les communications MQTT et LoRa.
Elle récupère les données reçues via LoRa et les affiche avec la force du signal en réception.

### Etapes :
1. **Connexion au réseau WiFi** : Le programme commence par se connecter au réseau WiFi spécifié en utilisant les identifiants fournis (SSID et mot de passe).
2. **Connexion au broker MQTT** : Ensuite, une connexion est établie avec le broker MQTT situé à l'adresse spécifiée (test.mosquitto.org) sur le port MQTT standard (1883). Le client MQTT utilise des identifiants vides pour la connexion.
3. **Abonnement au topic MQTT** : Le client s'abonne au topic MQTT spécifié ("srt5/GEFY"), prêt à recevoir des données publiées sur ce topic.
4. **Réception des paramètres LoRa** : Lorsqu'un message est reçu sur le topic MQTT, la fonction de rappel (callback) est déclenchée. Cette fonction extrait les paramètres de connexion LoRa (fréquence, facteur d'étalement, bande passante) à partir du message reçu et initialise les paramètres LoRa en conséquence.
5. **Initialisation LoRa** : Une fois les paramètres LoRa reçus, le module LoRa est initialisé avec ces paramètres.
6. **Écoute des données LoRa** : Dans la boucle principale, le programme vérifie continuellement la réception de paquets LoRa. Lorsqu'un paquet LoRa est reçu, les données sont extraites et affichées.

## Améliorations possibles
1. **Utilisation de la structure (typedef struct tcp)** :
La structure tcp est définie pour représenter une structure de données contenant une adresse IP, un port et un mot de passe. Cela permet de regrouper ces informations connexes en une seule entité, ce qui facilite la manipulation et le passage de ces données dans le programme.
Cette structure est utilisée pour définir le type de données tcp qui est utilisé pour créer des instances de cette structure.

2. **Utilisation de l'union (typedef union rdp)** :

L'union rdp est utilisée pour manipuler les données soit sous forme de tableau de bytes, soit sous forme de structure de données.
Dans ce cas, l'union permet de stocker les données sous forme de tableau de bytes (byte byteArray[union_size]), qui peut être utilisé pour une manipulation directe des données reçues via LoRa.

Structure TCP à implémenter :
```
typedef struct tcp{
  uint8_t ip[4];
  uint32_t port;
  char mdp[8];
};
const int union_size = sizeof(tcp);
typedef union rdp{
 tcp structure;
 byte byteArray[union_size]; /* you can use other variable types if you want. Like: a 32bit integer if you have 4 8bit variables in your struct */
};
rdp tcp1;
// print en fct de structure
for(int i=0; i < union_size; i++)
{
Serial.print(tcp1.structure.ip);
Serial.print("\t");
};
```