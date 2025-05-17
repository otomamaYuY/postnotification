// Mailbox notification sketch for Wio BG770A

#include <Arduino.h>
#include <WioCellular.h>
#include <ArduinoHttpClient.h>

// Network configuration based on the official example
#define SEARCH_ACCESS_TECHNOLOGY (WioCellularNetwork::SearchAccessTechnology::LTEM)
#define LTEM_BAND (WioCellularNetwork::NTTDOCOMO_LTEM_BAND)
static const char APN[] = "soracom.io";

static constexpr int POWER_ON_TIMEOUT = 20000;     // [ms]
static constexpr int NETWORK_TIMEOUT  = 120000;    // [ms]

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
bool connectCellular();
void disconnectCellular();

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  // Initialize sensor pins
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // Initialize cellular module
  WioCellular.begin();
  WioNetwork.config.searchAccessTechnology = SEARCH_ACCESS_TECHNOLOGY;
  WioNetwork.config.ltemBand = LTEM_BAND;
  WioNetwork.config.apn = APN;
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
  if (!connectCellular()) {
    return;
  }

  HttpClient client = HttpClient(WioCellular, WEBHOOK_HOST, 443);
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
  disconnectCellular();
}

void enterLowPower(unsigned long ms) {
  Wio.setPowerSavingMode(true);
  delay(ms);
  Wio.setPowerSavingMode(false);
}

bool connectCellular() {
  Serial.println("Powering on cellular module...");
  if (WioCellular.powerOn(POWER_ON_TIMEOUT) != WioCellularResult::Ok) {
    Serial.println("Failed to power on module.");
    return false;
  }

  WioNetwork.begin();
  if (!WioNetwork.waitUntilCommunicationAvailable(NETWORK_TIMEOUT)) {
    Serial.println("Network connection failed.");
    return false;
  }

  Serial.println("Network connected.");
  return true;
}

void disconnectCellular() {
  WioNetwork.end();
  if (WioCellular.powerOff() != WioCellularResult::Ok) {
    Serial.println("Failed to power off module.");
  } else {
    Serial.println("Cellular module powered off.");
  }
}
