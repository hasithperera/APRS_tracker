# APRS Tracker - customization 

Most common settings are set up as `#define` statements for easy configuration. Some of the major settings are listed below with their typical value

## Programming the firmware

You need to have avrdude installed on your local computer along with an USBasp programmer. To program the APRS board use the following command. 

`avrdude -p m328p -c usbasp -U flash:w:tracker_ahe.ino.hex`
