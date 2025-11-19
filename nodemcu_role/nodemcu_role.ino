#define LED_BUILTIN 4
char lettre ; 
void setup() {
  
  Serial.begin(4800); // Console
  Serial2.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
  Serial.println("UART prêt à recevoir...");

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial2.available()) {
    
    String lettre = Serial2.readStringUntil('\n'); // Attendre la fin de ligne
    if (lettre == 't'){
    Serial.print("Trame reçue: ");
    Serial.println(lettre);
    digitalWrite(LED_BUILTIN, LOW);  // LED allumée (inversée sur NodeMCU)
    delay(1000);                     // Attendre 1 seconde
    digitalWrite(LED_BUILTIN, HIGH); // LED éteinte
    delay(1000);
    }
  }
}
