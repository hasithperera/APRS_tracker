

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
#define alt_freq_delay 10 //every 10 packets it will switch to alt freq

#define ctcss 146.2

#define simulate 0 // Set to 1 for testing
#define firmware_v 4.3

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




SoftwareSerial *dra_serial;  // Serial connection to DRA818
DRA818 *dra;                 // the DRA object once instanciated

String packetBuffer;
SoftwareSerial gps(8, 10);  // RX, TX


// W8CUL location
char Lat[] = "xxxx.xxxxxx";
char Lon[] = "xxxxx.xxxxxxxx";
char alt[] = "xxxxxxxxxx xxxxxxxxxx xxxxxxxxxx xxxxxxxxxx";  //43 chars 
char cmd[20];

char test_data[100];

int cmd_cnt = 0;

char final_msg[100];

int time_share = 0;
int msg_id = 0;
int msg_valid = 0;
char myCALL[] = "KE8TJE";
float freq_tx;

float alt_num = 0;

int packet_id = 0;
int radio_freq_change = 0;


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
  set_radio_pwr(1); // 0 low power

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


  gps.stopListening();
  Wire.end(); //ahe-1
  
  if (msg_id > 0 & msg_valid == 1) {
    location_update();
    Serial.print("APRS:");
    Serial.println(freq_tx);
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
      // if (gps_raw.substring(0, 6) == "$GNRMC") {
      //   //simulate locked data
      //   if (digitalRead(sim_packet) == 0) {
      //     //gps_raw = "$GPGLL,3938.28486,N,07957.13511,W,191757.00,A,A*7D";

      //     // V2 - Packet
      //     // $GPGLL,3927.83254,N,0808.25462,W,130448.00,A,A*71

      //     // V3 - Packet
      //     gps_raw ="$GNRMC,134055.000,A,3509.7572,N,09010.4938,W,1.51,338.00,121023,,,A*6C";
      //     Serial.println("[info] Sim Packet");
      //   }

      //   Serial.println(gps_raw);


      //   if (gps_raw.length() > 30) {
      //     msg_id++;
      //     // GPS locked
   
      //     gps_raw.toCharArray(test_data, 100);
      //     //Serial.println(test_data);

      //     //i2c functions needed
      //   }
      // }

      //long altitude packet - code
      // v2 - "$GPGGA"
      // v3 - "$GNGGA"
      //Serial.println(gps_raw); 

      // debug packet
      //gps_raw = "$GNGGA,165006.000,2241.9107,N,12017.2383,E,1,14,0.79,22.6,M,18.5,M,,*42";

      //$GNGGA,033145.000,3938.0803,N,07957.1891,W,1,09,1.05,293.2,M,-33.0,M,,*4D

      if (gps_raw.substring(0, 6) == "$GNGGA"){
         if (digitalRead(sim_packet) == 0 || simulate) {
          gps_raw = "$GNGGA,042303.000,3938.7688,N,07958.4412,W,1,09,1.33,8328.5,M,-33.0,M,,*4F";
          //v4.2 test data
         }else{
          //print raw GPS data
          Serial.println(gps_raw);
         }
        if (gps_raw.length() > 50) {
          gps_raw.toCharArray(test_data, 100);
          char *p = strtok(test_data, ",");
          update_GPS_alt(p);
          msg_id++;
        }     
      }
    }
  }
  time_share = 0;
}

void update_GPS(String gps_data) {


  
  gps_data.toCharArray(test_data, 100);

  Serial.println(test_data);
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
    msg_valid = 0;
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

void update_GPS_alt(char *p) {

  //data is sent with 2 decimal places
  // reformat the data to be used by the APRS library
  //"$GPGGA,191757.00,3938.28486,N,07957.13511,W,1,03,2.71,274.5,M,-33.9,M,,*6F";
  // v3
  // $GNGGA,165006.000,2241.9107,N,12017.2383,E,1,14,0.79,22.6,M,18.5,M,,*42

  // Documentation: https://openrtk.readthedocs.io/en/latest/communication_port/nmea.html
  //$GNGGA<0>,000520.095<1>,<2>,<3>,<4>,<5>,0<6>,0<7>,<8>,<9>,M<10>,<11>,M<12>,<13>,*5D<14>

  //char *p = strtok(test_data, ",");  //code - <0>
  p = strtok(NULL, ",");             //time - <1>


  p = strtok(NULL, ",");             //lat - <2>
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

  if (p[0]>='1' & p[0]<='4') {
    ;
  }
  else{
    //Serial.println("[!] Invalid data");
    msg_valid = 0;
    return;
  }

  // add firmware version to the packet
  sprintf(alt,"%x,",firmware_v);
 
  p = strtok(NULL, ",");    //sta-no - <7>
  strcat(alt,p);
  strcat(alt,",");
  p = strtok(NULL, ",");    //horizontal 
  strcat(alt,p);
  strcat(alt,",alt=");
  p = strtok(NULL, ",");    //alti
  alt_num = atof(p);  //added altitude conversion for conditiona statements
  strcat(alt,p);
  p = strtok(NULL, ",");    //alti-unit
  strcat(alt,p);
  

  //Serial.print("alt len:");
  //strcpy(alt,final_msg);
  msg_valid = 1;

  Serial.print(Lat);
  Serial.print(",");
  Serial.println(Lon);

  if(packet_id%alt_freq_delay==0 && radio_freq_change){
    dra->group(DRA818_12K5, freq_alt, freq_alt, 13, 4, 0);
    Serial.println("alt_freq");
    radio_freq_change = 0;

  }

  if(packet_id%alt_freq_delay==1 && radio_freq_change){
    dra->group(DRA818_12K5, 144.390, 144.390, 13, 4, 0);
    Serial.println("main_freq");
    radio_freq_change = 0;
  }
}

int location_update() {
  int wait = 400;

  // stop transmitting on a busy channel

  /*
  while (digitalRead(radio_sql) == 0) {
    wait--;
    Serial.print(".");
    if (wait == 0) {
      Serial.println("[info] Skip TX: busy channel");
      return 0;
    }
  }
  */

  if(alt_num>7000){
    APRS_setSymbol(aprs_icon_low);  
    set_radio_pwr(0);
  }else{
    APRS_setSymbol(aprs_icon_high);  
    set_radio_pwr(1);
  }
  
  //radio_TX();
  Serial.println("[info] APRS:start");
  Serial.println(alt);
  time_share = 0;
  APRS_init();

  APRS_setPreamble(400);

  APRS_setCallsign(myCALL, aprs_id);
  APRS_setLat(Lat);
  APRS_setLon(Lon);
  
  char comment[30];
  //delay(100);
  //sprintf(comment, "NEBP-WV msg_id:%d", msg_id);
  //APRS_sendLoc(comment, strlen(comment), ' ');

  sprintf(alt,"%s,%d",alt,packet_id++);
  APRS_sendLoc(alt, strlen(alt), ' ');
  

  delay(1500);

  Serial.println("[info] APRS:end");
  Serial.println(alt);
  msg_valid = 0;
  //gps.flush();
  radio_freq_change = 1;
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
