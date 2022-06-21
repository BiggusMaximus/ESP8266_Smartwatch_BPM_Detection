// Adding sensor library and ESP library
#include <DHT.h>
#define DHTTYPE DHT22
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//Pin for sensor
#define soilPin 0
#define DHTPIN D4


// Adding wifi ssid, password and server name
const char* ssid = "PARAMITA";
const char* password = "76602223";
const char* serverName = "https://tranquil-tor-91089.herokuapp.com/api/monitor";


int16_t soilValue;
float humidity, temperature, heat_index;
String keyPass_value = "HY02_companyBangkit";
String sensorName = "DHT22";
String sensorLocation = "Home";

DHT dht(DHTPIN, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(soilPin, INPUT);
  dht.begin();
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP()); 

}

void loop() {
  // Read data from sensor
  soilValue = analogRead(soilPin);
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  heat_index = dht.computeHeatIndex(temperature, humidity);
  
  Serial.print("Humidity : " + String(humidity));
  Serial.print("  | Temperature : " + String(temperature));
  Serial.print("  | Heat index : " + String(heat_index) + "\n");

  // If wifi is connected
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");\
    String a = "coba";
    String httpRequestData = "?text=" + a + "";
    //String httpRequestData = "keyPass=" + keyPass_value + "&sensor=" + sensorName
                         // + "&location=" + sensorLocation + "&value1=" + String(humidity)
                          //+ "&value2=" + String(temperature) + "&value3=" + String(heat_index) + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    http.begin(client, httpRequestData.c_str());
    
  // Send HTTP POST request
    int httpResponseCode = http.GET();

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  delay(1000);
}
