#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Adding wifi ssid, password and server name
const char* ssid = "PARAMITA";
const char* password = "76602223";
const char* serverName = "http://192.168.0.100/ESP8266_Smartwatch_BPM_Detection/sensorToDatabase.php";


const long utcOffsetInSeconds = 25200;
String hari[7] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  // put your setup code here, to run once:
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
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  timeClient.begin();
}

void loop() {
  // Read data from sensor
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
   
  int hour_time = ptm->tm_hour;
  if(hour_time < 10) {
    Serial.print("0");
  }
  Serial.print(String(hour_time) + ":");
  
  int minute_time = ptm->tm_min; 
  if(minute_time < 10) {
    Serial.print("0");
  }
  Serial.print(String(minute_time) + ":");
  
  int sec_time = ptm->tm_sec;
  if(sec_time < 10) {
    Serial.print("0");
  }
  Serial.print(sec_time);

  Serial.print(" | ");

  int monthDay = ptm->tm_mday;
  if(monthDay < 10) {
    Serial.print("0");
  }
  Serial.print(String(monthDay) + "/");
  
  int currentMonth = ptm->tm_mon+1;
  if(currentMonth < 10) {
    Serial.print("0");
  }
  Serial.print(String(currentMonth) + "/");
  
  int currentYear = ptm->tm_year+1900;
  Serial.print(currentYear);
  Serial.println();

  
  delay(1000);
}
