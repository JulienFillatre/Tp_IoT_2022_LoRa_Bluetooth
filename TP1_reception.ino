#include <WiFi.h>
#include <PubSubClient.h>
#include <LoRa.h>
#include <SPI.h>
#define SCK 5    // GPIO5 -- SX127x's SCK
#define MISO 19  // GPIO19 -- SX127x's MISO
#define MOSI 27  // GPIO27 -- SX127x's MOSI
#define SS 18    // GPIO18 -- SX127x's CS
#define RST 14   // GPIO14 -- SX127x's RESET
#define DI0 26   // GPIO26 -- SX127x's IRQ(Interrupt Request)
// WiFi Pub Sub
WiFiClient espClient;
PubSubClient client(espClient);
const char *ssid = "Raspi-web";     // Enter your WiFi name
const char *password = "azertyui";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org";
const char *topic = "srt5/GEFY";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

//variables
int frequence = 0;
int sf1 = 0;
int sb1 = 0;
float d1, d2;
int i = 0, rssi = 0;

union pack
{
  uint8_t frame[16]; // trames avec octets
  float data[4]; // 4 valeurs en virgule flottante
} sdp ; // paquet d’émission lora

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  pinMode(DI0, INPUT);  // signal interrupt
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  //client connected with id + mac adress
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // publish and subscribe
  //client.publish(topic, "Hi EMQX I'm julien ^^");
  //récupération des paramètres de connexion Lora
  client.subscribe(topic);
}

bool doCallback = true;
void callback(char *topic, byte *payload, unsigned int length) {
  if (!doCallback) return;
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.println("-----------------------");
  char message[length];  // +1 pour le caractère de fin de chaîne '\0'
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  //fonction permettant d'isoler les 3 variables réceptionnées : freq, sf1, sb1
  sscanf(message, "%d;%d;%d", &frequence, &sf1, &sb1);
  Serial.println(frequence);
  Serial.println(sf1);
  Serial.println(sb1);
  //initialisation des paramètres de Lora
  lora_init();
  //permet d'executer une seule fois 
  doCallback = false;
}

void lora_init() {
  if (!LoRa.begin(frequence)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("Starting LoRa good!");
  LoRa.setSpreadingFactor(sf1);
  LoRa.setSignalBandwidth(sb1);
  Serial.println("lora initialized!");
}

void loop() {
  client.loop();
  int packetLen;
  packetLen = LoRa.parsePacket();
  if (packetLen == 16) {
    i = 0;
    //lecture du bytearray
    while (LoRa.available()) {
      rdp.byteArray[i] = LoRa.read();
      i++;
    }
    d1=rdp.data[0];d2=rdp.data[1];
    rssi = LoRa.packetRssi();  // force du signal en réception en dB
    Serial.println("reception Lora : ");
    Serial.println(d1); //valeur 1 reçue
    Serial.println(d2); //valuer 2 reçue
    Serial.print(rssi);
    Serial.print(" dBm"); // force du signal en réception en dB
  }
}
