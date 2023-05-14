
import serial
import time
# Set the serial port and baud rate
ser = serial.Serial('/dev/ttyUSB0', 9600)
t_start = time.time()
while True:
    try:
        data = ser.readline().decode('utf-8').strip()
    except:
        data = '[packet error]'
    print("{}:{}".format(time.time()-t_start,data))
