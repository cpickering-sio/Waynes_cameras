/*This code interfaces two Serial TTL cameras with a Maker Buoy mainboard. The program will wake up every "image_interval" minutes
  to take a burst of "number_images" pictures separated by "time_between_images" milliseconds. User can select which camera
  (A or B) to use. Resolution and compression level can be selected. */

//Wayne J. Pavalko
//makerbuoyshop@gmail.com

#include <Arduino.h>   // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function
#include <Wire.h>
#include <SPI.h>

//TTL Serial Camera Library
#include <Adafruit_VC0706.h>

//Adafruit Feather Adalogger SD Card Library
#include "SdFat.h"
SdFat SD;

File myFile;

//Feather M0 RTC (for standby function only)
#include <RTCZero.h>
RTCZero rtc1;
int Alarm_Time;  //Used for Feather M0 Standby function

//Defines HW Serial on Feather MO--For SERCOM5 must comment out last lines in config files
Uart Serial2 (&sercom1, 12, 11, SERCOM_RX_PAD_3, UART_TX_PAD_0);
Uart Serial3 (&sercom5, 19, 6, SERCOM_RX_PAD_0, UART_TX_PAD_2);

//Defines HW Serial on Feather MO
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

//Defines HW Serial on Feather MO--For SERCOM5 must comment out last lines in config files
void SERCOM5_Handler()
{
  Serial3.IrqHandler();
}

#define CamA_connection Serial2
Adafruit_VC0706 CamA = Adafruit_VC0706(&CamA_connection);

#define CamB_connection Serial1
Adafruit_VC0706 CamB = Adafruit_VC0706(&CamB_connection);

//Global Variables for Images

uint8_t image_interval = 5; //Minutes between turning on and taking images
uint8_t number_images = 3; //Number of burst images to take (1 - 5)
uint16_t time_between_images = 500; //Time in milliseconds between burst images (Limits: 4 images, 1000; 5 images 750)
uint8_t image_resolution = 5; //Resolution, 1 (lowest) to 9 (max)
uint8_t image_compression = 2; //Compression, 1 (smallest image) to 3 (largest image)

//GPS variables; Do not set time here, see section below

int gps_year = 1;
byte gps_month = 1;
byte gps_day = 1;
byte gps_hour = 0;
byte gps_minute = 1;
byte gps_second = 0;
byte hundredths = 0;

char img_name[] = "0000000000000.jpg";

//*****************************Setup****************************************

void setup()
{
  //Status LED
  pinMode(13, OUTPUT);

  //Watchdog "Petting" Pin on Maker Buoy Mainboard
  //IMPORTANT: Must call ResetWatchdog() at least every 5 min
  pinMode(14, OUTPUT);

  //Status Light
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  //Start SD Card Interface
  SD.begin(4, SD_SCK_MHZ(12));

  //CamA MOSFET
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  delay(1000);

  //CamB MOSFET
  pinMode(17, OUTPUT);
  digitalWrite(17, LOW);
  delay(1000);

  CamA.begin(460800);
  delay(1000);

  CamB.begin(460800);
  delay(1000);

  //Set absolute RTC time here. Recommend making 5 min after expected flashing, then resetting at this time.
  rtc1.begin();
  rtc1.setTime(1, 10, 0);  // HH MM SS
  rtc1.setDate(1, 2, 26);  // DD MM YY

  //Required to use hardware serial on Feather M0
  pinPeripheral(11, PIO_SERCOM);
  pinPeripheral(12, PIO_SERCOM);

}  //End setup()

//*****************************Begin main loop****************************************

void loop()
{

  FlashStatus();

  if (minutes_past_midnight() % image_interval == 0) {

    digitalWrite(13, HIGH);

    TakeandStoreCamA(image_resolution, image_compression, number_images);
    //TakeandStoreCamB(image_resolution, image_compression, number_images);

    digitalWrite(13, LOW);

    delay(30000);
  }

  ResetWatchdog();

  //Put Feather M0 in Standby for 30 sec

  Alarm_Time += 30;
  Alarm_Time = Alarm_Time % 60;
  rtc1.setAlarmSeconds(Alarm_Time);
  rtc1.enableAlarm(rtc1.MATCH_SS);
  delay(20); //Test delay 12 Sept 2018
  rtc1.standbyMode();    // Sleep until next alarm match

}  //*****************************End main loop****************************************


void ResetWatchdog() {
  digitalWrite(14, HIGH);
  delay(100);
  digitalWrite(14, LOW);
}

