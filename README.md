# Mailbox Notification Sketch

This repository provides an example Arduino sketch for the **Wio BG770A** board.
The sketch uses an ultrasonic distance sensor to detect when new mail has arrived
in a mailbox and sends a notification to a Discord channel using a webhook over
LTE. It is designed for battery-powered operation and minimizes power usage by
entering a low-power mode between measurements.

## File

- `mailbox_notifier.ino` – Complete sketch ready for compilation. Update the
  `WEBHOOK_PATH` constant with your Discord webhook ID and token before use.

## Usage

1. Wire an ultrasonic sensor (e.g., HC‑SR04) to the board using the pins
   specified in the sketch (`PIN_TRIG` and `PIN_ECHO`).
2. Flash the sketch to the Wio BG770A.
3. Adjust `emptyDistance` and `mailThreshold` according to your mailbox size.
4. Power the board with a battery and ensure the KDDI SIM is inserted.

Upon detecting mail, the device activates its LTE connection, posts a message to
Discord, and then returns to low-power mode.
