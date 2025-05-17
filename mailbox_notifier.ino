// Mailbox notification sketch for Wio BG770A

#include <Arduino.h>
#include <WioCellular.h>
#include <ArduinoHttpClient.h>


// Pin definitions for ultrasonic sensor
const int PIN_TRIG = 3; // Example pin for TRIG
const int PIN_ECHO = 2; // Example pin for ECHO

// Discord webhook configuration
const char WEBHOOK_HOST[] = "discord.com";
const char WEBHOOK_PATH[] = "/api/webhooks/REPLACE_WITH_ID/REPLACE_WITH_TOKEN";

// Mailbox detection parameters
float emptyDistance = 30.0; // Baseline distance (cm) when mailbox is empty
const float mailThreshold = 5.0; // Difference that indicates new mail (cm)
const int measurements = 3; // Number of samples for averaging

// Sleep interval in milliseconds between checks
const unsigned long sleepInterval = 300000; // 5 minutes

WioCellular Wio;

// Forward declarations
float readDistance();
void sendDiscordNotification(const String &message);
void enterLowPower(unsigned long ms);


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  // Initialize sensor pins
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);


}

void loop() {
  // Read averaged distance
  float distance = 0;
  for (int i = 0; i < measurements; i++) {
    distance += readDistance();
    delay(50);
  }
  distance /= measurements;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Detect mail arrival
  if (distance < emptyDistance - mailThreshold) {
    sendDiscordNotification("Mail has arrived in the mailbox!");
    // Wait longer after notifying to avoid spamming
    enterLowPower(30UL * 60UL * 1000UL); // 30 minutes
  } else {
    enterLowPower(sleepInterval);
  }
}

float readDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  unsigned long duration = pulseIn(PIN_ECHO, HIGH, 30000); // timeout after 30 ms
  float distance = duration / 58.0; // convert to cm
  return distance;
}

void sendDiscordNotification(const String &message) {
  String payload = String("{\"content\":\"") + message + "\"}";

  client.beginRequest();
  client.post(WEBHOOK_PATH);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", payload.length());
  client.endRequest();
  client.print(payload);

  int status = client.responseStatusCode();
  Serial.print("HTTP status: ");
  Serial.println(status);

  client.stop();

}

void enterLowPower(unsigned long ms) {
  Wio.setPowerSavingMode(true);
  delay(ms);
  Wio.setPowerSavingMode(false);
}
