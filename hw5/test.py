import os
import time
import re

def send(value1, value2):
    send = value1
    print(send)


while True :
    text = os.popen("free -m").readlines()
    header = re.split(" +",text[0])
    value = re.split(" +",text[1])
    send(header[2],value[2])
    send(header[3],value[3])
    send(header[4],value[4])
    time.sleep(1)


