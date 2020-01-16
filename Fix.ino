
/////////SetupFirebase&Wifi/////////
#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "-" 
#define FIREBASE_AUTH "-"
#define WIFI_SSID "L"
#define WIFI_PASSWORD "12345678i"

FirebaseData firebaseData;

////////SetupLCD///////
#include "Adafruit_GFX.h"     
#include "Adafruit_ILI9341.h" 
#include "SPI.h"

#define TFT_DC 4              
#define TFT_CS 15             
#define TFT_RST 2             
#define TFT_MISO 19           
#define TFT_MOSI 23           
#define TFT_CLK 18       
   
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

int w = tft.width();
int h = tft.height();


///////SetupDS18b20//////
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 22

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float previous_temperature;

////////SetupRelay/////////
int relayPakan = 21;

void setup(){
  Serial.begin(9600);
  sensors.begin();
  tft.begin();
  relaySetting();
  cekKoneksi();
  drawUI();
}
 
void loop()
{
  char printLog[256];
  sensors.requestTemperatures();
  float previous_temperature = sensors.getTempCByIndex(0);
  Serial.print("Celsius temperature: ");
  Serial.print(sensors.getTempCByIndex(0)); 
  Serial.print(" - Fahrenheit temperature: ");
  Serial.println(sensors.getTempFByIndex(0));
  displayTemperature();
  displaySuhuAPP();
  relayFirebase();
}

////////PROGRAM DISPLAY UI LCD//////////////
void drawUI()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2);

  tft.drawRoundRect(0, 0, 239 , 158, 4, ILI9341_WHITE);
  tft.fillRoundRect(20, 1, 200, 40, 4, ILI9341_GREEN);

  tft.drawRoundRect(0, 160, 239 , 158, 4, ILI9341_WHITE);
  tft.fillRoundRect(20, 161, 200, 40, 4, ILI9341_CYAN);

   tft.setCursor(100, 15);
   tft.setTextColor(ILI9341_BLACK);
   tft.setTextSize(2);

   tft.print("SUHU");

   tft.setCursor(120, 173);
   tft.setTextColor(ILI9341_BLACK);
   tft.setTextSize(2);
   tft.print("PH");
}

///////PROGRAM DISPLAY TEMPERATURE LCD//////////
void displayTemperature()
{     sensors.requestTemperatures();
      float temperature = sensors.getTempCByIndex(0);
      
      if(temperature != previous_temperature){
      tft.setCursor(40, 80);
      tft.setTextColor(ILI9341_BLACK);
      tft.setTextSize(4);
      tft.print(previous_temperature);
      
      tft.setTextSize(2);
      tft.setCursor(40, 80);
      tft.setCursor(170, 75);
      tft.print("o");
      tft.setCursor(185, 80);
      tft.setTextSize(4);
      tft.print("C");
        
      tft.setCursor(40, 80);
      tft.setTextColor(ILI9341_GREEN);
      tft.setTextSize(4);
      tft.print(temperature);
      
      tft.setTextSize(2);
      tft.setCursor(65, 80);
      tft.setCursor(170, 75);
      tft.print("o");
      tft.setCursor(185, 80);
      tft.setTextSize(4);
      tft.print("C");
}
      previous_temperature = temperature;
}

///////PROGRAM DISPLAY TEMP KE APP//////////
void displaySuhuAPP()
{
    FirebaseJson json;
  sensors.requestTemperatures(); 
  float c = sensors.getTempCByIndex(0);
Firebase.setFloat(firebaseData, "suhu", c);
}

////////PROGRAM SETTING KONEKSI FIREBASE DAN WIFI///////////
void cekKoneksi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  relaySetting();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}

void relaySetting()
{
   pinMode(relayPakan, OUTPUT);
   digitalWrite(relayPakan, LOW);
}
///////////PROGRAM RELAY FIREBASE///////////////
void relayFirebase()
{ 
  digitalWrite(relayPakan, HIGH);
  int z = 1;
  int x = 0;
  Firebase.setInt(firebaseData, "relayPakan", z);
  Serial.println("Menyalajan Relay");
  delay(20000);
  digitalWrite(relayPakan, LOW);
  Serial.println("Mematikan Relay");
  delay(2000);
}
