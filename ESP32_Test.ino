// -------- BLYNK CONFIG --------
#define BLYNK_TEMPLATE_ID "TMPL3lf5JNSrv"
#define BLYNK_TEMPLATE_NAME "Rail Monitor"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// -------- WIFI --------
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// -------- PINS --------
#define ONE_WIRE_BUS 4
#define DHTPIN 5
#define DHTTYPE DHT22
#define BUZZER 18

// -------- OBJECTS --------
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature railSensor(&oneWire);
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;

// -------- FUNCTION --------
void sendData() {

  railSensor.requestTemperatures();
  float railTemp = railSensor.getTempCByIndex(0);

  float airTemp = dht.readTemperature();
  float humidity = dht.readHumidity();

  // SERIAL
  Serial.print("Rail Temp: ");
  Serial.print(railTemp);
  Serial.print(" C | Air Temp: ");
  Serial.print(airTemp);
  Serial.print(" C | Humidity: ");
  Serial.println(humidity);

  // LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Rail:");
  lcd.print(railTemp);
  lcd.print("C");

  lcd.setCursor(0,1);
  lcd.print("Hum:");
  lcd.print(humidity);
  lcd.print("%");

  // BUZZER + ALERT
  if (railTemp > 40 || railTemp < 5) {
    digitalWrite(BUZZER, HIGH);

    Blynk.logEvent("temp_alert", "Rail Temperature Alert!");
  } else {
    digitalWrite(BUZZER, LOW);
  }

  // SEND TO BLYNK
  Blynk.virtualWrite(V0, railTemp);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, airTemp);
}

void setup() {

  Serial.begin(115200);

  pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  railSensor.begin();
  dht.begin();

  lcd.init();
  lcd.backlight();

  lcd.print("Starting...");
  delay(2000);
  lcd.clear();

  // CONNECT TO WIFI + BLYNK
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(3000L, sendData);
}

void loop() {
  Blynk.run();
  timer.run();
}