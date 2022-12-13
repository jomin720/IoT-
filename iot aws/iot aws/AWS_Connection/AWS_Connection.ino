

#include <ESP8266WiFi.h>
#include <PubSubClient.h> // version 2.7.0
#include <time.h>
#include <ArduinoJson.h>

#define MQ2Pin 7
#define TemPin A0
#define num 10

float FilterGas=0;
float FilterTem=0;


//json을 위한 설정
StaticJsonDocument<200> doc;
DeserializationError error;
JsonObject root;

const char *ssid = "soong";  // 와이파이 이름
const char *pass = "1771167a";      // 와이파이 비밀번호
const char *thingId = "ESP8266";          // 사물 이름 (thing ID) 
const char *host = "a32qc196pzw17h-ats.iot.ap-northeast-1.amazonaws.com"; // AWS IoT Core 주소
const char* outTopic = "$aws/things/ESP8266/shadow/update/accepted"; 
const char* inTopic = "inTopic"; 

// 사물 인증서 (파일 이름: xxxxxxxxxx-certificate.pem.crt)
const char cert_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAK2rN8rnq8PxAmis0Qc9qqP3JFudMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjEyMDIwNTE5
NTlaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC2wlHE/zXH1PKl1ZIi
f8/7unsyA2Brpp6iW9iUyLyY0tpZK9ZzBD2O+m3dpB/04H1u2t0/pDiGW/WaRkQA
jOpUNZo2ZnSNn+71kINsvrFQ/6Vz6IOdrJJ6gboXrOoqxLTZ8Y13XbUyVjp81s67
iEMG70AorjwWgnCa+APzmgRDFfWPfyxsy2rjQVjKb2FgD4uIymrc1Mdm13vsNSjp
BDlOSGHJQXkeIgx6zmCmxQN9C+8vJ4kN5FyGoFHnoY0oWpTOeVYKraRAGNZzDB2s
0QOKtk4Xiun/nvAd5d6KAy4xDUyjDhSkm8H7crHEo1ZcyJWYGtfy9isIPzdsMNrw
bTArAgMBAAGjYDBeMB8GA1UdIwQYMBaAFOVtqDyGKdlOUDLPliyi5a8nKxweMB0G
A1UdDgQWBBTkxb/bG8UGjOFG/sBwH7Gg1ka7CjAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAe5aOM1/WkIy/Wgk0F4SDdybq
SKijlZE9VXI0EYYiCGEB2iBkKBotHESTGSi18zFNKW2doD8L2Jig6FVvXINkpmBl
y9816ANCAjGOaQdJE/eiQWljf3mArjNGXDIFCoAWnAaNV3+RtwyQbYeCwNdvului
IIdlr76HLN66m2HlIaFvk4og6wA+PB0nsaTfFuNx8D+6Hzrocx8fLZtwMeG4uWgh
CS7+0XrH/VHO4ZbC4KQ43uRldMHBXNyuMOqq3IiyAN9wcGhECVbxFUpEcsJocwa9
Glqg32u71dp7ey32ih1fDU8cmPY+QIMs0/7bEqxoYExEf70wM8cr7hrIzSPayA==
-----END CERTIFICATE-----
)EOF";
// 사물 인증서 프라이빗 키 (파일 이름: xxxxxxxxxx-private.pem.key)
const char key_str[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAtsJRxP81x9TypdWSIn/P+7p7MgNga6aeolvYlMi8mNLaWSvW
cwQ9jvpt3aQf9OB9btrdP6Q4hlv1mkZEAIzqVDWaNmZ0jZ/u9ZCDbL6xUP+lc+iD
naySeoG6F6zqKsS02fGNd121MlY6fNbOu4hDBu9AKK48FoJwmvgD85oEQxX1j38s
bMtq40FYym9hYA+LiMpq3NTHZtd77DUo6QQ5TkhhyUF5HiIMes5gpsUDfQvvLyeJ
DeRchqBR56GNKFqUznlWCq2kQBjWcwwdrNEDirZOF4rp/57wHeXeigMuMQ1Mow4U
pJvB+3KxxKNWXMiVmBrX8vYrCD83bDDa8G0wKwIDAQABAoIBABpd2IwyQaEN4N/5
+vZNOEVZ9xIVGKEPTc2gsJNujn8B1IyO3YaEcxkgmh8i4ZVjmdUXcFoRKDKCK+HR
B1iI+/mgLhpxF5mjHEeDTOKwdEZ6S3V4cxyZXByegKmPwczfyjOzPLSe6VhUxmVi
XDsEAp+Kftgfov2gXfyNkYxgiQLgdpfhjnGQ1Y2Zpp98ctYGRNvAbucmVYeWPwtf
uW4T9Gx3EH7WjD2VrSJ1HtJZ2MhiY7YJW/mNY2hqW4crSSoRMyvRy6PoZA1QUigB
RbhkSBy4EwXD1bNPjnnF6+4CJJr+Gtp0THkYwizbG/TSlPOwih344lmAp6DFGL64
JlYRMrkCgYEA59ls6LQmFk7P9yfCkq7SStYUaGEyFk8wp79RGLtXHgmHQ6BP0Dyb
LFx3jE9UdejRanPQztC7dtlhhdxdCaUapFEBL5+v4Ot+Egvze/a+//KSJJjHiqb0
4RRSAMDWurj5vItlnO2gIms/1YH+3V2o6GiLy3Vq5rElMAmEG/PCtHcCgYEAycvW
CiDMV9ggXXZL0FTF7wKoSq2Hiyldfa3UzEBESIAlIt+tGiJ7yDUFindsyTF6qHXH
lBCOboygIqv3a59YKmeDDJGZql/wa/cEw/9/2KARwfZcLLYDmoWniAMdDjPai7fV
VXsBDuGwR530N2l9lHqcxHVywo4nY+hndUESUu0CgYEAj9lxHziW4QRpN/ntWUEq
rNACvXiAsSLVF182m9ewh6x1xlmy64qO5IlCi0W1K6sWM6xEA+2yQl1BiuJcv103
GHhB+iBj6XNzNxM/MMy5WOhSEG7Sb46DJRuMfXcoQ5X9LSR4GVO+JVzRsFEFpqcz
427qPYROnW+PmSxuUUF+rqUCgYEAnXaML10Gdulz39pGMGtmBCobriIYaVO+2yoA
qdzgK36q+7p0NbeAxWnyR+kGnzwXzUoSkQwWSFc9QbDUoUs2neglz3Hx6YtxbJ3V
Pyeo2B2XnbWimVTyHOp8HtrnGkUGHQ42zrvryHP9fPTZM0ACuRCwMXDAOuZ4ADLC
QDGlncUCgYBbC78FGQS9Ijb9ceGQGFHo67I/TEOwF58MdVqd8OliUXIaIjNdfD6U
k2HKc3lszzRWnUYOt5O4TM8PR6WjXEV9c9beGVC/A1mcahotH4+4CJvYW3VMangJ
Gq1bwL1d3jF3KcBBk8nE0S7elZda0p0WXtrgq12rbxjxJcJ4kQJPrQ==
-----END RSA PRIVATE KEY-----
)EOF";
// Amazon Trust Services(ATS) 엔드포인트 CA 인증서 (서버인증 > "RSA 2048비트 키: Amazon Root CA 1" 다운로드)
const char ca_str[] PROGMEM = R"EOF(
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

