#include <WiFi.h>
#include <HTTPClient.h>

/* Wifi Configurations */
#define APSSID "ESP_AccessPoint"
#define APPSK "plantsarecool"
#define server "http://192.168.4.1/"

/* Server Routes for sensor data */
#define serverTemperature server "temperature"
#define serverHumidity server "humidity"
#define serverPhotosensor server "photosensor"
#define serverWaterLVL server "water-level"
#define serverSoilMoist server "soil-moisture"
#define serverPumpState server "pump-state"

#define onDemandTemp serverTemperature "-onDemand"
#define onDemandHumid serverHumidity "-onDemand"
#define onDemandPhoto serverPhotosensor "-onDemand"
#define onDemandWater serverWaterLVL "-onDemand"
#define onDemandSoil serverSoilMoist "-onDemand"
#define onDemandPump serverPumpState "-onDemand"

#define defaultRate 10000
/* All current possible data readings */
enum SENSOR {
  TEMPERATURE = 1,
  HUMIDITY,
  PHOTOSENSOR,
  WATER_LVL,
  SOIL_MOISTURE,
  PUMP_STATE,
};

typedef struct {
  const char *type;
  bool enablePeriodic;
  unsigned long samplingRate;
  unsigned long lastTime;
} SensorConfig;

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

SensorConfig *temp, *humid, *light, *water, *soil, *pump;

void setup() {
  configInit();
  Serial.begin(19200);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  // Wifi Setup
  connectToWifi();
  WiFi.setAutoReconnect(true);
  Serial.println("Press 'M' to list all commands");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected");
    Serial.println("*********Attempting to Reconnect*********");
    delay(5000);
  } else {
    if ((millis() - temp->lastTime) > temp->samplingRate && temp->enablePeriodic) {
      getReading(TEMPERATURE, false);
      temp->lastTime = millis();
    }
    if ((millis() - humid->lastTime) > humid->samplingRate && humid->enablePeriodic) {
      getReading(HUMIDITY, false);
      humid->lastTime = millis();
    }
    if ((millis() - light->lastTime) > light->samplingRate && light->enablePeriodic) {
      getReading(PHOTOSENSOR, false);
      light->lastTime = millis();
    }
    if ((millis() - water->lastTime) > water->samplingRate && water->enablePeriodic) {
      getReading(WATER_LVL, false);
      water->lastTime = millis();
    }
    if ((millis() - soil->lastTime) > soil->samplingRate && soil->enablePeriodic) {
      getReading(SOIL_MOISTURE, false);
      soil->lastTime = millis();
    }
    if ((millis() - pump->lastTime) > pump->samplingRate && pump->enablePeriodic) {
      getReading(PUMP_STATE, false);
      pump->lastTime = millis();
    }
  }

  // TODO: Listen to serial reads from terminal then execute the command (ex: ON DEMAND data gather)
  if (Serial.available()) {
    char userInput = tolower(Serial.read());

    if (userInput == 'm') {
      listCommands();
    }
    if (userInput == 'a') {
      getReading(TEMPERATURE, true);
      delay(100);
      getReading(HUMIDITY, true);
      delay(100);
      getReading(PHOTOSENSOR, true);
      delay(100);
      getReading(WATER_LVL, true);
      delay(100);
      getReading(SOIL_MOISTURE, true);
      delay(100);
      getReading(PUMP_STATE, true);
    }
    if (userInput == 't') {
      getReading(TEMPERATURE, true);
    }
    if (userInput == 'h') {
      getReading(HUMIDITY, true);
    }
    if (userInput == 'l') {
      getReading(PHOTOSENSOR, true);
    }
    if (userInput == 'w') {
      getReading(WATER_LVL, true);
    }
    if (userInput == 's') {
      getReading(SOIL_MOISTURE, true);
    }
    if (userInput == 'p') {
      getReading(PUMP_STATE, true);
    }
  }
}

