import paho.mqtt.client as mqtt
import time
import json

ID = [{'Temp-1' : 0, 'HeartRate-1': 0, 'SpO2-1': 0},
      {'Temp-2' : 0, 'HeartRate-2': 0, 'SpO2-2': 0},
      {'Temp-3' : 0, 'HeartRate-3': 0, 'SpO2-3': 0},
      {'Temp-4' : 0, 'HeartRate-4': 0, 'SpO2-4': 0},
      {'Temp-5' : 0, 'HeartRate-5': 0, 'SpO2-5': 0}]

Location = {'lat' : 14.866199, 'long' : 102.0368255}

#### Function Decode Payload from Chirpstack #####
def on_message(client, userdata, message):
    mydata     = str(message.payload.decode("utf-8"))
    datajs     = json.loads(mydata)
    data_dict  = datajs["object"]
    # print(data_dict)
    data_dictLV2   =   json.loads(data_dict["data"])
    # print(data_dictLV2)
    check_Data(data_dictLV2)

#### Function Connect ChirpStack #####
def on_connect(client, userdata, flags, rc):
    print('Connected with result code ' + str(rc))

def check_Data(DataforSend):
    for i in range(10):
        if(f'Temp-{i + 1}' in DataforSend.keys()):
            
            ID[i][f'Temp-{i + 1}'] = DataforSend[f'Temp-{i + 1}']
            ID[i][f'HeartRate-{i + 1}'] = int(DataforSend[f'HeartRate-{i + 1}'])
            ID[i][f'SpO2-{i + 1}'] = int(DataforSend[f'SpO2-{i + 1}'])

            client.publish('anurak/out', json.dumps(ID[i]), 1)
            print(f'ID-{i+1}:', ID[i])

chirpstack = "203.158.2.100"
mqttc = mqtt.Client()
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.connect(chirpstack, 1883, 10)
mqttc.loop_start()

mqttbroker = "electsut.ddns.net"
client = mqtt.Client()
client.on_message = on_message
client.on_connect = on_connect
client.connect(mqttbroker, 8883, 10)
client.loop_start()

while True:
    try:
        mqttc.subscribe("application/105/device/24c5d9e632581161/rx", 2)
        client.publish('b6106164/location', json.dumps(Location), 1)
        time.sleep(0.1)
    except:
        pass