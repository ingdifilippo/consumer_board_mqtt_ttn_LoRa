#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include "secret.h"
#include <ArduinoJson.h>

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "eu1.cloud.thethings.network";
int        port     = 1883;
//const char topic[]  = "v3/app1@tenant1/devices/dev1/up";

const char topic[]  = "#";

String inputString = "";

 // Allocate the JSON document
JsonDocument doc;

//const char* input = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(SECRET_SSID);
  while (WiFi.begin(SECRET_SSID, SECRET_PASS) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  mqttClient.setUsernamePassword("lorawan-e5-board2", "NNSXS.FFXRDDZVBUWZPXZHCHDL7LWMMXXBQB5IZMGHQ4Q.45RR4QYOOOGM2XXTA2TECMIVNU7SJESWX42I6YIAMJAPPUH3NI3Q");


  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

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
const char* sensor = doc["end_device_ids"]["device_id"];
  Serial.println(sensor);
  Serial.println();
}
