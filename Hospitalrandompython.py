import paho.mqtt.client as mqtt
import time
import json
import numpy as np

ID = [{'Temp-1' : 0, 'HeartRate-1': 0, 'SpO2-1': 0},
      {'Temp-2' : 0, 'HeartRate-2': 0, 'SpO2-2': 0},
      {'Temp-3' : 0, 'HeartRate-3': 0, 'SpO2-3': 0},
      {'Temp-4' : 0, 'HeartRate-4': 0, 'SpO2-4': 0},
      {'Temp-5' : 0, 'HeartRate-5': 0, 'SpO2-5': 0}
      ]

Location = {'lat' : 14.866199, 'long' : 102.0368255}

#### Function Decode Payload from Chirpstack #####
def on_message(client, userdata, message):
    mydata     = str(message.payload.decode("utf-8"))

#### Function Connect ChirpStack #####
def on_connect(client, userdata, flags, rc):
    print('Connected with result code ' + str(rc))

##### Function Check data receive and Send to thingsboard #####
            
mqttbroker = "electsut.ddns.net"
client = mqtt.Client()
client.on_message = on_message
client.on_connect = on_connect
client.connect(mqttbroker, 8883, 10)
client.loop_start()

i = 0

while True:
    try:
        if i == 5:
            i = 0

        Temp = np.random.uniform(30.0, 45.0)
        Heartrate = np.random.randint(60, 120)
        SpO2 = np.random.randint(90, 100)
        ID[i][f'Temp-{i + 1}'] = Temp
        ID[i][f'HeartRate-{i + 1}'] = Heartrate
        ID[i][f'SpO2-{i + 1}'] = SpO2

        client.publish('anurak/out', json.dumps(ID[i]), 1)
        client.publish('b6106164/location', json.dumps(Location), 1)
        print(f'ID-{i+1}:', ID[i])
        i  += 1
        time.sleep(5)
    except:
        pass