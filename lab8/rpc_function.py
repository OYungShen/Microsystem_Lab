import serial
import time
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)

s.write("                   ")
line=s.readline() # Read an echo string from K66F terminated with '\n'
print(line)
time.sleep(1)

s.write("/LEDControl/run 1 1")
line=s.readline() # Read an echo string from K66F terminated with '\n'
print(line)
time.sleep(1)

s.write("/LEDControl/run 2 1")
line=s.readline() # Read an echo string from K66F terminated with '\n'
print(line)
time.sleep(1)

s.write("/LEDControl/run 3 1")
line=s.readline() # Read an echo string from K66F terminated with '\n'
print(line)
time.sleep(1)

s.write("/LEDControl/run 2 0")
line=s.readline() # Read an echo string from K66F terminated with '\n'
print(line)
time.sleep(1)

s.write("/LEDControl/run 3 0")
line=s.readline() # Read an echo string from K66F terminated with '\n'
print(line)
time.sleep(1)

s.write("/LEDControl/run 1 0")
line=s.readline() # Read an echo string from K66F terminated with '\n'
print(line)


s.close()