void FlashStatus() {

  pinMode(9, INPUT);
  delay(250);
  int voltage = int(analogRead(A7) * 1.38 * 3.3 * 2 * 10 * 4.2 / 1024 + .5);
  pinMode(9, OUTPUT);

  if (voltage >= 40) {
    for (int i = 0; i <= 3; i++) {
      digitalWrite(13, HIGH);
      delay(75);
      digitalWrite(13, LOW);
      delay(250);
    }
  } else if (voltage >= 39) {
    for (int i = 0; i <= 2; i++) {
      digitalWrite(13, HIGH);
      delay(75);
      digitalWrite(13, LOW);
      delay(250);
    }
  } else if (voltage >= 38) {
    for (int i = 0; i <= 1; i++) {
      digitalWrite(13, HIGH);
      delay(75);
      digitalWrite(13, LOW);
      delay(250);
    }
  } else {
    digitalWrite(13, HIGH);
    delay(75);
    digitalWrite(13, LOW);
  }
}

uint16_t minutes_past_midnight() {
  return (rtc1.getHours() * 60 + rtc1.getMinutes());
}

uint32_t seconds_past_midnight() {
  return (rtc1.getHours() * 3600 + rtc1.getMinutes() * 60 + rtc1.getSeconds());  
}

//Camera A Take and Store Pic
void TakeandStoreCamA(uint8_t &resolution, uint8_t &compression, uint8_t &number_images) {

  //Code below takes photo and writes to SD card. Currently send_image() will not send an image if this code is active.

  digitalWrite(10, HIGH);

  delay(4000);

  switch (resolution) {
    case 1: CamA.setImageSize(VC0706_160x120);
      break;
    case 2: CamA.setImageSize(VC0706_320x240);
      break;
    case 3: CamA.setImageSize(VC0706_640x480);
      break;
    case 4: CamA.setImageSize(VC0706_1024x768);
      break;
    case 5: CamA.setImageSize(VC0706_1280x720);
      break;
    case 6: CamA.setImageSize(VC0706_1280x960);
      break;
    case 7: CamA.setImageSize(VC0706_1600x1200);
      break;
    case 8: CamA.setImageSize(VC0706_1920x1080);
      break;
    case 9: CamA.setImageSize(VC0706_2304x1296);
      break;
  }
  delay(2000);

  switch (compression) {
    case 1: CamA.setCompression(0x84);
      break;
    case 2: CamA.setCompression(0x72);
      break;
    case 3: CamA.setCompression(0x64);
      break;
  }
  delay(2000);

  uint32_t timer = millis();

  while (millis() < timer + 2000)
  {
    digitalWrite(13, LOW);
    delay(100);
    digitalWrite(13, HIGH);
    delay(100);
  }

  uint8_t hex1 = time_between_images >> 8;
  uint8_t hex2 = time_between_images & 0xff;

  CamA.takeMultiPic(number_images, hex1, hex2);
  delay(2000);

  for (uint8_t image_n = 1; image_n <= number_images; image_n++) {

    CamA.resetframeptr();

    //Start Image Save

    gps_year = rtc1.getYear();
    gps_month = rtc1.getMonth();
    gps_day = rtc1.getDay();
    gps_hour = rtc1.getHours();
    gps_minute = rtc1.getMinutes();
    gps_second = rtc1.getSeconds();

    img_name[0] = 'A';
    img_name[1] = (gps_year / 10) % 10 + '0';
    img_name[2] = (gps_year) % 10 + '0';
    img_name[3] = (gps_month / 10) % 10 + '0';
    img_name[4] = (gps_month) % 10 + '0';
    img_name[5] = (gps_day / 10) % 10 + '0';
    img_name[6] = (gps_day) % 10 + '0';
    img_name[7] = (gps_hour / 10) % 10 + '0';
    img_name[8] = (gps_hour) % 10 + '0';
    img_name[9] = (gps_minute / 10) % 10 + '0';
    img_name[10] = (gps_minute) % 10 + '0';
    img_name[11] = (gps_second / 10) % 10 + '0';
    img_name[12] = (gps_second) % 10 + '0';

    SD.remove(img_name);
    delay(500);  //Added to address missing images

    File imgFile = SD.open(img_name, FILE_WRITE);
    delay(500);  //Added to address missing images
        
    // Get the size of the image (frame) taken
    uint32_t jpglen = CamA.frameLength(image_n);
    delay(250);  //Original 250

    pinMode(8, OUTPUT);
    
    // Read all the data up to # bytes!
    byte wCount = 0; // For counting # of writes
    while (jpglen > 0) {
      // read 96 bytes at a time;
      uint8_t *buffer;
      uint8_t bytesToRead = min((uint16_t)96, jpglen);
      buffer = CamA.readPicture(bytesToRead, image_n);

      if (buffer == 0) {
        delay(500);
        buffer = CamA.readPicture(bytesToRead, image_n);
      }

      imgFile.write(buffer, bytesToRead);
      jpglen -= bytesToRead;
    }

    imgFile.close();

  } //End Multi-Image Save

  digitalWrite(10, LOW);
  delay(1000);

  //Complete Image Save
}

