# メールボックス通知スケッチ

このリポジトリは、**Wio BG770A** ボード用の Arduino スケッチの例を提供します。
このスケッチは超音波距離センサーを使用してメールボックスに新しいメールが到着したかを検出し
Discord チャンネルにウェブフック経由で通知を送信します。
LTE 経由で送信されます。バッテリー駆動を想定しており、測定の間隔で低消費電力モードに移行することで
電力消費を最小限に抑えるように設計されています。

## ファイル

- `mailbox_notifier.ino` – コンパイル可能な完成済みのスケッチ。使用前に
`WEBHOOK_PATH`定数をDiscordのウェブフックIDとトークンで置き換えてください。

## 使用方法

1. 超音波センサー（例：HC-SR04）をスケッチで指定されたピン（`PIN_TRIG`と`PIN_ECHO`）に接続します。
2. スケッチをWio BG770Aにフラッシュします。
2. スケッチをWio BG770Aにフラッシュします。
3. `emptyDistance`と`mailThreshold`を郵便受けのサイズに合わせて調整します。
4. ボードにバッテリーを接続し、KDDI SIMが挿入されていることを確認します。

郵便物を検出すると、デバイスはLTE接続をアクティブ化し、
Discordにメッセージを投稿し、その後低消費電力モードに戻ります。


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
