#define PWRKEY_PIN       4
#define MODEM_POWER_ON   23
#define MODEM_RST        5
#define RXD2             26  // ESP32 RX ← SIM800 TX
#define TXD2             27  // ESP32 TX → SIM800 RX


#define ledPin  13

#ifdef ESP8266
  #include <ESP8266WiFi.h>     /* WiFi library for ESP8266 */
#else
  #include <WiFi.h>            /* WiFi library for ESP32 */
#endif
#include <Wire.h>
#include <PubSubClient.h>
#include "DHT.h"             /* DHT11 sensor library */

#define DHTPIN 2
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define wifi_ssid "Redmi Note 11 Gaied"
#define wifi_password "123456789a"
#define mqtt_server "192.168.13.66"

#define humidity_topic "sensor/DHT11/humidity"
#define temperature_celsius_topic "sensor/DHT11/temperature_celsius"
#define temperature_fahrenheit_topic "sensor/DHT11/temperature_fahrenheit"

#define light_topic "led"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  // dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);


digitalWrite(ledPin, HIGH);
delay(2000);

 pinMode(ledPin, OUTPUT);
    pinMode(PWRKEY_PIN, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);

  // Allumer le modem SIM800
  digitalWrite(MODEM_POWER_ON, HIGH);
  digitalWrite(MODEM_RST, HIGH);
  delay(100);

  // Impulsion sur PWRKEY pour allumer le SIM800
  digitalWrite(PWRKEY_PIN, LOW);
  delay(1000);  // Maintenir LOW 1 seconde
  digitalWrite(PWRKEY_PIN, HIGH);
  delay(3000);  // Laisser le module démarrer

Serial.println("Envoi de la commande AT...");
  sendCommand("AT");


  delay(5000);
  sendCommand("AT+CPIN?"); 
    delay(1000); 
  sendCommand("AT+CREG?"); 
    delay(1000); 
  sendCommand("AT+CSQ"); 
    delay(1000); 
  sendCommand("AT+COPS?");
    delay(1000);  
 // sendCommand("AT+GSN");
// delay(10000);
  
  // Vérifie si le module répond
   Serial1.println("AT+CMGF=1");                // Mode SMS texte
    delay(1000); 
   Serial1.println("AT+CSCS=\"GSM\"");          // Encodage GSM classique
    delay(1000); 
   Serial1.println("AT+CMGS=\"+21626567312\""); // Numéro du destinataire
  delay(5000);
  Serial1.print("Bonjour ! GSM SMS.");  // Le message à envoyer
  delay(500);
  Serial1.write(0x1A);

  

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected£
   updateSerial(); 
  while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");

  if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void loop() {

      if (!client.connected()) {
        reconnect();
      }
      client.loop();

      // Wait a few seconds between measurements.
      delay(2000);

      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float f = dht.readTemperature(true);

      // Check if any reads failed and exit early (to try again).
     /* if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
      }*/

      // Compute heat index in Fahrenheit (the default)
      float hif = dht.computeHeatIndex(f, h);
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");
      Serial.print(f);
      Serial.print(" *F\t");
      Serial.print("Heat index: ");
      Serial.print(hic);
      Serial.print(" *C ");
      Serial.print(hif);
      Serial.println(" *F");

      Serial.print("Temperature in Celsius:");
      Serial.println(String(t).c_str());
      client.publish(temperature_celsius_topic, String(t).c_str(), true);

      Serial.print("Temperature in Fahrenheit:");
      Serial.println(String(f).c_str());
      client.publish(temperature_fahrenheit_topic, String(f).c_str(), true);

      Serial.print("Humidity:");
      Serial.println(String(h).c_str());
      client.publish(humidity_topic, String(h).c_str(), true);

      if (client.connect("ESP32Client")) {
  Serial.println("Connecté au broker MQTT");
  client.subscribe("led");  // S'abonner ici, une fois connecté



  if(t>28)
  { Serial1.println("AT+CMGS=\"+21626567312\""); // Numéro du destinataire
  delay(5000);
  Serial1.print("Bonjour ! TEMPRATURE ELEVE.");  // Le message à envoyer
  delay(500);
  Serial1.write(0x1A);
  
  Serial.println("Appel en cours...");
  Serial1.println("ATD+21626484641;"); // Ex: ATD+33612345678;
  delay(10000); // Durée de l'appel (20 sec)
  Serial1.println("ATH"); // Raccrocher
  Serial.println("Appel termine.");
  }
}


}



void mqttCallback(char* topic, byte* message, unsigned int len) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.println(". Message: ");
  String incomming_message;

  for (int i = 0; i < len; i++) {
    incomming_message += (char)message[i];
  }
  incomming_message.trim();
  Serial.println(incomming_message);
  if (incomming_message == "ON") {
    Serial.println("Turing On Built-in LED");
    digitalWrite(ledPin, HIGH);
  }
  if (incomming_message == "OFF") {
    Serial.println("Turing Off Built-in LED");
    digitalWrite(ledPin, LOW);
  }
  Serial.println();
}



void updateSerial() {
  while (Serial.available()) {
    Serial1.write(Serial.read());
  }

  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
}


void sendCommand(String cmd) {
 
  Serial.println(cmd);
  Serial1.println(cmd);
  delay(1000);
  while (Serial1.available()) {
    char c = Serial1.read();
    Serial.write(c);
  }
  Serial.println();
}
