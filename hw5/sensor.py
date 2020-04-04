import os
import time
import re
import serial

# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600, timeout=1)

s.write('+++'.encode())
char = s.read(3)
print('Enter AT mode.')
print(char.decode())

s.write('ATMY 0x207\r\n'.encode())
char = s.read(3)
print('Set MY 0x207.')
print(char.decode())

s.write('ATDL 0x107\r\n'.encode())
char = s.read(3)
print('Set DL 0x107.')
print(char.decode())

s.write('ATID 0x7777\r\n'.encode())
char = s.read(3)
print('Set PAN ID 0x7777.')
print(char.decode())

s.write('ATWR\r\n'.encode())
char = s.read(3)
print('Write config.')
print(char.decode())

s.write('ATCN\r\n'.encode())
char = s.read(3)
print('Exit AT mode.')
print(char.decode())

print('')
print('Start Communication')


def send(value1):
    send = value1
    s.write(send.encode())


while True :
    text = os.popen("free -m").readlines()
    send(str(text[1]))
    print(text[1])
    time.sleep(1)
    

