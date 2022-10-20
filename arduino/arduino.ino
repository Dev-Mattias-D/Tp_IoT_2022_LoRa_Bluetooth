#include <MQTT.h>
#include <MQTTClient.h>
#include <WiFi.h>

const char ssid[] = "Ma";
const char pass[] = "jlmjlmjlm";

const char ap[] = "videoprojecteur";
const char mosquito[] = "test.mosquitto.org";

WiFiClient net;
MQTTClient client;
unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("."); delay(1000);
  }
  Serial.print("\nconnecting...");
  while (!client.connect("thierr54545yf")) {
    Serial.print("."); delay(1000);
  }
  Serial.println("\nconnected!");
  
  client.subscribe("/videoprojecteur");
}

void messageReceived(String &topic, String &ip) {
  Serial.println("incoming: " + topic + " - " + ip);
  delay(1000);

  //Envoi OK serveur
  client.publish("/videoprojecteur", "ok");
  client.unsubscribe(ap);
  delay(1000);

  //Disconnect AP
  WiFi.disconnect();
  delay(1000);

  //Connexion AP
  WiFi.begin(ap,"");
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("."); delay(1000);
  }
  Serial.println("\nconnected to new ap!");
  delay(1000);

  int str_len = ip.length() + 1; 
  char ipChar[str_len];
  ip.toCharArray(ipChar, str_len);
  
  //HTTP
  if (net.connect(ipChar, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    net.println("GET / HTTP/1.1");
    net.println("Host: " + ip);
    net.println("Connection: close");
    net.println();
  }
  while (net.available()) {
    char c = net.read();
    Serial.write(c);
  }

  //Ecouter LoRa
  
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid,pass);
  client.begin(mosquito, net);
  client.onMessage(messageReceived);
  connect();
}

void loop() {
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability
  
}
