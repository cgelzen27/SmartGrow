#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

/* WiFi configuration */
#ifndef APSSID
#define APSSID "ESP_AccessPoint"
#define APPSK "plantsarecool"
#endif

#define tx 14  //GPIO 14(D5)
#define rx 12  //GPIO 12(D6)

/* Commands for requesting sensor data from Arduino via Serial*/
enum COMMANDS {
  TEMPERATURE = 1,
  HUMIDITY,
  PHOTOSENSOR,
  WATER_LVL,
  SOIL_MOISTURE,
  PUMP_STATE,
};

enum REPORT_MODE : uint8_t {
  PERIODICAL,
  ON_DEMAND
};

typedef struct {
  const char *type;
  byte *data;
} Sensor;

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

SoftwareSerial mySerial(rx, tx);
AsyncWebServer server(80);
Sensor *temp, *humid, *light, *water, *soil, *pump;
JsonDocument doc;

void setup() {
  sensorInit();
  Serial.begin(115200);
  mySerial.begin(57600);
  Serial.println();

  /* Configure Access point */
  Serial.println("Configuring access point...");

  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  /* Configure server routes for accessing data */
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, TEMPERATURE, false);
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, HUMIDITY, false);
  });
  server.on("/photosensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, PHOTOSENSOR, false);
  });
  server.on("/water-level", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, WATER_LVL, false);
  });
  server.on("/soil-moisture", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, SOIL_MOISTURE, false);
  });
  server.on("/pump-state", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, PUMP_STATE, false);
  });

  server.on("/temperature-onDemand", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, TEMPERATURE, true);
  });
  server.on("/humidity-onDemand", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, HUMIDITY, true);
  });
  server.on("/photosensor-onDemand", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, PHOTOSENSOR, true);
  });
  server.on("/water-level-onDemand", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, WATER_LVL, true);
  });
  server.on("/soil-moisture-onDemand", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, SOIL_MOISTURE, true);
  });
  server.on("/pump-state-onDemand", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, PUMP_STATE, true);
  });
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
}

/* helper function for initiating a command to the periperhal hub(Arduino) to request sensor data then sending it over httpGet request */
void doServerRequest(AsyncWebServerRequest *request, COMMANDS cmd, bool onDemand) {

  String dataToSend;

  // Process the command
  switch (cmd) {
    case TEMPERATURE:
      {
        static float tempVal;

        // Send TEMPERATURE command to arduino
        mySerial.write(TEMPERATURE);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&tempVal, sizeof(tempVal));
        temp->data = (byte *)&tempVal;

        doc["sensor"] = temp->type;
        doc["data"] = *(float *)(temp->data);

        serializeJson(doc, dataToSend);
        //doc["mode"] =
        // // Convert float value to a string
        // int len = snprintf(NULL, 0, "%f", tempVal) + 1;
        // dataToSend = (char *)malloc(len);
        // snprintf(dataToSend, len, "%.2f", tempVal);

        break;
      }
    case HUMIDITY:
      {
        static float humVal;

        // Send HUMIDITY command to arduino
        mySerial.write(HUMIDITY);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&humVal, sizeof(humVal));
        humid->data = (byte *)&humVal;

        doc["sensor"] = humid->type;
        doc["data"] = *(float *)(humid->data);
        serializeJson(doc, dataToSend);

        // // Convert float value to a string
        // int len = snprintf(NULL, 0, "%f", humVal) + 1;
        // dataToSend = (char *)malloc(len);
        // snprintf(dataToSend, len, "%.2f", humVal);

        break;
      }
    case PHOTOSENSOR:
      {
        static int16_t lightVal;

        // Send WATER_LVL command to arduino
        mySerial.write(PHOTOSENSOR);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&lightVal, sizeof(lightVal));
        light->data = (byte *)&lightVal;

        doc["sensor"] = light->type;
        doc["data"] = *(int16_t *)(light->data);
        serializeJson(doc, dataToSend);
        // Convert int value to a string
        // int len = snprintf(NULL, 0, "%d", lightVal) + 1;
        // dataToSend = (char *)malloc(len);
        // snprintf(dataToSend, len, "%d", lightVal);

        break;
      }
    case WATER_LVL:
      {
        static int16_t waterVal;

        // Send WATER_LVL command to arduino
        mySerial.write(WATER_LVL);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&waterVal, sizeof(waterVal));
        water->data = (byte *)&waterVal;

        doc["sensor"] = water->type;
        doc["data"] = *(int16_t *)(water->data);
        serializeJson(doc, dataToSend);
        // Convert int value to a string
        // int len = snprintf(NULL, 0, "%d", waterVal) + 1;
        // dataToSend = (char *)malloc(len);
        // snprintf(dataToSend, len, "%d", waterVal);

        break;
      }
    case SOIL_MOISTURE:
      {
        static int16_t soilVal;

        // Send SOIL_MOISTURE command to arduino
        mySerial.write(SOIL_MOISTURE);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&soilVal, sizeof(soilVal));
        soil->data = (byte *)&soilVal;

        doc["sensor"] = soil->type;
        doc["data"] = *(int16_t *)(soil->data);
        serializeJson(doc, dataToSend);
        // Convert int value to a string
        // int len = snprintf(NULL, 0, "%d", soilVal) + 1;
        // dataToSend = (char *)malloc(len);
        // snprintf(dataToSend, len, "%d", soilVal);

        break;
      }
    case PUMP_STATE:
      {
        static bool is_On;

        // Send PUMP_STATE command to arduino
        mySerial.write(PUMP_STATE);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&is_On, sizeof(is_On));
        pump->data = (byte *)&is_On;

        doc["sensor"] = pump->type;
        doc["data"] = *(bool *)(pump->data);
        serializeJson(doc, dataToSend);
        // // strdup allocates memory to be free later, just like in the other cases above
        // dataToSend = is_On ? strdup("ON") : strdup("OFF");
        break;
      }
    default:
      break;
  }

  if (onDemand) {
    doc["mode"] = "ON_DEMAND";
  } else {
    doc["mode"] = "PERIODIC";
  }

  // Send the data over http
  serializeJson(doc, dataToSend);
  request->send_P(200, "text/plain", dataToSend.c_str());
  doc.clear();
  // deallocate memory
  //free(dataToSend);
}

void sensorInit() {
  temp = (Sensor *)malloc(sizeof(Sensor));
  temp->type = "Temperature";
  temp->data = (byte *)malloc(sizeof(float));

  humid = (Sensor *)malloc(sizeof(Sensor));
  humid->type = "Humidity";
  humid->data = (byte *)malloc(sizeof(float));

  light = (Sensor *)malloc(sizeof(Sensor));
  light->type = "Photosensor";
  light->data = (byte *)malloc(sizeof(int16_t));

  water = (Sensor *)malloc(sizeof(Sensor));
  water->type = "Water Level";
  water->data = (byte *)malloc(sizeof(int16_t));

  soil = (Sensor *)malloc(sizeof(Sensor));
  soil->type = "Soil Moisture";
  soil->data = (byte *)malloc(sizeof(int16_t));

  pump = (Sensor *)malloc(sizeof(Sensor));
  pump->type = "Pump State";
  pump->data = (byte *)malloc(sizeof(bool));
}
