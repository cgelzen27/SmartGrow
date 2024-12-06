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
  const char *  type;
  bool          enablePeriodic;
  unsigned long samplingRate;
} SensorConfig;

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

void setup() {
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
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected");
    Serial.println("*********Attempting to Reconnect*********");
  } 

  // TODO: Listen to serial reads from terminal then execute the command (ex: ON DEMAND data gather)
}

/* function for requesting a read (HttpGET) from a specific server route */
void getReading(SENSOR sensor) {

  String sensorReading;

  // Process the reading
  switch (sensor) {
    case TEMPERATURE:
      Serial.println("Getting temperature data.......");
      sensorReading = httpGETRequest(serverTemperature);
      Serial.print(" Temperature = ");
      Serial.print(sensorReading);
      Serial.println();  // Celsius or Farenheit
      break;
    case HUMIDITY:
      Serial.println("Getting humidity data.......");
      sensorReading = httpGETRequest(serverHumidity);
      Serial.print(" Humidity = ");
      Serial.print(sensorReading);
      Serial.println();
      break;
    case PHOTOSENSOR:
      Serial.println("Getting photosensor data.......");
      sensorReading = httpGETRequest(serverPhotosensor);
      Serial.print(" Light value = ");
      Serial.print(sensorReading);
      Serial.println();
      break;
    case WATER_LVL:
      Serial.println("Getting water level reading.......");
      sensorReading = httpGETRequest(serverWaterLVL);
      Serial.print(" Water Level = ");
      Serial.print(sensorReading);
      Serial.println();
      break;
    case SOIL_MOISTURE:
      Serial.println("Getting soil moisture data.......");
      sensorReading = httpGETRequest(serverSoilMoist);
      Serial.print(" Soil Moisture = ");
      Serial.print(sensorReading);
      Serial.println();
      break;
    case PUMP_STATE:
      Serial.println("Getting Pump's state.......");
      sensorReading = httpGETRequest(serverPumpState);
      Serial.print(" Pump State = ");
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

// TODO: Hardware Timer that can fire based on an interval variable.
