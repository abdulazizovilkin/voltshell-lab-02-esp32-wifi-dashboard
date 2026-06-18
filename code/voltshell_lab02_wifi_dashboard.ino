#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SDA 21
#define OLED_SCL 22

#define LED_ARMED_PIN 23
#define LED_DANGER_PIN 19
#define BUZZER_PIN 25
#define BUTTON_PIN 27
#define POT_PIN 34

const char* apSSID = "VoltShell-Lab02";
const char* apPassword = "12345678";

WebServer server(80);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool oledOK = false;

bool systemArmed = false;
int dangerThreshold = 2500;

bool lastButtonReading = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long lastBuzzerTime = 0;
bool buzzerState = LOW;

unsigned long lastDisplayUpdate = 0;

String getStatusText(int potValue) {
  if (systemArmed == true && potValue > dangerThreshold) {
    return "DANGER";
  } else {
    return "NORMAL";
  }
}

String getSystemText() {
  if (systemArmed == true) {
    return "ARMED";
  } else {
    return "DISARMED";
  }
}

void handleRoot() {
  int potValue = analogRead(POT_PIN);
  int percent = map(potValue, 0, 4095, 0, 100);

  String statusText = getStatusText(potValue);
  String systemText = getSystemText();

  String html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<meta http-equiv='refresh' content='1'>";
  html += "<title>VoltShell Lab 02</title>";

  html += "<style>";
  html += "body { font-family: Arial; background: #111827; color: white; text-align: center; padding: 25px; }";
  html += ".card { background: #1f2937; padding: 20px; border-radius: 15px; max-width: 400px; margin: auto; }";
  html += "h1 { color: #38bdf8; }";
  html += ".value { font-size: 28px; font-weight: bold; }";
  html += ".normal { color: #22c55e; }";
  html += ".danger { color: #ef4444; }";
  html += "button { padding: 14px 25px; margin: 10px; border: none; border-radius: 10px; font-size: 18px; cursor: pointer; }";
  html += ".arm { background: #22c55e; color: white; }";
  html += ".disarm { background: #ef4444; color: white; }";
  html += "</style>";

  html += "</head>";
  html += "<body>";

  html += "<div class='card'>";
  html += "<h1>VoltShell Lab 02</h1>";
  html += "<h2>ESP32 Wi-Fi Dashboard</h2>";

  html += "<p>Sensor Level</p>";
  html += "<div class='value'>";
  html += String(percent);
  html += "%</div>";

  html += "<p>System</p>";
  html += "<div class='value'>";
  html += systemText;
  html += "</div>";

  html += "<p>Status</p>";

  if (statusText == "DANGER") {
    html += "<div class='value danger'>DANGER</div>";
  } else {
    html += "<div class='value normal'>NORMAL</div>";
  }

  html += "<br>";

  html += "<a href='/arm'><button class='arm'>ARM</button></a>";
  html += "<a href='/disarm'><button class='disarm'>DISARM</button></a>";

  html += "<p style='font-size:12px; color:#9ca3af;'>Physical button: GPIO27</p>";
  html += "<p style='font-size:12px; color:#9ca3af;'>Build in Public | VoltShell Labs</p>";

  html += "</div>";
  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}

void handleArm() {
  systemArmed = true;
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleDisarm() {
  systemArmed = false;
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_DANGER_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleButton() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        systemArmed = !systemArmed;

        if (systemArmed == false) {
          digitalWrite(BUZZER_PIN, LOW);
          digitalWrite(LED_DANGER_PIN, LOW);
          buzzerState = LOW;
        }
      }
    }
  }

  lastButtonReading = reading;
}

void updateOLED(int potValue) {
  if (oledOK == false) {
    return;
  }

  if (millis() - lastDisplayUpdate < 300) {
    return;
  }

  lastDisplayUpdate = millis();

  int percent = map(potValue, 0, 4095, 0, 100);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("VoltShell Lab 02");

  display.setCursor(0, 14);
  display.println("Wi-Fi Dashboard");

  display.setCursor(0, 28);
  display.print("Level: ");
  display.print(percent);
  display.println("%");

  display.setCursor(0, 42);
  display.print("System: ");

  if (systemArmed == true) {
    display.println("ARMED");
  } else {
    display.println("DISARMED");
  }

  display.setCursor(0, 55);
  display.print("Status: ");

  if (systemArmed == true && potValue > dangerThreshold) {
    display.println("DANGER");
  } else {
    display.println("NORMAL");
  }

  display.display();
}

void handleAlarmLogic(int potValue) {
  if (systemArmed == true) {
    digitalWrite(LED_ARMED_PIN, HIGH);
  } else {
    digitalWrite(LED_ARMED_PIN, LOW);
  }

  if (systemArmed == true && potValue > dangerThreshold) {
    digitalWrite(LED_DANGER_PIN, HIGH);

    if (millis() - lastBuzzerTime > 150) {
      lastBuzzerTime = millis();
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState);
    }

  } else {
    digitalWrite(LED_DANGER_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    buzzerState = LOW;
  }
}

void setup() {
  pinMode(LED_ARMED_PIN, OUTPUT);
  pinMode(LED_DANGER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_ARMED_PIN, LOW);
  digitalWrite(LED_DANGER_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.begin(115200);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    oledOK = true;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("VoltShell Lab 02");
    display.setCursor(0, 15);
    display.println("Starting Wi-Fi...");
    display.display();

  } else {
    oledOK = false;
  }

  WiFi.softAP(apSSID, apPassword);

  Serial.println();
  Serial.println("VoltShell Lab 02 Started");
  Serial.print("Wi-Fi Name: ");
  Serial.println(apSSID);
  Serial.print("Password: ");
  Serial.println(apPassword);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);

  server.begin();

  if (oledOK == true) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Wi-Fi Ready");
    display.setCursor(0, 15);
    display.println("VoltShell-Lab02");
    display.setCursor(0, 30);
    display.println("IP: 192.168.4.1");
    display.display();
  }

  delay(1500);
}

void loop() {
  server.handleClient();

  int potValue = analogRead(POT_PIN);

  handleButton();
  handleAlarmLogic(potValue);
  updateOLED(potValue);
}
