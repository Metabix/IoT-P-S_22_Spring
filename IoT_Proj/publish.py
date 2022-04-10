#aws stuff
# importing libraries
import paho.mqtt.client as paho
import os
import socket
import ssl
import random
import string
import json
from time import sleep
from random import uniform

#esp32-pi stuff
import paho.mqtt.client as mqtt

MQTT_ADDRESS = '172.16.105.2'

MQTT_USER = 'mo'

MQTT_PASSWORD = '1234567890'

MQTT_TOPIC = 'home/+/+'


def on_connect(client, userdata, flags, rc):

    """ The callback for when the client receives a CONNACK response from the server."""

    print('Connected with result code ' + str(rc))

    client.subscribe(MQTT_TOPIC)



def on_message(client, userdata, msg):

    """The callback for when a PUBLISH message is received from the server."""
    if msg.topic=="home/livingroom/temperature":
        a=(msg.payload)
        print(codecs.decode(a,'UTF-8'))
        #print("temperature"+ ' ' + a)
    
    else:
        b=(msg.payload)
        print(codecs.decode(b,'UTF-8'))
        #print("humidity"+ ' ' + b)






mqtt_client = mqtt.Client()

mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)

mqtt_client.on_connect = on_connect

mqtt_client.on_message = on_message



mqtt_client.connect(MQTT_ADDRESS, 1883)






\
#----------------------------------------- ESP data collected --------------------------------------------------

 
connflag = False
 
def awson_connect(client, userdata, flags, rc):                # func for making connection
    global connflag
    print("Connected to AWS")
    connflag = True
    print("Connection returned result: " + str(rc) )
 
def awson_message(client, userdata, msg):                      # Func for Sending msg
    print(msg.topic+" "+str(msg.payload))
    
    
 
#def on_log(client, userdata, level, buf):
#    print(msg.topic+" "+str(msg.payload))
 
mqttc = paho.Client()                                       # mqttc object
mqttc.on_connect = awson_connect                               # assign on_connect func
mqttc.on_message = awson_message                               # assign on_message func
#mqttc.on_log = on_log

#### Change following parameters #### 
awshost = "aelti3spevku9-ats.iot.us-west-2.amazonaws.com"      # Endpoint
awsport = 8883                                              # Port no.   
clientId = "AgrIoT_client"                                     # Thing_Name
thingName = "AgrIoT_client"                                    # Thing_Name
caPath = "/home/pi/Downloads/AmazonRootCA1.pem"                                      # Root_CA_Certificate_Name
certPath = "/home/pi/Downloads/AgrIoT-certificate.pem.crt"                            # <Thing_Name>.cert.pem
keyPath = "/home/pi/Downloads/AgrIoT-private.pem.key"                         # <Thing_Name>.private.key
 
mqttc.tls_set(caPath, certfile=certPath, keyfile=keyPath, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)  # pass parameters
 
mqttc.connect(awshost, awsport, keepalive=60)               # connect to aws server
 
mqttc.loop_start()                                          # Start the loop
 
while 1==1:
    sleep(5)
    if connflag == True:
        randomNumber = uniform(20.0,25.0)
        temperature_string= on_message
        humidity_string= on_message
        paylodmsg0="{"
        paylodmsg1 = "\"temperature\": \""
        paylodmsg2 = "\"humidity\": \""
        paylodmsg3="\"}"
        paylodmsg = "{} {} {} {} {} {}".format(paylodmsg0, paylodmsg1,temperature_string ,paylodmsg2, humidity_string, paylodmsg3)
        paylodmsg = json.dumps(paylodmsg) 
        paylodmsg_json = json.loads(paylodmsg)       
        mqttc.publish("ElectronicsInnovation", paylodmsg_json , qos=1)        # topic: temperature # Publishing Temperature values
        print("msg sent: ElectronicsInnovation" ) # Print sent temperature msg on console
        print(paylodmsg_json)

    else:
        print("waiting for connection...")
        
mqtt_client.loop_forever()


