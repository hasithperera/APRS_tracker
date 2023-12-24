To make this platform capable of using DTMF commands the audio from the radio (DRA818v) needs to be processed. 

### 23/12/2023

- Initial experiments done before failed since the induced RF was too strong to measure the output signal. a VOX base remote transmitting radio was used to isolate the induced RF
- SQL signal was used to trigger the scope.
	
	![F0004TEK](ALL0004/F0004TEK.BMP)
	- Yellow -  SQL signal from the radio
	- Blue - Audio output from the radio (1500 Hz tone playing) 
	- Purple - Audio output from FT530 (standard radio)
- DTMF tones - 0,1,3 
	
	![F0007TEK| 320](ALL0007/F0007TEK.JPG)
- tones are generated using audacity
- **This was audible using a head phone.** It might work without any modifications

### 24/12/2023

- a gain of 4 on the pre-amp worked out well from lab testing. It would be better to add a pot to have a rage of gain we can tune on the final board 
- use a Attiny84 to send commands to the radio 
- StD pin should be mounted as an interrupt pin on the attiny  
### Todo

- [x] Test a schematic for MT8870
- [ ] PCB desing

