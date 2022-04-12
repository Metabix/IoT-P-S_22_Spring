import paho.mqtt.client as mqtt
import codecs


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

    """The callback for when a PUBLISH message is received from the server."""
    if msg.topic=="home/livingroom/temperature":
        a=(msg.payload)
        print("temp",a.decode('cp437'))
        print(msg)
        
        #print("temperature"+ ' ' + a)
    
    else:
        b=(msg.payload)
        print("humid",b.decode('cp437'))
        print(" ")
        print(msg)
        #print("humidity"+ ' ' + b)






def main():

    mqtt_client = mqtt.Client()

    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)

    mqtt_client.on_connect = on_connect

    mqtt_client.on_message = on_message

    

    mqtt_client.connect(MQTT_ADDRESS, 1883)

    mqtt_client.loop_forever()

    print("hey")



if __name__ == '__main__':

    print('MQTT to InfluxDB bridge')

    main()


