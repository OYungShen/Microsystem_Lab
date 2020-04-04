import serial
import time
import paho.mqtt.client as paho

# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600, timeout=3)

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

s.write('ATID 0x0707\r\n'.encode())
char = s.read(3)
print('Set PAN ID 0x0707.')
print(char.decode())

s.write('ATWR\r\n'.encode())
char = s.read(3)
print('Write config.')
print(char.decode())

s.write('ATCN\r\n'.encode())
char = s.read(3)
print('Exit AT mode.')
print(char.decode())

mqttc = paho.Client()
# Settings for connection
host = "localhost"
topic= "Mbed"
port = 1883

# Callbacks
def on_connect(self, mosq, obj, rc):
    print("Connected rc: " + str(rc))

def on_message(mosq, obj, msg):
    print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n");

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed OK")

def on_unsubscribe(mosq, obj, mid, granted_qos):
    print("Unsubscribed OK")

# Set callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe
mqttc.on_unsubscribe = on_unsubscribe

# Connect and subscribe
print("Connecting to " + host + "/" + topic)
mqttc.connect(host, port=1883, keepalive=60)
mqttc.subscribe(topic, 0)

while True:
    line = s.readline()
    print(line)
    mqttc.publish(topic, line)
    print(line)