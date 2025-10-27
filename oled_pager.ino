#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ThingerESP8266.h>
#include <NTPClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1

#define USERNAME "Gauth_xo"
#define DEVICE_ID "ESP-FUN"
#define DEVICE_CREDENTIAL "pz8FRRS5DHu7WcZn"
#define WIFI_SSID "S20fe"
#define WIFI_PASSWORD "admin123"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
String message = "Welcome!";

// NTP client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // Offset for IST (UTC+5:30) in seconds

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();

  // Initialize WiFi and Thinger
  thing.add_wifi(WIFI_SSID, WIFI_PASSWORD);

  // Initialize NTP client
  timeClient.begin();

  // Thinger.io message resource
  thing["message"] << [](pson &in) {
    if (in.is_empty()) {
      in = (const char*) message.c_str();
    } else {
      message = (const char*) in;
    }
  };
}

void updateDisplay() {
  display.clearDisplay();

  // Update time from NTP
  timeClient.update();

  // Get the 24-hour formatted time
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();

  // Convert to 12-hour format
  String period = (hours >= 12) ? "PM" : "AM";
  hours = hours % 12;
  hours = (hours == 0) ? 12 : hours; // Adjust 0 hour to 12 for 12-hour format

  // Format the time string (HH:MM AM/PM)
  String timeStr = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + " " + period;

  // Set the size and color for time
  display.setTextSize(3); // Use a larger size for better visibility
  display.setTextColor(SSD1306_WHITE);

  // Calculate the position for centering the time
  int16_t x1, y1;
  uint16_t width, height;
  display.getTextBounds(timeStr, 0, 0, &x1, &y1, &width, &height);
  int16_t xCenter = (SCREEN_WIDTH - width) / 2;

  // Draw the time text multiple times for bolder effect
  for (int i = -1; i <= 1; i++) {
    display.setCursor(xCenter + i, 0);
    display.print(timeStr);
  }

  // Adjust message position further down towards the bottom
  display.setTextSize(1);
  display.setCursor(1, SCREEN_HEIGHT - 8); // Positioned 8 pixels from the bottom
  display.print(message);

  display.display();
}


void loop() {
  thing.handle();
  updateDisplay();
  delay(1000); // Refresh display every second
}