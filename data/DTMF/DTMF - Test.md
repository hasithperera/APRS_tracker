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

### Todo

- [ ] Design a schematic for MT88700


