#include "SD.h"
#include "Wire.h"
#include "OneWire.h"
#include "SPI.h"
#include "RTClib.h"
#include "Adafruit_ADS1X15.h"
#define LOG_INTERVAL 1000
#define ONE_WIRE_BUS 2
#define FILTRO 12

Adafruit_ADS1115 ads1115;
Adafruit_ADS1115 ads1116;
Adafruit_ADS1115 ads1117;
Adafruit_ADS1115 ads1118;

String ID = "box-2";   ///////////WRITE STATION ID, BETWEEN QUOTES, HERE

const float aref_voltage = 5;     
int16_t thermoc_01;
int16_t thermoc_02;
int16_t thermoc_03;
int16_t thermoc_04;
int16_t thermoc_05;
int16_t thermoc_06;
//int16_t thermoc_07;
//int16_t thermoc_08;
const int chipSelect = 10;
String strFilename;
//unsigned long seconds = 1000L;
//unsigned long minutes = seconds * 60;

RTC_DS1307 RTC;
File logfile;
OneWire oneWire(ONE_WIRE_BUS);
void(* resetFunc) (void) = 0;
void sensorsRead() {
 int sum0_l = 0;
int   sum0_t = 0;
int   sum1_l = 0;
int   sum1_t = 0;
int   sum2_l = 0;
int   sum2_t = 0;
int   sum3_l = 0;
int   sum3_t = 0;
int   sum4_l = 0;
int   sum4_t = 0;

  for (int i =0; i < FILTRO; i++){
      
      sum1_t = sum1_t + ads1115.readADC_Differential_0_1();
      delay(50);
      sum1_l = sum1_l + ads1115.readADC_Differential_2_3();
      delay(50);
      sum2_t = sum2_t +  ads1116.readADC_Differential_0_1();
      delay(50);
      sum2_l = sum2_l + ads1116.readADC_Differential_2_3();
      delay(50);
      sum3_t = sum3_t +  ads1117.readADC_Differential_0_1();
      delay(50);
      sum3_l = sum3_l + ads1117.readADC_Differential_2_3();
      //delay(50);
      //sum4_t = sum4_t +  ads1118.readADC_Differential_0_1();
     // delay(50);
      //sum4_l = sum4_l + ads1118.readADC_Differential_2_3();
      //delay(50);
  }
thermoc_01 = sum1_t/FILTRO;
thermoc_02 = sum2_t/FILTRO;
thermoc_03 = sum3_t/FILTRO;
thermoc_04 = sum4_t/FILTRO;
thermoc_05 = sum1_l/FILTRO;
thermoc_06 = sum2_l/FILTRO;
//thermoc_07 = sum3_l/FILTRO;
//thermoc_08 = sum4_l/FILTRO; 
}

void fileName() {
  strFilename = ID+".csv";
}

void setup() {
  Wire.begin();
  ads1115.begin(0x48);
  ads1116.begin(0x49); 
  ads1117.begin(0x4A); 
  //ads1118.begin(0x4A);
  Serial.begin(9600); RTC.begin();


  pinMode(10, OUTPUT);

  fileName();

  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    delay(5000);
    resetFunc();
    
  }
  else {
    Serial.println(F("card initialized."));
    logfile = SD.open(strFilename, FILE_WRITE);
    if (!logfile) {
      Serial.println(F("Could not create file."));
      delay(5000);
      resetFunc();
    }
    else {
      Serial.print(F("Logging to: "));
      Serial.println(strFilename);
    }
  }
  

  ads1115.setGain(GAIN_SIXTEEN);
  ads1116.setGain(GAIN_SIXTEEN);
  ads1117.setGain(GAIN_SIXTEEN);
  ads1118.setGain(GAIN_SIXTEEN);
}


void write_number_with_0(int number){
  if(number < 10){
    logfile.print("0");
  }
  logfile.print(number, DEC);
}



void print_number_with_0(int number){
  if(number < 10){
    Serial.print("0");
  }
  Serial.print(number, DEC);
}


void open_file(){
  if(!SD.exists(strFilename)){
    logfile = SD.open(strFilename, FILE_WRITE);
    logfile.println(F("time,thermoc-01,thermoc-02,thermoc-03,thermoc-04,thermoc-05,thermoc-06,thermoc-07,thermoc-08"));
    logfile.close();
    delay(50);
  }
  logfile = SD.open(strFilename, FILE_WRITE);
  if (!logfile) {
     Serial.print(F("Could not create file. Error"));
     resetFunc();
     
  }
  else {
     Serial.print(F("Logging to: "));
     Serial.println(strFilename);
  }
}



void loop() {
  Serial.print("A");
  sensorsRead();
  Serial.print("b");
  DateTime now;
  now = RTC.now();

  Serial.print("\n");
  Serial.print(F("Location ID: ")); Serial.println(ID);
  print_number_with_0(now.day()); Serial.print('/');
  print_number_with_0(now.month()); Serial.print('/');
  print_number_with_0(now.year()); Serial.print(' ');
  print_number_with_0(now.hour()); Serial.print(':');
  print_number_with_0(now.minute()); Serial.print(':');
  print_number_with_0(now.second()); Serial.print(' ');
  Serial.println(' ');
  Serial.print(F("thermoc_01: ")); Serial.println(thermoc_01);
  Serial.print(F("thermoc_02: ")); Serial.println(thermoc_02);
  Serial.print(F("thermoc_03: ")); Serial.println(thermoc_03);
  Serial.print(F("thermoc_04: ")); Serial.println(thermoc_04);
  Serial.print(F("thermoc_05: ")); Serial.println(thermoc_05);
  Serial.print(F("thermoc_06: ")); Serial.println(thermoc_06);
//  Serial.print(F("thermoc_07: ")); Serial.println(thermoc_07);
//  Serial.print(F("thermoc_08: ")); Serial.println(thermoc_08);
  Serial.print("\n");

  open_file();
  write_number_with_0(now.year()); logfile.print('-');
  write_number_with_0(now.month()); logfile.print('-');
  write_number_with_0(now.day()); logfile.print(' ');
  write_number_with_0(now.hour()); logfile.print(':');
  write_number_with_0(now.minute()); logfile.print(':');
  write_number_with_0(now.second()); logfile.print(',');
  logfile.print(thermoc_01); logfile.print(',');
  logfile.print(thermoc_02); logfile.print(',');
  logfile.print(thermoc_03); logfile.print(',');
  logfile.print(thermoc_04); logfile.print(',');
  logfile.print(thermoc_05); logfile.print(',');
  logfile.print(thermoc_06);
// logfile.print(',');
//  logfile.print(thermoc_07); logfile.print(',');
//  logfile.print(thermoc_08); 
  logfile.println();
  logfile.flush();
  logfile.close();
  delay(5000);    ///////////SPECIFY HOW MANY SECONDS BETWEEN MEASUREMENTS HERE
}
