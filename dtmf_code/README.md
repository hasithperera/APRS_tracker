- This is an expansion board designed to be connected to the `v4` board
- Based on an ATTINY 84
- An i2c master will be running in order to sent data to the APRS board


## Audio performance 

- lab testing with 50 mW signal indicated limited functionality on the DTMF digits. Digits 7,8,9,0,#,* was confirmed to be working. a 620k is tested for the amplification
- It was noted the extended wire length induced RF and cause the signals to be unusable
	- 330 K and the build in 500 K pot was more than enough tuning range

## i2c 

- [ ] Send data to multiple addresses
- [ ] Request data from slave.  📅 2024-02-06 



# Todo

- [x] i2c master sending data ✅ 2024-01-28
- [ ] i2c master reading data from slaves
- [x] Evaluate the RF/Audio performance at altitude ✅ 2024-01-29

