# ESP32 Gateway: Plant to MQTT

### MQTT firmware für lilygo higrow v1.1

Lorem Ipsum ...ToDo write text:
- firmware for lilygo higrow v1.1 (change pins for other versions)
- mqtt publish {temperature, humidity, soil watering, battery} every 20 minutes
- optimized for low battery consumption (should wake up ~5s * 3 every hour)

![mint monitoring](https://github.com/schneebonus/ESP32-Plant-To-MQTT-Gateway/blob/main/mint.jpeg?raw=true)

### Requirenments
- DHT.h https://github.com/adafruit/DHT-sensor-library
- WiFi.h
- PubSubClient.h https://pubsubclient.knolleary.net/
- ArduinoJson.h https://arduinojson.org/?utm_source=meta&utm_medium=library.properties

### Install

- Download plant_mqtt.ino
- Set wifi settings (ssid and passsword)
- Set mqtt settings (mqttServer, mqttPort, mqttUser, mqttPassword)
- Deploy to your esp32 / lilygo / ...

### Node-RED and InfluxDB integration

- Install and setup default MQTT Node
- Install and setup node-red-contrib-stackhero-influxdb-v2 ( https://flows.nodered.org/node/node-red-contrib-stackhero-influxdb-v2 )
- Create custum function with content of mqtt_to_influxdb.js :
![nodered_mqtt_influxdv](https://github.com/schneebonus/ESP32-Plant-To-MQTT-Gateway/blob/main/nodered-plants.png?raw=true)

### Further Improvements (maybe... maybe not):

- MQTT over TLS ( + verify certificate fingerprint )

### Credits
- https://github.com/BeardedTinker/LilyGO-TTGO-HiGrow for a lot of inspiration and copied code
