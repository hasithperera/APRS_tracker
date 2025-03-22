# APRS Tracker - customization 

Most common settings are set up as `#define` statements for easy configuration. Some of the major settings are listed below with their typical value

variable | value | note 
---| --- | --- | 
myCALL[] |  | Change to your callsign
APRS_TX_offset_utc_sec  | 19 | UTC second the packet is sent out
aprs_id | 11 | station id used for aprs



## Programming the firmware

You need to have avrdude installed on your local computer along with an USBasp programmer. 

Using arduino you can export the `hex` file by using Sketch > Export compiled Binary. A new folder `build` will be created which contains the hex file needed.
To program the APRS board use the following command. 

`avrdude -p m328p -c usbasp -U flash:w:tracker_ahe.ino.hex`