//Camera B Take and Store Pic
void TakeandStoreCamB(uint8_t &resolution, uint8_t &compression, uint8_t &number_images) {

  //Code below takes photo and writes to SD card. Currently send_image() will not send an image if this code is active.

  digitalWrite(17, HIGH);

  delay(4000);

  switch (resolution) {
    case 1: CamB.setImageSize(VC0706_160x120);
      break;
    case 2: CamB.setImageSize(VC0706_320x240);
      break;
    case 3: CamB.setImageSize(VC0706_640x480);
      break;
    case 4: CamB.setImageSize(VC0706_1024x768);
      break;
    case 5: CamB.setImageSize(VC0706_1280x720);
      break;
    case 6: CamB.setImageSize(VC0706_1280x960);
      break;
    case 7: CamB.setImageSize(VC0706_1600x1200);
      break;
    case 8: CamB.setImageSize(VC0706_1920x1080);
      break;
    case 9: CamB.setImageSize(VC0706_2304x1296);
      break;
  }
  delay(2000);

  switch (compression) {
    case 1: CamB.setCompression(0x84);
      break;
    case 2: CamB.setCompression(0x72);
      break;
    case 3: CamB.setCompression(0x64);
      break;
  }
  delay(2000);

  uint32_t timer = millis();

  while (millis() < timer + 2000)
  {
    digitalWrite(13, LOW);
    delay(100);
    digitalWrite(13, HIGH);
    delay(100);
  }

  uint8_t hex1 = time_between_images >> 8;
  uint8_t hex2 = time_between_images & 0xff;

  CamB.takeMultiPic(number_images, hex1, hex2);
  delay(2000);

  for (uint8_t image_n = 1; image_n <= number_images; image_n++) {

    CamB.resetframeptr();

    //Start Image Save

    gps_year = rtc1.getYear();
    gps_month = rtc1.getMonth();
    gps_day = rtc1.getDay();
    gps_hour = rtc1.getHours();
    gps_minute = rtc1.getMinutes();
    gps_second = rtc1.getSeconds();

    img_name[0] = 'B';
    img_name[1] = (gps_year / 10) % 10 + '0';
    img_name[2] = (gps_year) % 10 + '0';
    img_name[3] = (gps_month / 10) % 10 + '0';
    img_name[4] = (gps_month) % 10 + '0';
    img_name[5] = (gps_day / 10) % 10 + '0';
    img_name[6] = (gps_day) % 10 + '0';
    img_name[7] = (gps_hour / 10) % 10 + '0';
    img_name[8] = (gps_hour) % 10 + '0';
    img_name[9] = (gps_minute / 10) % 10 + '0';
    img_name[10] = (gps_minute) % 10 + '0';
    img_name[11] = (gps_second / 10) % 10 + '0';
    img_name[12] = (gps_second) % 10 + '0';

    SD.remove(img_name);
    delay(500);  //Added to address missing images

    File imgFile = SD.open(img_name, FILE_WRITE);
    delay(500);  //Added to address missing images

    uint32_t jpglen = CamB.frameLength(image_n);
    delay(250);

    pinMode(8, OUTPUT);
    // Read all the data up to # bytes!
    byte wCount = 0; // For counting # of writes
    while (jpglen > 0) {
      // read 96 bytes at a time;
      uint8_t *buffer;
      uint8_t bytesToRead = min((uint16_t)96, jpglen);
      buffer = CamB.readPicture(bytesToRead, image_n);

      if (buffer == 0) {
        delay(500);
        buffer = CamB.readPicture(bytesToRead, image_n);
      }

      imgFile.write(buffer, bytesToRead);

      jpglen -= bytesToRead;
    }
    imgFile.close();

  } //End Multi-Image Save

  digitalWrite(17, LOW);
  delay(1000);

  //Complete Image Save
}
