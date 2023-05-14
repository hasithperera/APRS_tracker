

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

char Lat[] = "3938.83N";
char Lon[] = "07958.05W";

int time_share = 0;
int msg_id = 0;



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

  freq = 144.390;  // rf.listen();


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

  //gps.listen();



  //Serial.println();
  //if (time_share > 20) {




  // limit 3000 ~7 sec when using characters
  // reading lines: need to change

  /*
  while (time_share < 3000)
    while (gps.available() > 0) {
      time_share += 1;

      char inByte = gps.read();
      Serial.write(inByte);


      }
      */


  //if inByte=='\n'
  //}

  gps.stopListening();

  if (msg_id > 0){

    location_update();
  }

  gps.listen();


  while (time_share < 40)
    while (gps.available() > 0) {
      time_share += 1;

      String gps_raw = gps.readStringUntil('\n');
      Serial.println(gps_raw);
      // Valid data: $GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D


      if (gps_raw.substring(0, 6) == "$GPGLL") {
       
        //simulate locked data
        //gps_raw = "$GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D";

        
        Serial.println(gps_raw);

        if (gps_raw.length() > 30) {
          msg_id ++;
          // GPS locked

          update_GPS(gps_raw);
        }
      }

      //delay(5000);
    }
}

void update_GPS(String gps_data) {
  //char data[100];
  //gps_data.toCharArray(*data,100);
  //Serial.println("extraction function");

  //Serial.print(gps_data.substring(7,7+10));
  //Serial.println(gps_data.charAt(18));


  gps_data.substring(7, 7 + 8).toCharArray(Lat, 8);
  Lat[7] = char(gps_data.charAt(18));
  Serial.println(Lat);

  gps_data.substring(20, 20 + 12).toCharArray(Lon, 10);
  Lon[8] = char(gps_data.charAt(32));
  Serial.println(Lon);
}


void location_update() {
  //radio_TX();
  Serial.println("t:");
  Serial.println(time_share);
  Serial.println("APRS:start");
  time_share = 0;
  APRS_init();
  char myCALL[] = "KC3RXZ";



  APRS_setPreamble(500);
  APRS_setCallsign(myCALL, 9);
  APRS_setLat(Lat);
  APRS_setLon(Lon);
  APRS_setSymbol('S');

  //delay(100);
  sprintf(comment,"WVUERC msg_id:%d",msg_id);
  APRS_sendLoc(comment, strlen(comment), ' ');
  delay(1200);
  Serial.println("APRS:end");

  //gps.flush();
}