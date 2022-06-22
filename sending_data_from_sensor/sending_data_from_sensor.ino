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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MAX30105 particleSensor;

// Adding wifi ssid, password and server name
const char* ssid = "PARAMITA";
const char* password = "76602223";
const char* serverName = "http://192.168.0.100/ESP8266_Smartwatch_BPM_Detection/sensorToDatabase.php";


const long utcOffsetInSeconds = 25200;


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;


int baca_bpm(){
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
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
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

}

void loop() {
  // Read data from sensor
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

}
