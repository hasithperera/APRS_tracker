

#include <stdio.h>
#include <SoftwareSerial.h>
#include "DRA818.h"  // uncomment the following line in DRA818.h (#define DRA818_DEBUG)
#include <LibAPRS_Tracker.h>

#define ADC_REFERENCE REF_5V
#define OPEN_SQUELCH false

/* Used Pins */
#define radio_wake 16
#define radio_ppt 13  //not needed
#define radio_pwr 17

#define RX 14  // arduino serial RX pin to the DRA818 TX pin
#define TX 15  // arduino serial TX pin to the DRA818 RX pin

SoftwareSerial *dra_serial;  // Serial connection to DRA818
DRA818 *dra;                 // the DRA object once instanciated
float freq;                  // the next frequency to scan
String packetBuffer;

SoftwareSerial gps(8, 10);  // RX, TX



char comment[] = "WVU-ERC";
int time_share = 0;

void setup() {

  Serial.begin(9600);  // for logging




  // APRS_setPreamble(750);


  Serial.println("Booting ...");
  Serial.println("initializing I/O");
  dra_serial = new SoftwareSerial(RX, TX);  // Instantiate the Software Serial Object.

  Serial.println("initializing DRA818");

  init_radio();
  radio_on();
  set_radio_pwr(0);
  // radio_off();

  freq = 145.390;  // rf.listen();


  //start GPS
  gps.begin(9600);

  // while(rf.available()){
  //   char inByte = rf.read();
  //   Serial.write(inByte);
  // }
  // rf.end();.39;
  dra = DRA818::configure(dra_serial, DRA818_VHF, freq, freq, 4, 8, 0, 0, DRA818_12K5, true, true, true, &Serial);
  if (!dra) {
    Serial.println("Radio - error");
  } else {
    Serial.println("Init - ok");
  }
}



void loop() {

  gps.listen();



  Serial.println();
  //if (time_share > 20) {
  location_update();
  while (time_share < 1000)
    while (gps.available() > 0) {
      time_share += 1;
      char inByte = gps.read();
      Serial.write(inByte);
    }
  time_share++;
}



void location_update() {
  //radio_TX();
  Serial.print("t:");
  Serial.println(time_share);
  Serial.println("Update APRS location");
  time_share = 0;
  APRS_init();
  char myCALL[] = "KC3RXZ";

  char Lat[] = "3938.83N";
  char Lon[] = "07958.05W";

  APRS_setPreamble(300);
  APRS_setCallsign(myCALL, 9);
  APRS_setLat(Lat);
  APRS_setLon(Lon);
  APRS_setSymbol('S');

  //delay(100);
  APRS_sendLoc(comment, strlen(comment), ' ');
  delay(1000);
}