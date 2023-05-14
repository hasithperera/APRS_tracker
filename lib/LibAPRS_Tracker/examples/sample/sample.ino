/*
 * This is a very simple usage for LibAPRS_Tracker
 *
 * Author : Baris DINC (TA7W/OH2UDS)
 *
 */

#include <LibAPRS_Tracker.h>
 //Screenshot at 2023-03-27 19-52-25
void setup() {
  APRS_init();
  char myCALL[] = "KE8TJE";
  char Lat[] = "3964.50N";
  char Lon[] = "07996.00E";

  APRS_setCallsign(myCALL, 7);
  APRS_setLat(Lat);
  APRS_setLon(Lon);

  Serial.begin(9600);
  Serial.println("WVU Rocketry - GPS APRS tracker");
}
 
void loop() {
  
  Serial.println("s");
  char comment []= "Test APRS homebrew ";
  APRS_sendLoc(comment, strlen(comment),'1');
  while(bitRead(PORTB,5)); //Wait for transmission to be completed
  Serial.println("e");
  delay(5000);
}
 
 
