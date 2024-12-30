#include <ArduinoMqttClient.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <WiFi.h>
#include "secret.h"
#include <ArduinoJson.h>

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
TFT_eSPI tft = TFT_eSPI();
  


const char broker[] = "eu1.cloud.thethings.network";
int        port     = 1883;
//const char topic[]  = "v3/app1@tenant1/devices/dev1/up";

const char topic[]  = "#";

String inputString = "";

 // Allocate the JSON document
JsonDocument doc;

//const char* input = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 1);
  tft.setTextSize(1);
  tft.println("The Thing Network data consumer\n");
  tft.println("IIS AOSTA 2025 ");
  tft.println("Elettronica");
  delay(2000);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // attempt to connect to Wifi network:
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 1);
  tft.println("Tentativo di connessione");
  tft.print("al router: ");
  tft.setTextColor(TFT_GREEN);
  tft.print(SECRET_SSID);
  tft.setTextColor(TFT_WHITE);

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(SECRET_SSID);
  while (WiFi.begin(SECRET_SSID, SECRET_PASS) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    tft.print(".");
    delay(2000);
  }
  tft.setTextColor(TFT_GREEN);
  tft.println("\n[ok]");
  tft.setTextColor(TFT_WHITE);
  Serial.println("You're connected to the network");
  Serial.println();

  tft.println("\nTentativo di connessione");
  tft.println("al server MQTT-TTN:  ");
  tft.println(broker);
  
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  mqttClient.setUsernamePassword("lorawan-e5-board2", "NNSXS.FFXRDDZVBUWZPXZHCHDL7LWMMXXBQB5IZMGHQ4Q.45RR4QYOOOGM2XXTA2TECMIVNU7SJESWX42I6YIAMJAPPUH3NI3Q");


  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    tft.setTextColor(TFT_RED);
    tft.print("MQTT connessione fallita! Error code = ");
    tft.println(mqttClient.connectError()); 
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
  tft.setTextColor(TFT_GREEN);
  tft.println("[ok]");
  tft.println();
  tft.setTextColor(TFT_WHITE);

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  tft.print("Sottoscrizione al topic: ");
  tft.println(topic);
  tft.println();

  // subscribe to a topic
  mqttClient.subscribe(topic);
  

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  Serial.print("Topic: ");
  Serial.println(topic);


  Serial.println();

 

  
}

void loop() {
  // call poll() regularly to allow the library to receive MQTT messages and
  // send MQTT keep alive which avoids being disconnected by the broker
  mqttClient.poll();
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 1);
  tft.println("Ricevuto un messaggio! ");
  
  tft.print("lunghezza: ");
  tft.print(messageSize);
  tft.setTextColor(TFT_GREEN);
  tft.println(" bytes\n");
  tft.setTextColor(TFT_WHITE);
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  
  char inChar;
  
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
  
     // get the new byte:
    inChar = (char)mqttClient.read();;
    // add it to the inputString:
    inputString += inChar;
    //Serial.print(inChar);
  }
  

  Serial.println(inputString);
  const char* input = inputString.c_str();
  
   // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, input);

  // Test if parsing succeeds
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  const char* device = doc["end_device_ids"]["device_id"];
  const char* rcv = doc["received_at"];
  int hr = doc["uplink_message"]["decoded_payload"]["payload"]["humidity"];
  int tmp = doc["uplink_message"]["decoded_payload"]["payload"]["temperature"];
 
  tft.println("dispositivo: ");
  tft.setTextColor(TFT_GREEN);
  tft.println(device);
  tft.setTextColor(TFT_WHITE);
  tft.println();
  tft.println(rcv);

  tft.println("Uplink hr: ");
  tft.setTextColor(TFT_GREEN);
  tft.println(hr);
   tft.setTextColor(TFT_WHITE);
  tft.println();

  tft.println("Uplink temp: ");
  tft.setTextColor(TFT_GREEN);
  tft.println(tmp);
  tft.setTextColor(TFT_WHITE);
  tft.println();

  Serial.println(device);
  tft.setTextColor(TFT_PURPLE);
  Serial.println("Ricezione messaggio: ");
  tft.setTextColor(TFT_GREEN);
  Serial.println(rcv);
  Serial.println();
  
  Serial.println("Uplink msg: ");
  Serial.println(hr);
  Serial.println();
}
