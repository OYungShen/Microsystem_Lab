import serial
import math
import time

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
start = 0


while(start == 0):
    line=s.readline()
    print(line)
    if(line == b"IIstart\r\n"):
        start = 1
time.sleep(3)
formatter = lambda v: "%1.4f" % v

while(start == 1):
    line=s.readline()
    x = float(line)
    line=s.readline()
    y = float(line)
    line=s.readline()
    z = float(line)
    cosvalue = z/(math.sqrt((x*x+y*y+z*z)))
    angle = math.acos(cosvalue)
    s.write(b'Angle is ')
    s.write(bytes(formatter(angle), 'UTF-8'))
    print(formatter(angle),"test")
    time.sleep(0.5)
