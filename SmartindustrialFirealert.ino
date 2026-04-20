#include <SoftwareSerial.h>
#include <DHT11.h>

// Pin definitions
#define DHTPIN 2
#define DHTTYPE DHT11
#define FLAME_SENSOR 3
#define GAS_SENSOR A0
#define RELAY_PIN 4
#define BUZZER_PIN 5

// GSM module pins
#define RX 7
#define TX 8

SoftwareSerial sim800l(RX, TX);
DHT11 dht11(DHTPIN);

String managerNumber = "+91XXXXXXXXXX"; // Replace with manager's phone number
bool systemActive = true;

void setup() {
  Serial.begin(9600);
  sim800l.begin(9600);
  

  pinMode(FLAME_SENSOR, INPUT);
  pinMode(GAS_SENSOR, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // System ON
  digitalWrite(BUZZER_PIN, LOW);

  delay(1000);
  sendSMS(managerNumber, "System Started. Monitoring hazards...");
}

void loop() {
  if (systemActive) {
    // Read sensors
    float temp = dht11.readTemperature();
    int flame = digitalRead(FLAME_SENSOR);
    int gasValue = analogRead(GAS_SENSOR);

    // Hazard detection
    if (flame == LOW) { // Flame detected
      triggerAlert("FIRE detected!");
    }
    if (gasValue > 400) { // Adjust threshold
      triggerAlert("GAS leak detected!");
    }
    if (temp > 50) { // Adjust threshold
      triggerAlert("High Temperature detected!");
    }
  }

  // Check for incoming SMS
  if (sim800l.available()) {
    String sms = sim800l.readString();
    sms.toUpperCase();
    if (sms.indexOf("OFF") != -1) {
      systemActive = false;
      digitalWrite(RELAY_PIN, LOW); // Shut down system
      sendSMS(managerNumber, "System shut down by manager.");
    }
    if (sms.indexOf("ON") != -1) {
      systemActive = true;
      digitalWrite(RELAY_PIN, HIGH); // Restart system
      sendSMS(managerNumber, "System restarted by manager.");
    }
  }
}

// Function to send SMS
void sendSMS(String number, String message) {
  sim800l.println("AT+CMGF=1"); // Text mode
  delay(1000);
  sim800l.print("AT+CMGS=\"");
  sim800l.print(number);
  sim800l.println("\"");
  delay(1000);
  sim800l.println(message);
  delay(100);
  sim800l.write(26); // Ctrl+Z to send
  delay(5000);
}

// Function to trigger alert
void triggerAlert(String alertMsg) {
  digitalWrite(BUZZER_PIN, HIGH);
  sendSMS(managerNumber, alertMsg);
  delay(5000);
  digitalWrite(BUZZER_PIN, LOW);
}

