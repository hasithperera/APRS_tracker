
- Use `v4.2` this is found in the `dtmf` branch on the repository.
## Confirm APRS opetation

- By default this is design for a ~30 s update rate on the 144.390 MHz standard APRS frequency in the US.
- There are two use configurable jumpers available for testing:
	
	- JP1 can be used to change the frequency to an alternate frequency.
	- JP2 can be used to enable simulation mode. This allows indoor testing on the module without GPS lock

- Onboard LEDs
	- D1: GPS serial Rx
	- D2: GPS serial Tx
	- D3: 3D GPS lock
	- D4: RF channel busy indicator active low. (Rx frequency is set to 146.800 MHz)
	- D5: Power indicator
### Simulation mode

- Connect a jumper to JP2. This will populate GPS data with a valid data packet located in Morgantown WV. (location ) verify the APRS data is correctly encoded in your preferred decoding software

### Debug data via serial terminal

- In addition to readable status comment the `$GNGGA` packets are redirected to the serial terminal when not in simulation mode
- When there is GPS lock decoded Longitude and Latitude is made available to the user.
	- `3938.76N,07958.44W`
- Sample terminal out put during simulation mode
	![](reports/res/Pasted%20image%2020240317233841.png)

## DTMF commanding 

