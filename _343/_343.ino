#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <dht11.h>

#define ADD 0x00
#define OLED_MOSI  D7
#define OLED_CLK   D5
#define OLED_DC    D0
#define OLED_CS    D8
#define OLED_RESET D3
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int IR_S =  D2;
dht11 DHT11;

const char* ssid = "HiWiFi_324F3A";
const char* password = "12345678";
const char* mqtt_server = "m14.cloudmqtt.com";

WiFiClient espClient;

PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  
  pinMode(IR_S, OUTPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, 16761);
  boolean flag = client.connect("ESP8266Client", "uwreopem", "fzy16W0799Vg");
  Serial.println(flag);
  client.subscribe("pi");
  client.setCallback(callback);
  digitalWrite(BUILTIN_LED, HIGH);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
}

void loop() {
  display.clearDisplay();
  int chk = DHT11.read(D4);
//  switch (chk)
//  {
//    case 0:
//    Serial.print("Humidity (%): ");
//    Serial.println(DHT11.humidity, DEC);
//    Serial.print("Temperature (C): ");
//    Serial.println(DHT11.temperature, DEC);
//    break;
//    case -1: Serial.println("Checksum error"); break;
//    case -2: Serial.println("Time out error"); break;
//    default: Serial.println("Unknown error"); break;
//  }
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  
  float dht_hum;
  dht_hum = DHT11.humidity;
  float dht_tmp;
  dht_tmp = DHT11.temperature;
  
  display.println("HUM(%): ");
  display.println(String(dht_hum));
  display.println("TEMP(oC): ");
  display.println(String(dht_tmp));
  display.display();
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  delay(1000);
  
  display.clearDisplay();
}

void setup_wifi() {

  delay(10);
  //Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  uint8_t temp;
  if ((char)payload[0] == '1') {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20,20);
    display.print("occupied");
    display.display();
    temp = 1;
    IR_send(temp);
    digitalWrite(BUILTIN_LED, LOW); 
    delay(300000);
    temp = 2;
    IR_send(temp);
    digitalWrite(BUILTIN_LED, HIGH);
    display.clearDisplay();
  } else {
    temp = 2;
    IR_send(temp);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client", "uwreopem", "fzy16W0799Vg")) {//ESP8266Client
      client.subscribe("pi");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      delay(1000);
    }
  }
}

void IR_send(uint8_t code) {
  uint8_t dat;
  IR_Send38KHZ(280,1);//发送9ms的起始码
  IR_Send38KHZ(140,0);//发送4.5ms的结果码
   
  IR_Sendcode(ADD);//用户识别码
  dat=~ADD;
  IR_Sendcode(dat);//用户识别码反吗
   
  IR_Sendcode(code);//操作码
  dat=~code;
  IR_Sendcode(dat);//操作码反码
   
  IR_Send38KHZ(21,1);//发送结束码
}

void IR_Send38KHZ(int x,int y) {  //产生38KHZ红外脉冲 
  for(int i=0;i<x;i++) {  //15=386US
    if(y==1) {
     digitalWrite(IR_S,1);
     delayMicroseconds(9);
     digitalWrite(IR_S,0);
     delayMicroseconds(9);
    }
    else {
     digitalWrite(IR_S,0);
     delayMicroseconds(20);
    }            
  }
}

void IR_Sendcode(uint8_t x) {  //
  for(int i = 0; i < 8; i++) {
    if((x&0x01) == 0x01) {
      IR_Send38KHZ(23,1);
      IR_Send38KHZ(64,0);             
    }
    else {
      IR_Send38KHZ(23,1);
      IR_Send38KHZ(21,0);  
    }
    x=x>>1;
  }  
}