String sChipID; // mac address를 문자로 기기를 구분하는 기호로 사용
char cChipID[40];

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  deserializeJson(doc,payload);
  root = doc.as<JsonObject>();
  int value = root["on"];
  Serial.println(value);
}

X509List ca(ca_str);
X509List cert(cert_str);
PrivateKey key(key_str);
WiFiClientSecure wifiClient;
PubSubClient client(host, 8883, callback, wifiClient); //set  MQTT port number to 8883 as per //standard

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(thingId)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "hello world");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      char buf[256];
      wifiClient.getLastSSLError(buf,256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Set time via NTP, as required for x.509 validation
void setClock() {
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
  Serial.print(asctime(&timeinfo));
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  pinMode(MQ2Pin, INPUT);
  pinMode(TemPin, INPUT);
  Serial.println();
  Serial.println();

  //이름 자동으로 생성
  uint8_t chipid[6]="";
  WiFi.macAddress(chipid);
  sprintf(cChipID,"%02x%02x%02x%02x%02x%02x%c",chipid[5], chipid[4], chipid[3], chipid[2], chipid[1], chipid[0],0);
  sChipID=String(cChipID);
  thingId=cChipID;

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  wifiClient.setTrustAnchors(&ca);
  wifiClient.setClientRSACert(&cert, &key);
  Serial.println("Certifications and key are set");

  setClock();
  //client.setServer(host, 8883);
  client.setCallback(callback);
}

long lastMsg = 0;
char msg[50];


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int readValue = analogRead(TemPin);
  float t = ((readValue*5.0 / 1024.0)-0.5)*100;
  for(int i = 0; i <num; i++)
   {
    FilterTem += t;
    delay(100);
   }
  FilterTem /= num;

 
  int gas = digitalRead(MQ2Pin);
  for(int i = 0; i < num; i++)
  {
    FilterGas += gas;
    delay(100); 
  }
  FilterGas /= num;

  ///JSON 구조로 측정값을 출력하고 msg에 FilterTem와 FilterGas을 JSON 형태로 넣어 AWS로 전송
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    snprintf (msg, 75, "{\"state\":{\"reported\":{\"temperature\":\"%0.2f\",\"gas\":\"%0.2f\"}}}",FilterTem,FilterGas);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(outTopic, msg);
    Serial.print("Heap: "); Serial.println(ESP.getFreeHeap()); //Low heap can cause problems
 }
  
}
