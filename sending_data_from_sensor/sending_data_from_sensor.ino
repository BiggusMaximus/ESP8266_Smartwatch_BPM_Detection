#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "MAX30105.h"
#include "heartRate.h"

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MAX30105 particleSensor;


const char* ssid = "PARAMITA";
const char* password = "76602223";
const char* serverName = "http://192.168.0.100/ESP8266_Smartwatch_BPM_Detection/sensorToDatabase.php";


const long utcOffsetInSeconds = 25200;
unsigned long timerDelay = 5000;
unsigned long lastTime = 0;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
const byte RATE_SIZE = 4; 
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0; 
float beatsPerMinute;
int beatAvg;


int baca_bpm(){
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE; 
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  if (irValue < 50000)
    Serial.print(" No finger?");
    
  return beatAvg;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP()); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  display.display();
  delay(2000); 

  display.clearDisplay();
  timeClient.begin();
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) 
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); 
  particleSensor.setPulseAmplitudeRed(0x0A); 
  particleSensor.setPulseAmplitudeGreen(0); 

}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
   
  int hour_time = ptm->tm_hour;
  if(hour_time < 10) {
    Serial.print("0");
    display.print("0");
  }
  Serial.print(String(hour_time) + ":");
  display.print(String(hour_time) + ":");
  
  int minute_time = ptm->tm_min; 
  if(minute_time < 10) {
    Serial.print("0");
    display.print("0");
  }
  Serial.print(String(minute_time) + ":");
  display.print(String(minute_time) + ":");
  
  int sec_time = ptm->tm_sec;
  if(sec_time < 10) {
    Serial.print("0");
    display.print("0");
  }
  Serial.print(sec_time);
  display.print(String(sec_time) + " | ");
  Serial.print(" | ");

  int monthDay = ptm->tm_mday;
  if(monthDay < 10) {
    Serial.print("0");
    display.print("0");
  }
  Serial.print(String(monthDay) + "/");
  display.print(String(monthDay) + "/");
  
  int currentMonth = ptm->tm_mon+1;
  if(currentMonth < 10) {
    Serial.print("0");
    display.print("0");
  }
  Serial.print(String(currentMonth) + "/");
  display.print(String(currentMonth) + "/");
  
  int currentYear = ptm->tm_year+1900;
  Serial.print(currentYear);
  display.print(currentYear);
  Serial.println();
 
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.print("BPM : " + String(baca_bpm()));
  display.display(); 
  
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestData = "bpm=" + String(baca_bpm()) + "";  
      int httpResponseCode = http.POST(httpRequestData);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
