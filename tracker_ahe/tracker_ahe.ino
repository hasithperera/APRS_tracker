

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
#define alternate_freq 12
#define radio_sql 2

#define sim_packet 13
#define radio_freq_sw 12

#define RX 14  // arduino serial RX pin to the DRA818 TX pin
#define TX 15  // arduino serial TX pin to the DRA818 RX pin


//#define simulate 1


SoftwareSerial *dra_serial;  // Serial connection to DRA818
DRA818 *dra;                 // the DRA object once instanciated
                  // the next frequency to scan
String packetBuffer;
SoftwareSerial gps(8, 10);  // RX, TX



char Lat[] = "3938.83N";
char Lon[] = "07958.05W";

int time_share = 0;
int msg_id = 0;
int msg_valid = 0;
char myCALL[] = "KC3RXZ";
float freq;


void setup() {

  Serial.begin(9600);  // for logging

  Serial.println("[info] WVU ERC - APRS tracker");
  Serial.println("[info] IO init");
  dra_serial = new SoftwareSerial(RX, TX);  // Instantiate the Software Serial Object.

  init_radio();
  radio_on();
  set_radio_pwr(0);

  // afternate frequancy in run time
  if (digitalRead(radio_freq_sw))
    freq = 145.390;
  else
    freq = 144.390;

  //start GPS
  gps.begin(9600);

  //init radio module and change frequency 

  dra = DRA818::configure(dra_serial, DRA818_VHF, freq, freq, 4, 8, 0, 0, DRA818_12K5, true, true, true, &Serial);
  if (!dra) {
    Serial.println("[err ] RF init failed");
  } else {
    Serial.println("[info] RF OK");
  }
}

void loop() {

  gps.stopListening();

  if (msg_id > 0 & msg_valid == 1) {
    location_update();
  } else {
    Serial.println("[info] ------------------------ No location data");

  }
  gps.listen();


  while (time_share < 50) {
    while (gps.available() > 0) {
      time_share += 1;
      String gps_raw = gps.readStringUntil('\n');

      // all GPS packets are printed for debugging
      Serial.println(gps_raw);

      // Valid data: $GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D
      if (gps_raw.substring(0, 6) == "$GPGLL") {
        //simulate locked data
        if (digitalRead(sim_packet)) {
          gps_raw = "$GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D";
          Serial.println("[info] Sim Packet");
        }
        Serial.println(gps_raw);

        if (gps_raw.length() > 30) {
          msg_id++;
          // GPS locked
          update_GPS(gps_raw);

          //i2c functions needed 

        }
      }
    }
  }
  time_share = 0;
}

void update_GPS(String gps_data) {

  //data is sent with 2 decimal places 
  // reformat the data to be used by the APRS library 
 
  gps_data.substring(7, 7 + 8).toCharArray(Lat, 8);
  Lat[7] = char(gps_data.charAt(18));
  Serial.println(Lat);

  gps_data.substring(20, 20 + 12).toCharArray(Lon, 10);
  Lon[8] = char(gps_data.charAt(32));
  Serial.println(Lon);

  msg_valid = 1;
}


int location_update() {
  int wait = 400;

  // stop transmitting on a busy channel
  while(digitalRead(radio_sql)==0){
    wait--;
    Serial.print(".");
      if (wait==0){
        Serial.println("[info] Skip TX: busy channel");
        return 0;
      }
  }
  //radio_TX();
  Serial.println("[info] APRS:start");
  time_share = 0;
  APRS_init();

  APRS_setPreamble(300);
  APRS_setCallsign(myCALL, 9);
  APRS_setLat(Lat);
  APRS_setLon(Lon);
  APRS_setSymbol('S');
  char comment[30];
  //delay(100);
  sprintf(comment, "WVUERC msg_id:%d", msg_id);
  APRS_sendLoc(comment, strlen(comment), ' ');
  delay(1200);

  Serial.println("APRS:end");
  msg_valid = 0;
  //gps.flush();

}