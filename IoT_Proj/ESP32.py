#include "mbedtls/aes.h"
#include "PubSubClient.h" // Connect and publish to the MQTT broker

// Code for the ESP32
#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)

#include "SparkFun_Si7021_Breakout_Library.h" 

void encrypt(char * plainText, char * key, unsigned char * outputBuffer){
 
  mbedtls_aes_context aes;
 
  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, strlen(key) * 8 );
  mbedtls_aes_crypt_ecb( &aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)plainText, outputBuffer);
  mbedtls_aes_free( &aes );
}
 
void decrypt(unsigned char * chipherText, char * key, unsigned char * outputBuffer){
 
  mbedtls_aes_context aes;
 
  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_dec( &aes, (const unsigned char*) key, strlen(key) * 8 );
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)chipherText, outputBuffer);
  mbedtls_aes_free( &aes );
}

Weather sensor;


// WiFi
const char* ssid = "Airwave-05-521B";                 // Your personal network SSID
const char* wifi_password = ""; // Your personal network password

// MQTT
const char* mqtt_server = "172.16.105.1";  // IP of the MQTT broker
const char* humidity_topic = "home/livingroom/humidity";
const char* temperature_topic = "home/livingroom/temperature";
const char* mqtt_username = "mo"; // MQTT username
const char* mqtt_password = "1234567890"; // MQTT password
const char* clientID = "client_livingroom"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 

// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


void setup() {
  Serial.begin(115200);
  sensor.begin();
}

void loop() {
  connect_MQTT();
  Serial.setTimeout(2000);

  float h = sensor.getRH();
  float t = sensor.getTemp();

  Serial.print("Humidity:  ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");



  

  // MQTT can only transmit strings
  String hs="Hum: "+String((float)h)+" % ";
  String ts="Temp: "+String((float)t)+" C ";

  // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
  if (client.publish(temperature_topic, String(t).c_str())) {
    Serial.println("Temperature sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  //else {
  //  Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
  //  client.connect(clientID, mqtt_username, mqtt_password);
  //  delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
  //  client.publish(temperature_topic, String(t).c_str());
  // }

  // PUBLISH to the MQTT Broker (topic = Humidity, defined at the beginning)
  if (client.publish(humidity_topic, String(h).c_str())) {
    Serial.println("Humidity sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  //else {
  //  Serial.println("Humidity failed to send. Reconnecting to MQTT Broker and trying again");
  //  client.connect(clientID, mqtt_username, mqtt_password);
  //  delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
  //  client.publish(humidity_topic, String(h).c_str());
  //}
  client.disconnect();  // disconnect from the MQTT broker
  delay(500); //delay(1000*60);       // print new values every 1 Minute
  connect_MQTT();
}
