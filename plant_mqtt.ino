#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


#define DEBUGGING false

#define MQTT_PUB "plants/lilygo"

// Aktuell:
// 1 Minze
// 2 Schnittlauch
// 3 Oregano
// 4 Thymian
#define DEVICE_ID 1

const char* ssid = "";          # ToDo
const char* password = "";      # ToDo
const char* mqttServer = "";    # ToDo
const int mqttPort = 1883;      # ToDo
const char* mqttUser = "";      # ToDo
const char* mqttPassword = "";  # ToDo

#define uS_TO_S_FACTOR  1000000
#define TIME_TO_SLEEP   20 * 60

WiFiClient espClient;
PubSubClient client(espClient);

#define I2C_SDA             25
#define I2C_SCL             26
#define DHT_PIN             16
#define BAT_ADC             33
#define SALT_PIN            34
#define SOIL_PIN            32
#define BOOT_PIN            0
#define POWER_CTRL          4
#define USER_BUTTON         35

#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  // fire everything up
  if(DEBUGGING)Serial.begin(115200);
  connect_wifi();               // wlan verbinden
  connect_mqtt();               // mqtt verbinden
  enable_sensors();             // sensoren mit Strom versorgen

  // do the job
  send_data();                  // Sensoren auslesen und über mqtt raus senden

  // shut everything down
  disable_sensors();            // Sensoren vom Strom trennen
  client.disconnect();          // mqtt abmelden
  WiFi.disconnect(true);        // wlan trennen
  WiFi.mode(WIFI_OFF);          // wlan vom Strom trennen

  if(DEBUGGING){
    unsigned long myTime = millis();
    Serial.print("Required time: ");
    Serial.print(myTime);
    Serial.println("ms");
    }

  // deep sleep
  if(DEBUGGING)Serial.print("Deep sleep");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);  // Wecker stellen
  esp_deep_sleep_start();       // schlafen gehen und Strom sparen
}

void loop() {
}

void send_data() {
  StaticJsonDocument<200> doc;

  // Sensoren lesen
  uint16_t soil = readSoil();
  float bat = readBattery();
  float h = int(dht.readHumidity() * 10.0);
  float t = int(dht.readTemperature() * 10.0);

  // JSON Objekt erstellen
  doc["t"] = t;
  doc["h"] = h;
  doc["b"] = bat;
  doc["s"] = soil;

  // JSON zu String umwandeln
  String payload;
  serializeJson(doc, payload);
  
  // Über MQTT raus senden
  String topic = MQTT_PUB + String(DEVICE_ID);
  client.publish(topic.c_str(), payload.c_str());
  if(DEBUGGING)Serial.print("Topic: ");
  if(DEBUGGING)Serial.println(topic);
  if(DEBUGGING)Serial.print("Data: ");
  if(DEBUGGING)Serial.println(payload);

  // warten damit der Deep Sleep das asynchrone Senden nicht abwürgt
  delay(1500);
}

void enable_sensors() {
  pinMode(POWER_CTRL, OUTPUT);
  digitalWrite(POWER_CTRL, 1);
  dht.begin();
  delay(500);
}

void disable_sensors() {
  pinMode(POWER_CTRL, OUTPUT);
  digitalWrite(POWER_CTRL, 0);
}

void connect_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
  }
  if(DEBUGGING)Serial.println("Connected to the WiFi network");
}

void connect_mqtt() {
  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      if(DEBUGGING)Serial.println("Connected to MQTT");
    } else {
      if(DEBUGGING)Serial.print("failed with state ");
      if(DEBUGGING)Serial.print(client.state());
      delay(2000);
    }
  }
}

float readBattery()
{
  int vref = 1100;
  uint16_t volt = analogRead(BAT_ADC);
  float battery_voltage = ((float)volt / 4095.0) * 2.0 * 3.3 * (vref) / 1000;
  battery_voltage = battery_voltage * 100;
  //int battery_voltage_int = int(battery_voltage);
  return (map(battery_voltage, 409, 300, 10000, 0)) / 100;
}

uint16_t readSoil()
// It is a really good thing to calibrate each unit for soil, first note the number when unit is on the table, the soil number is for zero humidity. Then place the unit up to the electronics into a glass of water, the number now is the 100% humidity.
// By doing this you will get the same readout for each unit. Replace the value below for the dry condition, and the 100% humid condition, and you are done.
{
  uint16_t soil = analogRead(SOIL_PIN);
  // ToDo: Calibration for new devices
  if(DEVICE_ID == 1)return map(soil, 1322, 3178, 100, 0); // Lilygo1
  if(DEVICE_ID == 2)return map(soil, 1302, 3027, 100, 0); // Lilygo2
  if(DEVICE_ID == 3)return map(soil, 1550, 3194, 100, 0); // Lilygo3
  if(DEVICE_ID == 4)return map(soil, 1622, 3330, 100, 0); // Lilygo4
  return map(soil, 1322, 3178, 100, 0); // default fallback
}
