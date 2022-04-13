/* ESP8266 AWS IoT
 *  
 * Simplest possible example (that I could come up with) of using an ESP8266 with AWS IoT.
 * No messing with openssl or spiffs just regular pubsub and certificates in string constants
 * 
 * This is working as at 7th Aug 2021 with the current ESP8266 Arduino core release 3.0.2
 * 
 * Author: Anthony Elder 
 * License: Apache License v2
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Airwave-05-521B";
const char* password = "";

// Find this awsEndpoint in the AWS Console: Manage - Things, choose your thing
// choose Interact, its the HTTPS Rest endpoint 
const char* awsEndpoint = "aelti3spevku9-ats.iot.us-west-2.amazonaws.com";

// For the two certificate strings below paste in the text of your AWS 
// device certificate and private key:

// xxxxxxxxxx-certificate.pem.crt
static const char certificatePemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAODS0AMSUgNJlMMNV7zE2dLU0XoCMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjA0MDkyMjIw
MzNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCsKWzaK6LKWPgO650c
n4CdesvBImIlpVwIluaaxkeRYRUbKsAmL8yzgjc1fytbAmT3ZTQeqkeupEEmpgbW
nTTDCSAbeGWpqK65GVv3i7Wj2p+LLnfSjvJct/MKmGNeyXp7tEeO+A+ljajqbqH2
89bo2tbv9lFPi215OnwLGjX6ueYh/T3hb/M7VDpWGZ44oUtQxGR23snISIibWcm5
I8VF6JVJWbh8x52nsWz4Vg0wceLVPCeINuBttD2k7jYOxxLUr1blsAz1XIYUvbbK
dSRb0LAV8vs40sO6oF2dqcpKpv7xYfBkZbUgPmSop4t2tSdUpDGXRE+hlC4zYFWH
0XpnAgMBAAGjYDBeMB8GA1UdIwQYMBaAFHUOoSc7L83Qg3h2JosZLrFVjps9MB0G
A1UdDgQWBBR1wuLS2qN2f9DJ3YtBL6pVJS7Z2zAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAJ/Oks4vv+mWoguI3qwgPx2jd
tbKmPH+1eNStHlVyapYP639GCcfcNCsTEFtW9BeEiewKtqAVsyGWRl7cJaC2Q2nz
64U0FQH9VNAt7JZQsYUOpZMzXGuiLPZfrLIIiNVDKuWWS4rFynA8OgEn0xMnByHI
GGbKKpHLDXYSDCd3RItDXKkTWJbnIkW6ypyjICA9+EPgfsAuC545p8fCUIMFQjKo
5aV8p1h3LyNITgOW81+1E2M60o7/Ac2nJnfPQTy3XKF58sSxWR/svdvC3MZBQLOz
j2d36HqtCC7sJwrTHleI1ibswoTHCNiAIiGc2zK0IHgP63H9Oj0L1BRo0GX3/Q==
-----END CERTIFICATE-----
)EOF";

// xxxxxxxxxx-private.pem.key
static const char privatePemKey[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEArCls2iuiylj4DuudHJ+AnXrLwSJiJaVcCJbmmsZHkWEVGyrA
Ji/Ms4I3NX8rWwJk92U0HqpHrqRBJqYG1p00wwkgG3hlqaiuuRlb94u1o9qfiy53
0o7yXLfzCphjXsl6e7RHjvgPpY2o6m6h9vPW6NrW7/ZRT4tteTp8Cxo1+rnmIf09
4W/zO1Q6VhmeOKFLUMRkdt7JyEiIm1nJuSPFReiVSVm4fMedp7Fs+FYNMHHi1Twn
iDbgbbQ9pO42DscS1K9W5bAM9VyGFL22ynUkW9CwFfL7ONLDuqBdnanKSqb+8WHw
ZGW1ID5kqKeLdrUnVKQxl0RPoZQuM2BVh9F6ZwIDAQABAoIBAQCTov6m5W63ya4H
1YOpDRtVcP2URTEXiYOwFf/x0IGFKVTZNEMNPEZ11gAUT1bVt+rvwQj+FxZbrJ32
T8BjgPLDIms3Q3sjD/Wt5BJa6aM9vXi9+X5lwZITcufoOSBWI792n/YRo6Z/WUj2
qSMZzKeKjkhuBbRa231ozX+cYcDSnflfHiFB9wjadTfrnaPDJ4ENjmT0tSMtpe8v
OxvcZXKK/gcIfsu9qTgPmiovDl9mAWTcSMGtd9LILrTkBweC+3/uYKQoLGJnwwgK
Exs4zuLl/qOO9KhZzMlKCz6rf7trcSJrNBIZYMFmwwXSzkPa/qvN9tkfZ+YrbISK
sn4YUxgBAoGBAOSPlU4Q/WnnwfUfr0OLhm6PeZsfypD043SZT6tnCgbBYi0IXaZN
h8ahCN6g4EeIfLak7KSQWBHLXHRS5upm4oQCnZB0DdHG+1oADvljkH468RR14wFs
o5uJ0S1qDHCtcj3Uiw1/sxN8AZ7LVQKOYrrwcKyOzUqswWHsyqHz8X5nAoGBAMDU
hCGJfK+cEdaTMynDwrv4zFzL9RRU2YQOvzIyX38U5nSdK86jyDq5TZQSKBL0PO3s
GHDcXCTByyOOyNV16O02JFSDWbra1Tfh7LcEHs4Si5iX6ZIMjSjlwLtVR4c21SvT
148fl0+hKlOkZIK78/4zftCRm/CxogSMVieOKqQBAoGASLfSBfNr3O0KjaWWCoRy
sexeAVe9CB+R2oAIk/0FeJ7AUJmPyiByqWSEwOgoUzn+i9GvJLB0BL1R3gSeSUeQ
m/WFEfeKigqn37uTCA+AYlj4aM0U+1gIOa1AaxWJfbQBW+CqmRHe9L0IevAwTM7m
ZJmpD42jjTWnlTNUSPCnDdMCgYBSsA7E7tcH2u4PTGnpI+fygRrpRBChGk14Anbc
iJtIvGaiZO9vGe53uBRbRXnnvtIN2GsWonfkzzFT2xul5Xzvk4L6tqSYqtQ5tWiz
hlHLXkp8MdBX74fsnLEL4+LbSZkZxSqvVgWlgN/MpDbxlLVpMR3VREtRaGqodqFP
Lvh8AQKBgESIwmp0EAlaTza5BDecmD/FNP7qF4Voo9fmdwGjINAMlj+ET5/5f64R
OTq9i1DCsnFidlb0x/7OBndE6z5PwY3q+5afiBMppoGoi60wpTBsp/51/n+5/OXC
RHdTiya0J8neQ34cOS9SMxAsXJ8BPQNDrzuqkQ/hzVIlmm2fkgnT
-----END RSA PRIVATE KEY-----
)EOF";

// This is the AWS IoT CA Certificate from: 
// https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html#server-authentication
// This one in here is the 'RSA 2048 bit key: Amazon Root CA 1' which is valid 
// until January 16, 2038 so unless it gets revoked you can leave this as is:
static const char caPemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5 
-----END CERTIFICATE-----
)EOF";

BearSSL::X509List client_crt(certificatePemCrt);
BearSSL::PrivateKey client_key(privatePemKey);
BearSSL::X509List rootCert(caPemCrt);

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(115200); Serial.println();
  Serial.println("ESP8266 AWS IoT Example");

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  // get current time, otherwise certificates are flagged as expired
  setCurrentTime();

  wiFiClient.setClientRSACert(&client_crt, &client_key);
  wiFiClient.setTrustAnchors(&rootCert);
}

unsigned long lastPublish;
int msgCount;

void loop() {

  pubSubCheckConnect();

  if (millis() - lastPublish > 10000) {
    //String msg = String("Hello from ESP8266: ") + ++msgCount;
    //pubSubClient.publish("outTopic", msg.c_str());
    pubSubClient.subscribe("sensor");
    //Serial.print("Published: "); Serial.println(msg);
    //lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthing");
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); 
  Serial.print(asctime(&timeinfo));}
