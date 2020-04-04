import serial
import time

# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)


while True:
    # send RPC to remote
    line = s.readline()
    print(line)

s.close()