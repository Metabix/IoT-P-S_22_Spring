#include "mbedtls/aes.h"
#include "PubSubClient.h" // Connect and publish to the MQTT broker
#include<stdio.h>

// Code for the ESP32
#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)
#include "SparkFun_Si7021_Breakout_Library.h" 

Weather sensor;


// WiFi
const char* ssid = "Airwave-05-521B";                 // Your personal network SSID
const char* wifi_password = ""; // Your personal network password

// MQTT
const char* mqtt_server = "172.16.105.2";  // IP of the MQTT broker
const char* humidity_topic = "home/livingroom/humidity";
const char* temperature_topic = "home/livingroom/temperature";
const char* mqtt_username = "mo"; // MQTT username
const char* mqtt_password = "1234567890"; // MQTT password
const char* clientID = "client_livingroom"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 

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

  connect_MQTT();
}

void loop() {
Serial.setTimeout(2000);
mbedtls_aes_context aes; 
char * key = "abcdefghijklmnop";
unsigned char ciphertemp[16];
unsigned char cipherhum[16];

float h = sensor.getRH();
float t = sensor.getTemp();
char temp_buf[10],hum_buf[10];

gcvt(t,5,temp_buf);
char *temp_ptr = temp_buf;
encrypt(temp_ptr, key, ciphertemp);

gcvt(h,5,hum_buf);
char *hum_ptr = hum_buf;
encrypt(hum_ptr, key, cipherhum);


Serial.print("Humidity:  ");
Serial.print(h);
Serial.println(" %");
Serial.print("Temperature: ");
Serial.print(t);
Serial.println(" *C");



Serial.println("\nOriginal plain temp:");
Serial.println(t);
Serial.println("\nCiphered temp1:");

for (int i = 0; i < 16; i++) {
  char str[3];
  sprintf(str, "%02x", (int)ciphertemp[i]);
  Serial.print(str);
  Serial.print(" ");
  }
const char* x= (const char*)ciphertemp;
Serial.println(" ");
Serial.println(x);  



Serial.println("\nOriginal plain hum:");
Serial.println(h);
Serial.println("\nCiphered hum1:");

for (int i = 0; i < 16; i++) {
  char str[3];
  sprintf(str, "%02x", (int)cipherhum[i]);
  Serial.print(str);
  Serial.print(" ");
  }
const char* y= (const char*)cipherhum;
Serial.println(" ");
Serial.println(y); 

// PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
if (client.publish(temperature_topic, String(t).c_str())) 
{
  Serial.println("Temperature sent!");
}
else {Serial.println("Fail T");}


// PUBLISH to the MQTT Broker (topic = Humidity, defined at the beginning)
if (client.publish(humidity_topic, String(h).c_str())) 
{
  Serial.println("Humidity sent!");
}
else {Serial.println("Fail H");}

//client.disconnect();  // disconnect from the MQTT broker
client.connect(clientID, mqtt_username, mqtt_password);
delay(1000); //delay(1000*60);       // print new values every 1 Minute  
  
}
