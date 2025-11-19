#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <PubSubClient.h>


#define LED_PIN          13
#define MODEM_RST        5
#define MODEM_PWRKEY     4
#define MODEM_POWER_ON   23
#define MODEM_TX         27
#define MODEM_RX         26
#define BAUD_RATE        9600

// Réseau GSM
const char apn[]  = "internet";
//web.ooredoo.tn
//internet
const char user[] = "";
const char pass[] = "";

// Broker MQTT
const char* broker = "broker.hivemq.com";
//test.mosquitto.org
//broker.hivemq.com
const int   port = 1883;
const char* topic1 = "test/esp32";
const char* topic2 = "lightss";

// Modem
TinyGsm modem(Serial1);
TinyGsmClient gsmClient(modem);
PubSubClient mqtt(gsmClient);

bool messageEnvoye = false;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialisation des broches d'alimentation
  pinMode(MODEM_PWRKEY, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);

  digitalWrite(MODEM_POWER_ON, HIGH);
  digitalWrite(MODEM_RST, HIGH);
  delay(100);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1000);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(3000);

  // Lancer la communication série avec le modem
  Serial1.begin(BAUD_RATE, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  Serial.println(" Démarrage du modem...");
  modem.restart();

  Serial.println("Connexion au réseau...");
  if (!modem.waitForNetwork()) {
    Serial.println("❌ Réseau GSM non disponible");
    return;
  }
  Serial.print("Signal = ");
  Serial.println(modem.getSignalQuality());
  Serial.println(" Connexion GPRS...");
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" GPRS non connecté");
    return;
  }
  Serial.println(" GPRS connecté");

  // Configurer le serveur MQTT
  mqtt.setServer(broker, port);
  mqtt.setCallback(mqttCallback); 

  Serial.println(" Connexion au broker MQTT...");
  if (mqtt.connect("ESP32Client")) {
    Serial.println(" MQTT connectée !");
    delay(1000);
    
    bool success = mqtt.publish(topic1, "MQTT");
  Serial.println(success ? "Message envoyé !" : "Échec de l'envoi !");

    Serial.println(" Message envoyé !");
    messageEnvoye = true;
    mqtt.subscribe(topic2,1);

    
  } else {
    Serial.print(" Échec MQTT. Code = ");
    Serial.println(mqtt.state());
  }
}

void loop() {
   mqtt.loop();  // ← Obligatoire pour gérer MQTT


}


// Fonction appelée à chaque réception d’un message MQTT
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print(" Message reçu sur topic : ");
  Serial.println(topic);

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim(); // Supprime les espaces/retours à la ligne

  Serial.print(" Contenu : ");
  Serial.println(message);

  if (String(topic) == topic2) {
    if (message == "ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("? LED allumée");
    } else if (message == "OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println(" LED éteinte");
    }
  }
}
