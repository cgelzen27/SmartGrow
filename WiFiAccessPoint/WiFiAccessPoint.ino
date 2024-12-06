#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>

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

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

SoftwareSerial mySerial(rx, tx);
AsyncWebServer server(80);

void setup() {
  delay(1000);
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
    doServerRequest(request, TEMPERATURE);
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, HUMIDITY);
  });
  server.on("/photosensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, PHOTOSENSOR);
  });
  server.on("/water-level", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, WATER_LVL);
  });
  server.on("/soil-moisture", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, SOIL_MOISTURE);
  });
  server.on("/pump-state", HTTP_GET, [](AsyncWebServerRequest *request) {
    doServerRequest(request, PUMP_STATE);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {

}

/* helper function for initiating a command to the periperhal hub(Arduino) to request sensor data then sending it over httpGet request */
void doServerRequest(AsyncWebServerRequest *request, COMMANDS cmd) {

  char *dataToSend;

  // Process the command
  switch (cmd) {
    case TEMPERATURE:
      {
        float tempVal;

        // Send TEMPERATURE command to arduino
        mySerial.write(TEMPERATURE);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&tempVal, sizeof(tempVal));

        // Convert float value to a string
        int len = snprintf(NULL, 0, "%f", tempVal) + 1;
        dataToSend = (char *)malloc(len);
        snprintf(dataToSend, len, "%.2f", tempVal);

        break;
      }
    case HUMIDITY:
      {
        float humVal;

        // Send HUMIDITY command to arduino
        mySerial.write(HUMIDITY);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&humVal, sizeof(humVal));

        // Convert float value to a string
        int len = snprintf(NULL, 0, "%f", humVal) + 1;
        dataToSend = (char *)malloc(len);
        snprintf(dataToSend, len, "%.2f", humVal);

        break;
      }
    case PHOTOSENSOR:
      {
        int16_t lightVal;

        // Send WATER_LVL command to arduino
        mySerial.write(PHOTOSENSOR);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&lightVal, sizeof(lightVal));

        // Convert int value to a string
        int len = snprintf(NULL, 0, "%d", lightVal) + 1;
        dataToSend = (char *)malloc(len);
        snprintf(dataToSend, len, "%d", lightVal);

        break;
      }
    case WATER_LVL:
      {
        int16_t waterVal;

        // Send WATER_LVL command to arduino
        mySerial.write(WATER_LVL);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&waterVal, sizeof(waterVal));

        // Convert int value to a string
        int len = snprintf(NULL, 0, "%d", waterVal) + 1;
        dataToSend = (char *)malloc(len);
        snprintf(dataToSend, len, "%d", waterVal);

        break;
      }
    case SOIL_MOISTURE:
      {
        int16_t soilVal;

        // Send SOIL_MOISTURE command to arduino
        mySerial.write(SOIL_MOISTURE);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&soilVal, sizeof(soilVal));

        // Convert int value to a string
        int len = snprintf(NULL, 0, "%d", soilVal) + 1;
        dataToSend = (char *)malloc(len);
        snprintf(dataToSend, len, "%d", soilVal);

        break;
      }
    case PUMP_STATE:
      {
        bool is_On;

        // Send PUMP_STATE command to arduino
        mySerial.write(PUMP_STATE);
        // Read the sensor value sent by arduino
        mySerial.readBytes((byte *)&is_On, sizeof(is_On));

        // strdup allocates memory to be free later, just like in the other cases above
        dataToSend = is_On ? strdup("ON") : strdup("OFF");
        break;
      }
    default:
      break;
  }

  // Send the data over http
  request->send_P(200, "text/plain", dataToSend);
  // deallocate memory
  free(dataToSend);
}
