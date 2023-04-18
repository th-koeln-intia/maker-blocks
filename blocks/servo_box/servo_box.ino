// Dieses Sketch steuert einen Servo. Dieser Wird in eine Kiste verbaut und somit kann die Kiste über MQTT geöffnet und verschlossen werden.

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <string.h>
#include <ArduinoJson.h>

// Wifi
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

const char* MQTT_BROKER = "intia.local";
const char* TOPIC = "intia/kiste"; // Toppic an das gesendet wird
const char* CLIENT_ID ="Kiste"; // Bitte für jeden ESP eine individuele ClientID festlegen

const char* TOPIC_SUB = "intia/kiste/set"; // Topic das aboniert wird
//Wird an dieses Topic "open" gesendet so öffnet sich die Kiste. "close" zum schließen.


WiFiClient espClient;
PubSubClient client(espClient);

const int capacity = JSON_OBJECT_SIZE(50);
StaticJsonDocument<capacity> doc;
Servo servo;

const int servoPin = D4;     // Servo-Pin
const int ledPin =  D2;      // LED-Pin


void setup() {
  //Testing connecting with onboard led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);  
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.setHostname(CLIENT_ID);
  //first parameter is name of access point, second is the password
  wifiManager.autoConnect(CLIENT_ID);
  Serial.println("Starting Box...");
  /*
  pinMode(ledPin, OUTPUT);
  servo.attach(servoPin); //D4
  servo.write(98);
  Serial.begin(115200);
  delay(1000);
  servo.detach();
 
  */
  servo.attach(servoPin);
  servo.write(96);
  delay(1000);
  servo.detach();
  
  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message [");
  Serial.print(topic);
  Serial.print("] ");
  char msg[length + 1];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msg[i] = (char)payload[i];
  }
  Serial.println();

  msg[length] = '\0';
  Serial.println(msg);

  if (strcmp(msg, "on") == 0) {
    digitalWrite(ledPin, HIGH);
  }
  else if (strcmp(msg, "off") == 0) {
    digitalWrite(ledPin, LOW);
  }
  else if (strcmp(msg, "open") == 0) {
  servo.attach(servoPin);
    servo.write(288);
  delay(1000);
  servo.detach();
  }
  else if (strcmp(msg, "close") == 0) {
  servo.attach(servoPin);
    servo.write(96);
  delay(1000);
  servo.detach();
  }
  else {
    int val = strtol(msg, NULL, 16);
  servo.attach(servoPin);
    Serial.println(val);
    servo.write(val);
  delay(1000);
  servo.detach();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Reconnecting MQTT...");
    if (!client.connect(CLIENT_ID)) {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
  client.subscribe(TOPIC_SUB);
  Serial.println("MQTT Connected...");
}

void loop() {
  //Testing connecting with onboard led
    /*
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on by making the voltage LOW
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(2000);  

  */
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
