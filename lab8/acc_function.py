import serial
import time
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)

s.write("                   ")
line=s.readline() # Read an echo string from K66F terminated with '\n'
print(line)
time.sleep(1)

s.write("/getAcc/run        ")
line=s.readline() # Read an echo string from K66F terminated with '\n'
print(line)
time.sleep(1)

s.write("/getAddr/run       ")
line=s.readline() # Read an echo string from K66F terminated with '\n'
print(line)
time.sleep(1)