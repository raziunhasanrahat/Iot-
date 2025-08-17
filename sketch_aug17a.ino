#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define SOIL_PIN A0
#define FLAME_PIN D5
#define GAS_PIN D6
#define BUZZER_PIN D7

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";
#define BOT_TOKEN "YOUR_BOT_TOKEN"
#define CHAT_ID "YOUR_CHAT_ID"

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void setup() {
  pinMode(FLAME_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  secured_client.setInsecure();
}

void beepBuzzer(unsigned long duration) {
  unsigned long start = millis();
  while (millis() - start < duration) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

void loop() {
  int newMsgCount = bot.getUpdates(bot.last_message_received + 1);

  int soilVal = analogRead(SOIL_PIN);
  int flameVal = digitalRead(FLAME_PIN);
  int gasVal = digitalRead(GAS_PIN);

  if (flameVal == LOW) {
    bot.sendMessage(CHAT_ID, "🔥 Flame detected!");
    beepBuzzer(5000);
  }

  if (gasVal == LOW) {
    bot.sendMessage(CHAT_ID, "💨 Gas detected!");
    beepBuzzer(5000);
  }

  for (int i = 0; i < newMsgCount; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    int soilPercent = map(soilVal, 1023, 0, 0, 100);
    String soilStatus = (soilPercent > 30) ? "✅ Wet 💧" : "❌ Dry 🌵";
    String flameStatus = (flameVal == LOW) ? "❌ Bad 🔥" : "✅ Good 🔥";
    String gasStatus = (gasVal == LOW) ? "❌ Bad 💨" : "✅ Good 💨";

    if (text.equalsIgnoreCase("Soil")) {
      bot.sendMessage(chat_id, "🌱 Soil: " + String(soilPercent) + "% (" + soilStatus + ")");
    }

    if (text.equalsIgnoreCase("Condition")) {
      String status = "📋 Condition Report:\n";
      status += "🌱 Soil: " + soilStatus + "\n";
      status += "🔥 Flame: " + flameStatus + "\n";
      status += "💨 Gas: " + gasStatus;
      bot.sendMessage(chat_id, status);
    }

    if (text.equalsIgnoreCase("All")) {
      String status = "📊 Sensor Report:\n";
      status += "🌱 Soil: " + String(soilPercent) + "% (" + soilStatus + ")\n";
      status += "🔥 Flame: " + flameStatus + "\n";
      status += "💨 Gas: " + gasStatus;
      bot.sendMessage(chat_id, status);
    }
  }
}