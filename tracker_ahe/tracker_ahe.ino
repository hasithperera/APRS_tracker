

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
#define timeout 300

#define freq_rx 146.875
#define freq_main 144.390
#define freq_alt 145.390
#define alt_freq_delay 10  //every 10 packets it will switch to alt freq

#define ctcss 146.2

#define simulate 0  // Set to 1 for testing
#define firmware_v 5.2

#define aprs_icon_low 'S'
#define aprs_icon_high 'O'

// Icon - setting:
//APRS_setSymbol('S');
// S - shuttle
// < - Bike
// O - Balloon
// > - car
// $ - phone
// [ - walker

#define aprs_id 11

//9 - Mobile station
//11 - Aircraft/Balloon
//7 - Hand held

#define APRS_TX_offset_utc_sec 19  // this selects what UTC second the packet will be sent out
#define APRS_interval_count 100    // 100 is 1 min


SoftwareSerial *dra_serial;  // Serial connection to DRA818
DRA818 *dra;                 // the DRA object once instanciated

String packetBuffer;
SoftwareSerial gps(8, 10);  // RX, TX


// W8CUL location
char Lat[] = "xxxx.xxxxxx";
char Lon[] = "xxxxx.xxxxxxxx";
char alt[] = "xxxxxxxxxx xxxxxxxxxx xxxxxxxxxx xxxxxxxxxx";  //43 chars
char cmd[20];

char test_data[150];

int cmd_cnt = 0;

int msg_id = 0;
int msg_valid = 0;
int loc_valid = 0;

char myCALL[] = "KE8TJE";
float freq_tx;

float alt_num = 0;

int packet_id = 0;
int radio_freq_change = 0;

int time_trigger = 0;
float lon, lat, utc;

void setup() {

  Serial.begin(57600);  // for logging
  //Wire.begin(0x08);     //added for i2c`:w


  Serial.println("[info] KE8TJE - APRS tracker");
  Serial.print(firmware_v);
  Serial.println("v");
  Serial.println("[info] IO init");
  dra_serial = new SoftwareSerial(RX, TX);  // Instantiate the Software Serial Object.

  init_radio();
  radio_on();
  set_radio_pwr(1);  // 0 low power

  // afternate frequancy in run time
  if (digitalRead(radio_freq_sw)) {
    freq_tx = freq_main;

  } else {
    freq_tx = freq_alt;
  }
  Serial.print("[info] ");
  Serial.println(freq_tx);
  //start GPS
  gps.begin(9600);

  //init radio module and change frequency

  //dra = DRA818::configure(dra_serial, DRA818_VHF, freq_tx, freq_rx, sql=4, vol=8, 0, 0, DRA818_12K5, true, true, true, &Serial);
  dra = DRA818::configure(dra_serial, DRA818_VHF, freq_tx, freq_rx, 5, 8, 0, 0, DRA818_12K5, true, true, true, &Serial);

  if (!dra) {
    Serial.println("[err ] RF init failed");
  } else {
    Serial.println("[info] RF OK");
  }
}

void loop() {

  //$GNGGA,033145.000,3938.0803,N,07957.1891,W,1,09,1.05,293.2,M,-33.0,M,,*4D
  gps.listen();
  while (gps.available() > 0) {
    String gps_raw = gps.readStringUntil('\n');
    //Serial.println(gps_raw);
    // Valid data: $GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D
    if (gps_raw.substring(0, 6) == "$GNGGA") {
      Serial.println(gps_raw);
      gps_raw.toCharArray(test_data, 100);
      char *p = strtok(test_data, ",");
      update_GPS_v2(p);
    }
  }
}

void update_GPS_v2(char *p) {

  // V2 - Packet
  // $GPGLL,3927.83254,N,0808.25462,W,130448.00,A,A*71

  // V3 - Packet
  // $GNRMC,134055.000,A,3509.7572,N,09010.4938,W,1.51,338.00,121023,,,A*6C
  // V3 +
  // $GNGGA ,033145.000 ,3938.0803  ,N      ,07957.1891 ,W        ,1  ,09 ,1.05 ,293.2,M,-33.0,M,,*4D
  //  <>    ,<1>        ,<2lat>     ,<3latd> ,<4lon>    ,<5lond>  ,<6>,<7>,<8>  ,<9>  ,<10>  
  p = strtok(NULL, ",");  //time
  utc = atof(p);

  int sec = long(utc) % 100;
  int min = long(utc) % 10000 - sec;
  Serial.print(min);
  Serial.print(",");
  Serial.println(sec);

  p = strtok(NULL, ",");  //lat - <2>
  sprintf(Lat, "%s", p);

  // bug fix in v2
  if (Lat[4] == '.') {
    Lat[7] = '\0';
  } else {
    Lat[8] = '\0';
  }
  p = strtok(NULL, ",");  // lat_char - <3>
  sprintf(Lat, "%s%s\0", Lat, p);

  p = strtok(NULL, ",");  //lng - <4>
  sprintf(Lon, "%s", p);
  if (Lon[4] == '.') {
    Lon[7] = '\0';
  } else {
    Lon[8] = '\0';
  }

  p = strtok(NULL, ",");  //dir - <5>
  sprintf(Lon, "%s%s\0", Lon, p);

  p = strtok(NULL, ",");  //state - <6>
  //Serial.print("-");
  //Serial.println(p);

  if (p[0] >= '1' & p[0] <= '4') {
    msg_valid = 1;
  } else {
    //Serial.println("[!] Invalid data");
    msg_valid = 0;
    return;
  }

 

  if (sec == APRS_TX_offset_utc_sec & (min % APRS_interval_count) == 0 & msg_valid) {
    Serial.println("[i] Event trigger");
    gps.stopListening();
    //delay(5000);
    location_update();
    msg_valid = 0;
    gps.listen();
  }



}

