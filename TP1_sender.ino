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
int i = 0;

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
}

void loop() {
  Serial.println("Sending message to MQTT topic..");
  //envoi sur le topic des données de paramétrage de Lora : freq, sf, sb
  client.publish("srt5/GEFY", "870000000;12;125000");
  client.loop();

  //envoi du paquet Lora avec les valeurs d1 et d2
  Serial.println("Sending message to Lora");
  float d1=12.0, d2=321.54;
  LoRa.beginPacket(); // start packet
  sdp.data[0]=d1;
  sdp.data[1]=d2;
  LoRa.write(sdp.frame,16);
  LoRa.endPacket();
  d1++; d2+=2;
  delay(5000);
}
