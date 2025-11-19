#define PWRKEY_PIN       4
#define MODEM_POWER_ON   23
#define MODEM_RST        5
#define RXD2             26  // ESP32 RX ← SIM800 TX
#define TXD2             27  // ESP32 TX → SIM800 RX

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Initialisation SIM800...");

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
  sendCommand("AT+GSN");
  delay(10000);
  Serial.println("Appel en cours...");
  Serial1.println("ATD+21626484641;"); // Ex: ATD+33612345678;
  delay(20000); // Durée de l'appel (20 sec)
  Serial1.println("ATH"); // Raccrocher
  Serial.println("Appel termine.");
}

void loop() {
  updateSerial();  // Pont série entre PC ↔ SIM800
 
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
