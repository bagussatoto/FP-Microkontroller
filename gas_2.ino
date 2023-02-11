//Include the library files
#include "CTBot.h"; // LIBRARY BOT TELEGRAM
#include <ESP8266WiFi.h> // LIBRARY ESP
#include <ThingSpeak.h> // LIBRARY THINGS SPEAK

CTBot mybot; //deklarasi BOT
char ssid[] = "Marst"; //deklarasi nama WIFI
char pass[] = "testimoni"; //deklarasi password wifi
WiFiClient client; // untuk menghubungkan esp dengan thingpseak
unsigned long myChannelNumber = 1999441; //ID akun ThingSpeak
const char * myWriteAPIKey = "IXUIK0BL069L8I5K"; //memasukan apikey THINGSPEAK

String token = "5958096779:AAE_5spazuKocO7HTk0F9AEq7X8xSRW6_F4";
//ID TELEGRAM
const int id = 1147318206;


int activeButton = 0; //deklarasi kondisi hidup atau mati

//deklarasi
int redLed = 13; //LED MERAH
int greenLed = 12; //LED HIJAU
int whiteLed =15; //LED KUNING
int buzzer = 14; // BUZZER
int smokeA0 = A0; // SENSOR GAS 
int triggerPin = 16; // PIN TRIGGER SENSOR jARAK
int echoPin = 5; // PIN ECHO SENSOR jARAK


//FUNGSI UNTUK MEMBACA JARAK
long bacaJarak(int trig, int echo){
pinMode(trig, OUTPUT);
digitalWrite(trig, LOW);
delayMicroseconds(10);
digitalWrite(trig, HIGH);
delayMicroseconds(10);
digitalWrite(trig, LOW);
pinMode(echo, INPUT);
return pulseIn(echo, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600); //MENGHIDUPKAN SERIAL MONOTIOR

  pinMode(triggerPin, OUTPUT); //PIN TRIGGER MENJADI OUTPUT
  pinMode(echoPin, INPUT); //PIN ECHO MENJADI OUTPUT
  
  pinMode(whiteLed, OUTPUT); //PIN LED KUNING OUTPUT
  pinMode(redLed, OUTPUT); //PIN LED MERAH OUTPUT
  pinMode(greenLed, OUTPUT); //PIN LED HIJAU OUTPUT
  pinMode(buzzer, OUTPUT); ////PIN BUZZER OUTPUT
  pinMode(smokeA0, INPUT); //PIN GAS MENJADI OUTPUT

  Serial.println("Memulai telegram bot"); 
  mybot.wifiConnect(ssid, pass);
  ThingSpeak.begin(client); // MENGHIDUPKAN THINGS SPEAK
  
  mybot.setTelegramToken(token);
  if(mybot.testConnection()){
     Serial.println("koneksi berhasil");
  }
  else{
    Serial.println("Koneksi Gagal ");
  }  
}



//Looop
void loop() {

  long duration, jarak;
  digitalWrite(triggerPin, LOW);
  digitalWrite(triggerPin, HIGH);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  jarak = (duration/2) / 29.1;
 
  Serial.print("Jarak : ");
  Serial.print(jarak);
  Serial.println(" cm");


  
int analogSensor = analogRead(smokeA0);
int pwm = map(analogSensor,9,1023,0,100);
String nilai = String(pwm);

ThingSpeak.writeField(myChannelNumber, 1, pwm, myWriteAPIKey); 
ThingSpeak.writeField(myChannelNumber, 2,jarak, myWriteAPIKey); 

TBMessage msg;

     //Menerima data dari telegram
  if(mybot.getNewMessage(msg)){
    Serial.println("Pesan masuk : "+msg.text);
    String pesan = msg.text;
    
    if(pesan=="On"){
      activeButton = 1;
      mybot.sendMessage(id, "SELAMAT DATANG KELOMPOK 5 BOT SIAP DIGUNAKAN");
     digitalWrite(whiteLed, HIGH); //led nodemcu menyala
     }else if (pesan=="Off"){
      activeButton = 0;
      mybot.sendMessage(id, "TERIMA KASIH SUDAH MENGGUNAKAN BOT KAMI");
      digitalWrite(whiteLed, LOW);
      digitalWrite(greenLed, LOW);
      digitalWrite(redLed, LOW);//led nodemcu mati
    } else{
       mybot.sendMessage(id, "ketikkan 'On'/ 'Off' ");
    }
  }

    // looping
    if(activeButton ==1 && pwm > 70){
      digitalWrite(whiteLed, HIGH); //led nodemcu menyala
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLed, LOW);
      digitalWrite(buzzer,HIGH);
      tone(buzzer,6000,2000); //MENGATUR KETTINGIAN SUARA
      tone(buzzer,5000,2000);
      tone(buzzer,1000,2000);

      }else if(activeButton == 0 && pwm > 70){
        digitalWrite(redLed, HIGH);
        delay(200);
         digitalWrite(redLed, LOW);

        mybot.sendMessage(id, "TERCIUM BAU GAS !");
      }else if(activeButton == 1 && pwm < 70 && jarak < 5){
       
        digitalWrite(redLed, LOW);
        digitalWrite(whiteLed, HIGH);
        digitalWrite(greenLed, HIGH);

       
      }else if(activeButton == 1 && pwm < 70 && jarak > 8){
       
        digitalWrite(redLed, LOW);
        digitalWrite(whiteLed, LOW);
        digitalWrite(greenLed, HIGH);
  
       
      }else if(activeButton == 0 && pwm < 70){
        digitalWrite(greenLed, LOW);
        digitalWrite(whiteLed, LOW);
      }
 
    
  }
