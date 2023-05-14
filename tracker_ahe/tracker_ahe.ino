

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
                  // the next frequency to scan
String packetBuffer;
SoftwareSerial gps(8, 10);  // RX, TX

char comment[] = "WVU-ERC";

char Lat[] = "3938.83N";
char Lon[] = "07958.05W";
char myCALL[] = "KC3RXZ";

int time_share = 0;
int msg_id = 0;
int loc_valid = 0;

float freq = 144.390;  // rf.listen();

#define debug 1


void setup() {

  Serial.begin(9600);  // for logging
  // APRS_setPreamble(750);

#ifdef debug
  Serial.println("Booting ...");
  Serial.println("initializing I/O");
  Serial.println("initializing DRA818");
#endif
  dra_serial = new SoftwareSerial(RX, TX);  // Instantiate the Software Serial Object.

 

  init_radio();
  radio_on();
  set_radio_pwr(0);


  //start GPS
  gps.begin(9600);

  //init radio module and change frequency 

  dra = DRA818::configure(dra_serial, DRA818_VHF, freq, freq, 4, 8, 0, 0, DRA818_12K5, true, true, true, &Serial);
  if (!dra) {
    Serial.println("Radio - error");
  } else {
    Serial.println("Init - ok");
  }
}

void loop() {

  gps.stopListening();
  if (msg_id > 0){
    location_update();
  }

  gps.listen();

  while (time_share < 40)
    while (gps.available() > 0) {
      time_share += 1;
      String gps_raw = gps.readStringUntil('\n');

      // all GPS packets are printed for debugging
      Serial.println(gps_raw);

      // Valid data: $GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D
      if (gps_raw.substring(0, 6) == "$GPGLL") {
       
        //simulate locked data
        //gps_raw = "$GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D";

        if (gps_raw.length() > 30) {
          // GPS locked
          msg_id ++;
          update_GPS(gps_raw);

          //i2c functions needed 

        }
      }
    }
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
  loc_valid = 1;
}


void location_update() {
  //radio_TX();
  Serial.println("t:");
  Serial.println(time_share);
  Serial.println("APRS:start");
  time_share = 0;
  APRS_init();

  APRS_setPreamble(750);
  APRS_setCallsign(myCALL, 9);
  APRS_setLat(Lat);
  APRS_setLon(Lon);
  APRS_setSymbol('S');

  //delay(100);
  sprintf(comment,"WVUERC msg_id:%d",msg_id);
  APRS_sendLoc(comment, strlen(comment), ' ');
  delay(1200);
  Serial.println("APRS:end");
  
  //clear location validitiy after sending 
  loc_valid =0;

}