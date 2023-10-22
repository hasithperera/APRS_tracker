# Offline ground station and Tracker manual


## Tracker configuration 

- current configuration: These settings needs to be set by reprogramming and current settings are indicated 
	- call sign: KE8TJE-11
  - beacon time: ~40 

internally on the PCB there are two configuration pins available for testing

- pin 1: frequency selection (0: 144.390 M 1: 145.390 M) 
- pin 2: location data (1: sim data 0: GPS data)

## Ground station configuration

- Run `rtl-test` to check if the SDR is working 
- `rtl_fm -f 144.39M - |direwolf -c sdr.conf -r 24000 -D 1 - >> log_sdr.log`
- run for visualization `java -jar YAAC.jar`


