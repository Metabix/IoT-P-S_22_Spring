import paho.mqtt.client as mqtt
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import codecs
import time


MQTT_ADDRESS = '172.16.105.2'

MQTT_USER = 'mo'

MQTT_PASSWORD = '1234567890'

MQTT_TOPIC = 'home/+/+'





def on_connect(client, userdata, flags, rc):

    """ The callback for when the client receives a CONNACK response from the server."""

    print('Connected with result code ' + str(rc))

    client.subscribe(MQTT_TOPIC)





def on_message(client, userdata, msg):
    global a
    global b

    """The callback for when a PUBLISH message is received from the server."""
    if msg.topic=="home/livingroom/temperature":
        a=(msg.payload)
        print("temp",a.decode('cp437'))
        #print("temperature"+ ' ' + a)
    
    else:
        b=(msg.payload)
        print("humid",b.decode('cp437'))
        #print("humidity"+ ' ' + b)



def main():
    while True:
        mqtt_client = mqtt.Client()

        mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)

        mqtt_client.on_connect = on_connect

        mqtt_client.on_message = on_message

    

        mqtt_client.connect(MQTT_ADDRESS, 1883)

        mqtt_client.loop_start()
        time.sleep(1) 
        mqtt_client.disconnect()
        mqtt_client.loop_stop()
        
        myMQTTClient = AWSIoTMQTTClient("AgrIoTClientID") #random key, if another connection using the same key is opened the previous one is auto closed by AWS IOT
        myMQTTClient.configureEndpoint("aelti3spevku9-ats.iot.us-west-2.amazonaws.com", 8883)
        myMQTTClient.configureCredentials("/home/pi/Downloads/AmazonRootCA1.pem",
                                          "/home/pi/Downloads/AgrIoT-private.pem.key",
                                          "/home/pi/Downloads/AgrIoT-certificate.pem.crt")
        myMQTTClient.configureOfflinePublishQueueing(-1) # Infinite offline Publish queueing
        myMQTTClient.configureDrainingFrequency(2) # Draining: 2 Hz
        myMQTTClient.configureConnectDisconnectTimeout(10) # 10 sec
        myMQTTClient.configureMQTTOperationTimeout(5) # 5 sec
        print ('Initiating Realtime Data Transfer From Raspberry Pi...')
        myMQTTClient.connect()
        myMQTTClient.publish(
            topic="sensor",
            QoS=1,
            payload='{"Temperature":"'+str(a)+'","humidity":"'+str(b)+'"}')
        myMQTTClient.disconnect()



if __name__ == '__main__':

    print('MQTT to InfluxDB bridge')

    main()



