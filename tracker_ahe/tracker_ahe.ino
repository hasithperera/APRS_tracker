

#include <stdio.h>
//#include <SoftwareSerial.h>
#include <HardwareSerial.h>
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

#define sim_packet 11
#define radio_freq_sw 12

#define RX 14  // arduino serial RX pin to the DRA818 TX pin
#define TX 15  // arduino serial TX pin to the DRA818 RX pin

// old 300
#define timeout 150

#define freq_rx 144.978
#define ctcss 146.2 

//APRS specification
#define symbol 'O'
#define SSID 11

//APRS symbol
  // S - shuttle
  // < - Bike
  //O - Balloon
  //> - car

// station SSID
  // 9 - Mobile
  // 11 - Spacecraft

//#define simulate 1


SoftwareSerial *dra_serial;  // Serial connection to DRA818
DRA818 *dra;                 // the DRA object once instanciated

String packetBuffer;
SoftwareSerial gps(8, 10);  // RX, TX


// W8CUL location
char Lat[] = "3938.76Nxx";
char Lon[] = "07958.40Wxx";
char alt[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

int time_share = 0;
int msg_id = 0;
int msg_valid = 0;
char myCALL[] = "KE8TJE";
float freq_tx;


void setup() {

  Serial.begin(9600);  // for logging

  Serial.println("[info] KE8TJE - APRS tracker v2");
  Serial.println("[info] IO init");
  dra_serial = new SoftwareSerial(RX, TX);  // Instantiate the Software Serial Object.

  init_radio();
  radio_on();
  set_radio_pwr(0);

  // afternate frequancy in run time
  if (digitalRead(radio_freq_sw)) {
    freq_tx = 145.390;
    Serial.println("[info] Alternate freq set");
  } else {
    freq_tx = 144.390;
    Serial.println("[info] APRS freq set");
  }
  //start GPS 
  gps.begin(9600);

  //init radio module and change frequency

  //dra = DRA818::configure(dra_serial, DRA818_VHF, freq_rx, freq_tx, 4, 8, 0, 0, DRA818_12K5, true, true, true, &Serial);
  dra = DRA818::configure(dra_serial, DRA818_VHF, freq_tx, freq_tx, 4, 8, 0, 0, DRA818_12K5, true, true, true, &Serial);
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

  while (time_share < timeout) {
    while (gps.available() > 0) {
      time_share += 1;
      String gps_raw = gps.readStringUntil('\n');

      // all GPS packets are printed for debugging
      Serial.println(gps_raw);

      // Valid data: $GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D
      if (gps_raw.substring(0, 6) == "$GPGLL") {
        //simulate locked data
        if (digitalRead(sim_packet) == 0) {
          gps_raw = "$GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D";
          
          // V2 - Packet
          // $GPGLL,3927.83254,N,0808.25462,W,130448.00,A,A*71

          // V3 - Packet
          // $GNRMC,134055.000,A,3509.7572,N,09010.4938,W,1.51,338.00,121023,,,A*6C
          
          //
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

      //long packet


      if (gps_raw.substring(0, 6) == "$GPGGA") {
        //simulate locked data
        if (digitalRead(sim_packet) == 0) {
          gps_raw = "$GPGGA,191757.00,3938.28486,N,07957.13511,W,1,03,2.71,274.5,M,-33.9,M,,*6F";
          Serial.println("[info] Sim Packet 2");
        }
        Serial.println(gps_raw);

        if (gps_raw.length() > 40) {
          msg_id++;
          // GPS locked
          update_GPS_alt(gps_raw);

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

  /*
  gps_data.substring(7, 7 + 8).toCharArray(Lat, 8);
  Lat[7] = char(gps_data.charAt(18));
  Serial.println(Lat);

  gps_data.substring(20, 20 + 12).toCharArray(Lon, 10);
  Lon[8] = char(gps_data.charAt(32));
  Serial.println(Lon);
  */
  char test_data[100];
  gps_data.toCharArray(test_data, 100);

  // V2 - Packet
  // $GPGLL,3927.83254,N,0808.25462,W,130448.00,A,A*71

  // V3 - Packet
  // $GNRMC,134055.000,A,3509.7572,N,09010.4938,W,1.51,338.00,121023,,,A*6C
  
  char *p = strtok(test_data, ",");  //code

  //Process longitude
  // bug fix for v2 (location packet)
  p = strtok(NULL, ",");  //lat
  sprintf(Lat, "%s", p);

  // bug fix in v2
  if (Lat[4] == '.') {
    Lat[7] = '\0';
  } else {
    Lat[8] = '\0';
  }
  p = strtok(NULL, ",");  // lat_char
  sprintf(Lat, "%s%s\0", Lat, p);

  // Process latitude
  // bug fix for v2 (location packet)
  p = strtok(NULL, ",");  //lng
  sprintf(Lon, "%s", p);

  if (Lon[4] == '.') {
    Lon[7] = '\0';
  } else {
    Lon[8] = '\0';
  }

  p = strtok(NULL, ",");  //dir
  sprintf(Lon, "%s%s\0", Lon, p);

  p = strtok(NULL, ",");  //state
  sprintf(alt, "NEBP:%s,", p);

  Serial.println(Lat);
  Serial.println(Lon);

  
  msg_valid = 1;
}

void update_GPS_alt(String gps_data) {

  //data is sent with 2 decimal places
  // reformat the data to be used by the APRS library
  //"$GPGGA,191757.00,3938.28486,N,07957.13511,W,1,03,2.71,274.5,M,-33.9,M,,*6F";

  char test_data[100];
  gps_data.toCharArray(test_data, 100);

  char *p = strtok(test_data, ",");  //code
  p = strtok(NULL, ",");             //time
  p = strtok(NULL, ",");             //lat
  p = strtok(NULL, ",");             //dir
  p = strtok(NULL, ",");             //lng
  p = strtok(NULL, ",");             //dir
  
  p = strtok(NULL, ",");  //state
  
  p = strtok(NULL, ",");    //sta-no
  strcat(alt,p);
  strcat(alt,",");
  p = strtok(NULL, ",");    //horizontal
  strcat(alt,p);
  strcat(alt,",alt=");
  p = strtok(NULL, ",");    //alti
  strcat(alt,p);
  p = strtok(NULL, ",");    //alti-unit
  strcat(alt,p);
  

  msg_valid = 1;
}

int location_update() {
  int wait = 400;

  // stop transmitting on a busy channel
  while (digitalRead(radio_sql) == 0) {
    wait--;
    Serial.print(".");
    if (wait == 0) {
      Serial.println("[info] Skip TX: busy channel");
      return 0;
    }
  }
  //radio_TX();
  Serial.println("[info] APRS:start");
  time_share = 0;
  APRS_init();

  APRS_setPreamble(300);
  APRS_setCallsign(myCALL, SSID);  

  APRS_setLat(Lat);
  APRS_setLon(Lon);

  // APRS icon - setting
  APRS_setSymbol(symbol);  

  //add msg id:
  sprintf(alt,",%d",msg_id++);
  APRS_sendLoc(alt, strlen(alt), ' ');

  delay(1200);

  Serial.println("APRS:end");
  msg_valid = 0;
  //gps.flush();
}