/* function for requesting a read (HttpGET) from a specific server route */
void getReading(SENSOR sensor, bool onDemand) {

  String sensorReading;

  // Process the reading
  switch (sensor) {
    case TEMPERATURE:
      Serial.println("Getting temperature data.......");
      if (onDemand)
        sensorReading = httpGETRequest(onDemandTemp);
      else
        sensorReading = httpGETRequest(serverTemperature);
      Serial.print(sensorReading);
      Serial.println();  // Celsius or Farenheit
      break;
    case HUMIDITY:
      Serial.println("Getting humidity data.......");
      if (onDemand)
        sensorReading = httpGETRequest(onDemandHumid);
      else
        sensorReading = httpGETRequest(serverHumidity);
      Serial.print(sensorReading);
      Serial.println();
      break;
    case PHOTOSENSOR:
      Serial.println("Getting photosensor data.......");
      if (onDemand)
        sensorReading = httpGETRequest(onDemandPhoto);
      else
        sensorReading = httpGETRequest(serverPhotosensor);
      Serial.print(sensorReading);
      Serial.println();
      break;
    case WATER_LVL:
      Serial.println("Getting water level reading.......");
      if (onDemand)
        sensorReading = httpGETRequest(onDemandWater);
      else
        sensorReading = httpGETRequest(serverWaterLVL);
      Serial.print(sensorReading);
      Serial.println();
      break;
    case SOIL_MOISTURE:
      Serial.println("Getting soil moisture data.......");
      if (onDemand)
        sensorReading = httpGETRequest(onDemandSoil);
      else
        sensorReading = httpGETRequest(serverSoilMoist);
      Serial.print(sensorReading);
      Serial.println();
      break;
    case PUMP_STATE:
      Serial.println("Getting Pump's state.......");
      if (onDemand)
        sensorReading = httpGETRequest(onDemandPump);
      else
        sensorReading = httpGETRequest(serverPumpState);
      Serial.println(sensorReading);
      break;
    default:
      Serial.println("Unknown sensor");
      break;
  }
  Serial.println();
}

void connectToWifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
}

/* function for implementing a get request to a server route for data collection */
String httpGETRequest(const char *serverName) {
  WiFiClient client;
  HTTPClient http;

  // begin a request session to this route
  http.begin(client, serverName);

  // initiate get request to the route
  int httpResponseCode = http.GET();

  // stores the data collected
  String payload;

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error Code: ");
    Serial.println(httpResponseCode);
  }

  // end session
  http.end();

  return payload;
}

void listCommands() {
  Serial.println("Press A to initiate ONDEMAND for all sensor.");
  Serial.println("Press T to initiate ONDEMAND Temperature sensor data.");
  Serial.println("Press H to initiate ONDEMAND Humidity sensor data.");
  Serial.println("Press L to initiate ONDEMAND Photosensor data.");
  Serial.println("Press W to initiate ONDEMAND Water Level sensor data.");
  Serial.println("Press S to initiate ONDEMAND Soil Moisture sensor data.");
  Serial.println("Press P to initiate ONDEMAND Pump state.");
}

void configInit() {
  temp = (SensorConfig *)malloc(sizeof(SensorConfig));
  temp->type = "Temperature";
  temp->enablePeriodic = true;
  temp->samplingRate = defaultRate;
  temp->lastTime = millis();

  humid = (SensorConfig *)malloc(sizeof(SensorConfig));
  humid->type = "Humidity";
  humid->enablePeriodic = true;
  humid->samplingRate = defaultRate;
  humid->lastTime = millis();

  light = (SensorConfig *)malloc(sizeof(SensorConfig));
  light->type = "Photosensor";
  light->enablePeriodic = true;
  light->samplingRate = defaultRate;
  light->lastTime = millis();

  water = (SensorConfig *)malloc(sizeof(SensorConfig));
  water->type = "Water Level";
  water->enablePeriodic = true;
  water->samplingRate = defaultRate;
  water->lastTime = millis();

  soil = (SensorConfig *)malloc(sizeof(SensorConfig));
  soil->type = "Soil Moisture";
  soil->enablePeriodic = true;
  soil->samplingRate = defaultRate;
  soil->lastTime = millis();

  pump = (SensorConfig *)malloc(sizeof(SensorConfig));
  pump->type = "Pump State";
  pump->enablePeriodic = true;
  pump->samplingRate = defaultRate;
  pump->lastTime = millis();
}