void update_GPS_alt(char *p) {

  //data is sent with 2 decimal places
  // reformat the data to be used by the APRS library
  //"$GPGGA,191757.00,3938.28486,N,07957.13511,W,1,03,2.71,274.5,M,-33.9,M,,*6F";
  // v3
  // $GNGGA,165006.000,2241.9107,N,12017.2383,E,1,14,0.79,22.6,M,18.5,M,,*42

  // Documentation: https://openrtk.readthedocs.io/en/latest/communication_port/nmea.html
  //$GNGGA<0>,000520.095<1>,<2>,<3>,<4>,<5>,0<6>,0<7>,<8>,<9>,M<10>,<11>,M<12>,<13>,*5D<14>

  //char *p = strtok(test_data, ",");  //code - <0>
  p = strtok(NULL, ",");  //time - <1>


  p = strtok(NULL, ",");  //lat - <2>
  sprintf(Lat, "%s", p);

  // bug fix in v2
  if (Lat[4] == '.') {
    Lat[7] = '\0';
  } else {
    Lat[8] = '\0';
  }


  p = strtok(NULL, ",");  // lat_char - <3>
  sprintf(Lat, "%s%s\0", Lat, p);

  //p = strtok(NULL, ",");             //lng
  //p = strtok(NULL, ",");             //dir

  p = strtok(NULL, ",");  //lng - <4>
  sprintf(Lon, "%s", p);

  if (Lon[4] == '.') {
    Lon[7] = '\0';
  } else {
    Lon[8] = '\0';
  }

  p = strtok(NULL, ",");  //dir - <5>
  sprintf(Lon, "%s%s\0", Lon, p);

  p = strtok(NULL, ",");  //state - <6>
  //Serial.print("-");
  //Serial.println(p);

  if (p[0] >= '1' & p[0] <= '4') {
    ;
  } else {
    //Serial.println("[!] Invalid data");
    msg_valid = 0;
    return;
  }

  // add firmware version to the packet
  sprintf(alt, "%x,", firmware_v);

  p = strtok(NULL, ",");  //sta-no - <7>
  strcat(alt, p);
  strcat(alt, ",");
  p = strtok(NULL, ",");  //horizontal
  strcat(alt, p);
  strcat(alt, ",alt=");
  p = strtok(NULL, ",");  //alti
  alt_num = atof(p);      //added altitude conversion for conditiona statements
  strcat(alt, p);
  p = strtok(NULL, ",");  //alti-unit
  strcat(alt, p);


  //Serial.print("alt len:");
  //strcpy(alt,final_msg);
  msg_valid = 1;

  Serial.print(Lat);
  Serial.print(",");
  Serial.println(Lon);

  if (packet_id % alt_freq_delay == 0 && radio_freq_change) {
    dra->group(DRA818_12K5, freq_alt, freq_alt, 13, 4, 0);
    Serial.println("alt_freq");
    radio_freq_change = 0;
  }

  if (packet_id % alt_freq_delay == 1 && radio_freq_change) {
    dra->group(DRA818_12K5, 144.390, 144.390, 13, 4, 0);
    Serial.println("main_freq");
    radio_freq_change = 0;
  }
}

int location_update() {

  // stop transmitting on a busy channel

  if (alt_num > 7000) {
    APRS_setSymbol(aprs_icon_low);
    set_radio_pwr(0);
  } else {
    APRS_setSymbol(aprs_icon_high);
    set_radio_pwr(1);
  }

  //radio_TX();
  Serial.println("[info] APRS:start");
  sprintf(alt, "KE8TJE APRS v5.2 msg_id:");
  Serial.println(alt);

  APRS_init();

  APRS_setPreamble(400);

  APRS_setCallsign(myCALL, aprs_id);
  APRS_setLat(Lat);
  APRS_setLon(Lon);

  char comment[30];
  //delay(100);
  sprintf(alt, "%s,%d", alt, packet_id++);
  APRS_sendLoc(alt, strlen(alt), ' ');
  

  while(bitRead(PORTB,5)); //lock untill packet is sent
  delay(800);
  Serial.println("[info] APRS:end");
  msg_valid = 0;

}
