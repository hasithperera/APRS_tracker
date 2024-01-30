

#include <stdio.h>
//#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include "DRA818.h"  // uncomment the following line in DRA818.h (#define DRA818_DEBUG)
#include <LibAPRS_Tracker.h>
#include <Wire.h>


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

#define freq_rx 145.390
#define freq_main 145.390  //145.390

#define ctcss 146.2

//#define simulate 1


SoftwareSerial *dra_serial;  // Serial connection to DRA818
DRA818 *dra;                 // the DRA object once instanciated
                             // the next frequency to scan
String packetBuffer;
SoftwareSerial gps(8, 10);  // RX, TX


// W8CUL location
char Lat[] = "xxxx.xxxxxx";
char Lon[] = "xxxxx.xxxxxxxx";
char alt[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
char cmd[20];
int cmd_cnt = 0;

int time_share = 0;
int msg_id = 0;
int msg_valid = 0;
char myCALL[] = "KE8TJE";
float freq_tx;

int packet_id = 0;

void setup() {

  Serial.begin(9600);  // for logging
  //Wire.begin(0x08);     //added for i2c`:w

   
  Serial.println("[info] KE8TJE - APRS tracker v3 - Beta");
  Serial.println("[info] IO init");
  dra_serial = new SoftwareSerial(RX, TX);  // Instantiate the Software Serial Object.

  init_radio();
  radio_on();
  set_radio_pwr(0);

  // afternate frequancy in run time
  if (digitalRead(radio_freq_sw)) {
    freq_tx = freq_main;
    Serial.println("[info] Alternate freq set");
  } else {
    freq_tx = freq_main;
    Serial.println("[info] APRS freq set");
  }
  //start GPS
  gps.begin(9600);

  //init radio module and change frequency

  //dra = DRA818::configure(dra_serial, DRA818_VHF, freq_rx, freq_tx, sql=4, vol=8, 0, 0, DRA818_12K5, true, true, true, &Serial);
  dra = DRA818::configure(dra_serial, DRA818_VHF, freq_tx, freq_tx, 4, 8, 0, 0, DRA818_12K5, true, true, true, &Serial);

  if (!dra) {
    Serial.println("[err ] RF init failed");
  } else {
    Serial.println("[info] RF OK");
  }
}

void loop() {


  gps.stopListening();
  Wire.end(); //ahe-1
  
  if (msg_id > 0 & msg_valid == 1) {
    location_update();
    Serial.println("SEND APRS");
  } else {
    Serial.println("[info] ------------------------ No location data");
  }
  gps.listen();
  Wire.begin(0x08); //ahe-1
  Wire.onReceive(receiveEvent); // ahe-1
  

  while (time_share < timeout) {
    while (gps.available() > 0) {
      time_share += 1;
      String gps_raw = gps.readStringUntil('\n');

      // Valid data: $GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D
      if (gps_raw.substring(0, 6) == "$GNRMC") {
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

        //Serial.println(gps_raw);


        if (gps_raw.length() > 30) {
          msg_id++;
          // GPS locked
          //update_GPS(gps_raw);

          //i2c functions needed
        }
      }

      //long packet - code
      // v2 - "$GPGGA"
      // v3 - "$GNGGA"
      //Serial.println(gps_raw);
      if (gps_raw.substring(0, 6) == "$GNGGA"){
        
        // debug packet
        //gps_raw = "$GNGGA,165006.000,2241.9107,N,12017.2383,E,1,14,0.79,22.6,M,18.5,M,,*42";
        
        Serial.println(gps_raw);
        update_GPS_alt(gps_raw);
      }
/*
      if (gps_raw.substring(0, 6) == "$GNGGA") {
        //simulate locked data
        Serial.println("Altitude packet");
        if (digitalRead(sim_packet) == 0) {
          gps_raw = "$GPGGA,191757.00,3938.28486,N,07957.13511,W,1,03,2.71,274.5,M,-33.9,M,,*6F";
          Serial.println("[info] Sim Packet 2");
        }
        Serial.println(gps_raw);

        if (gps_raw.length() > 40) {
          msg_id++;
          // GPS locked
          update_GPS_alt(gps_raw);
        }
      }
      */
      
    }
  }
  time_share = 0;
}

void update_GPS(String gps_data) {


  char test_data[100];
  gps_data.toCharArray(test_data, 100);

  // V2 - Packet
  // $GPGLL,3927.83254,N,0808.25462,W,130448.00,A,A*71

  // V3 - Packet
  // $GNRMC,134055.000,A,3509.7572,N,09010.4938,W,1.51,338.00,121023,,,A*6C

  char *p = strtok(test_data, ",");  //code
  p = strtok(NULL,","); //time
  p = strtok(NULL,","); //validifty

  //Serial.print("Data validity:");
  //Serial.println(p);

  if (p[0]=='A') {
    Serial.println("[i] GPS valid");
  } else {
    Serial.println("[!] Invalid data");
    return;
  }
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
  //sprintf(alt, "NEBP-WV ");
  //Serial.println("Short packet:");
  Serial.println(Lat);
  Serial.println(Lon);
  
  
  msg_valid = 1;
}

void update_GPS_alt(String gps_data) {

  //data is sent with 2 decimal places
  // reformat the data to be used by the APRS library
  //"$GPGGA,191757.00,3938.28486,N,07957.13511,W,1,03,2.71,274.5,M,-33.9,M,,*6F";
  // v3
  // $GNGGA,165006.000,2241.9107,N,12017.2383,E,1,14,0.79,22.6,M,18.5,M,,*42

  char test_data[100];
  gps_data.toCharArray(test_data, 100);

  char *p = strtok(test_data, ",");  //code
  p = strtok(NULL, ",");             //time


  p = strtok(NULL, ",");             //lat
  sprintf(Lat, "%s", p);

  // bug fix in v2
  if (Lat[4] == '.') {
    Lat[7] = '\0';
  } else {
    Lat[8] = '\0';
  }

  
  p = strtok(NULL, ",");  // lat_char
  sprintf(Lat, "%s%s\0", Lat, p);

  //p = strtok(NULL, ",");             //lng
  //p = strtok(NULL, ",");             //dir

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
  sprintf(alt,"ARC Roadtripping,");
  
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

  APRS_setCallsign(myCALL, 9);
  //9 - Mobile station
  //11 - Aircraft/Balloon
  //7 - Hand held

  APRS_setLat(Lat);
  APRS_setLon(Lon);

  // Icon - setting:
  //APRS_setSymbol('S');
  // S - shuttle
  // < - Bike
  //O - Balloon
  //> - car

  APRS_setSymbol('>');  


  char comment[30];
  //delay(100);
  //sprintf(comment, "NEBP-WV msg_id:%d", msg_id);
  //APRS_sendLoc(comment, strlen(comment), ' ');

  sprintf(alt,"%s,%d",alt,packet_id++);
  APRS_sendLoc(alt, strlen(alt), ' ');

  delay(1200);

  Serial.println("[info] APRS:end");
  Serial.println(alt);
  msg_valid = 0;
  //gps.flush();
}


void receiveEvent(int howMany) {
  /*while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }*/
  //cmd[cmd_cnt++];
  int x = Wire.read();    // receive byte as an integer
  Serial.print(x);         // print the integer
  Serial.print(',');
  if(cmd==12)
    Serial.println();

}
