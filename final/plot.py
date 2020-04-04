import paho.mqtt.client as paho
import re
import matplotlib.pyplot as plt
import numpy as np
from math import *

# https://os.mbed.com/teams/mqtt/wiki/Using-MQTT#python-client

# MQTT broker hosted on local machine
mqttc = paho.Client()

# Settings for connection
# TODO: revise host to your ip
host = "localhost" 
topic= "Mbed"

global t_now
t_now = 0.1
t=[]
data1 = []
data2 = []
data3 = []

plt.ion() 
fig, ax = plt.subplots(3, 1)
ax[0].set_xlabel('Time')
ax[1].set_xlabel('Time')
ax[2].set_xlabel('Time')
ax[0].set_ylabel('x')
ax[1].set_ylabel('y')
ax[2].set_ylabel('z')


# Callbacks
def on_connect(self, mosq, obj, rc):
    print("Connected rc: " + str(rc))

def on_message(mosq, obj, msg):
    global t_now
    text = msg.payload.decode()
    text2 = re.sub("\r\n","",text)
    value = re.split(" ",text2)
    print(value)
    data1.append(value[0])
    data2.append(value[1])
    data3.append(value[2])
    t.append(t_now)
    t_now+=0.1
    ax[0].plot(t,data1,'-r')
    ax[1].plot(t,data2,'-r')
    ax[2].plot(t,data3,'-r')
    plt.pause(1)
    ax[0].clf()
    ax[1].clf()
    ax[2].clf()
   


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

# Loop forever, receiving messages
mqttc.loop_forever()

