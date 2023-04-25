#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256
#include <Blynk.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include "ThingSpeak.h"

//Blynk Widget
BlynkTimer timer;

//Setting For Blynk
char auth[] = "";
char ssid[] = "";
char pass[] = "";

//DS18B20 Sensors Library Configuration
OneWire onewire(D4);
DallasTemperature sensors(&onewire);

//ThingSpeak Variable
WiFiClient client;
unsigned long idthingspeak = ;
const char *apikey = "";
int interval = 10000;
unsigned long thingspeakmillis; 
unsigned long lastmillis;


//Global Variable
int value  = 0;
int lastvalue = 0;
int blynkvalue = 28;
int blynknotify = 0;
#define pinrelay D6

//LCD Configuration Library
LiquidCrystal_I2C lcd(0x27,16,2); //Declaration LCD I2C Library Paramaters (Address,Column,Row)


//Blynk Function
BLYNK_READ(V0)
{
  sensors.requestTemperatures(); //Request Temperature From Library
  blynkvalue = sensors.getTempCByIndex(0); //Insert Data Temperature To Variable Blynk
  Blynk.virtualWrite(V0,blynkvalue); //Send Data Temperature To Gauge Blynk
}


void notif()
{
  if (blynknotify == 1 && blynkvalue <= 32 && blynkvalue > 25)
  {
    Blynk.notify("Suhu Aquarium Kembali Normal !!!");
    blynknotify = !blynknotify;
  }
  else if (blynkvalue > 32)
  {
    Blynk.notify("Suhu Aquarium Terlalu Panas !!!");
    blynknotify = 1;
  }
  else if(blynkvalue <= 25)
  {
    Blynk.notify("Suhu Aquarium Terlalu Dingin !!!");
    blynknotify = 1;
  }
}


//Setup Function Arduino
void setup()
{
  Serial.begin(9600); //Start Serial Monitor Nodemcu
  Serial.println("Success Upload!!");
  WiFi.begin(ssid,pass);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting  ");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }
  ThingSpeak.begin(client);
  Blynk.begin(auth,ssid,pass,"iot.amikom.ac.id",8080); //Setup Blynk Paramaters(auth Code,SSID Wifi,PASS Wifi,Server name,Port Internet)
  sensors.begin(); //Starting Aquarium Temperature
  lcd.init(); //Setup LCD
  lcd.backlight(); //Setup LCD
  lcd.clear(); //Setup LCD
  timer.setInterval(10000L,notif);  //Setting Interval Function For Blynk Notification
  pinMode(pinrelay,OUTPUT);
}
 
//Main Function Arduino
void loop()
{
  thingspeakmillis = millis();
  sensors.requestTemperatures();
  value = sensors.getTempCByIndex(0);
  if (value != lastvalue)
  {
    lcd.clear();
    lcd.home();
    lcd.print("Water Temp : ");
    lcd.print(value);
    lcd.print(" °C");
    if (value > 32)
    {
      digitalWrite(pinrelay,HIGH);
      lcd.setCursor(0,1);
      lcd.print("Suhu Air Panas");
    }
    else if(value >= 25)
    {
      digitalWrite(pinrelay,LOW);
      lcd.setCursor(0,1);
      lcd.print("Suhu Air Normal");
    }
    else {
      digitalWrite(pinrelay,LOW);
      lcd.setCursor(0,1);
      lcd.print("Suhu Air Dingin");
    }
  }
  Blynk.run();
  timer.run();
  //ThingSpeak Millis
  if (thingspeakmillis - lastmillis >= interval)
  {
    int flag = ThingSpeak.writeField(idthingspeak,1,value,apikey);
    if (flag != 200)
    {
      Serial.println("Failed Send Request To ThingSpeak API!!!");
    }
    lastmillis = thingspeakmillis;
  }
  lastvalue = value;
}
