#include <SoftwareSerial.h>
#include <Servo.h>
#include "SNIPE.h"

#define TXpin 11
#define RXpin 10
#define ATSerial Serial
#define MQ2Pin A0
#define Buzzer 12
#define Led 13
#define Temperture A1
#define num 100

Servo ser;
float FilterGas=0;
float FilterTem=0;


SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";

void setup() {
  ATSerial.begin(115200);
  pinMode(13 , OUTPUT);

  // put your setup code here, to run once:
  while(ATSerial.read()>= 0) {}
  while(!ATSerial);

  DebugSerial.begin(115200);

  /* SNIPE LoRa Initialization */
  if (!SNIPE.lora_init()) {
    DebugSerial.println("SNIPE LoRa Initialization Fail!");
    while (1);
  }

  /* SNIPE LoRa Set App Key */
  if (!SNIPE.lora_setAppKey(lora_app_key)) {
    DebugSerial.println("SNIPE LoRa app key value has not been changed");
  }
  
  /* SNIPE LoRa Set Frequency */
  if (!SNIPE.lora_setFreq(LORA_CH_1)) {
    DebugSerial.println("SNIPE LoRa Frequency value has not been changed");
  }

  /* SNIPE LoRa Set Spreading Factor */
  if (!SNIPE.lora_setSf(LORA_SF_7)) {
    DebugSerial.println("SNIPE LoRa Sf value has not been changed");
  }
  
  /* SNIPE LoRa Set Rx Timeout 
   * If you select LORA_SF_12, 
   * RX Timout use a value greater than 5000  
  */
  if (!SNIPE.lora_setRxtout(5000)) {
    DebugSerial.println("SNIPE LoRa Rx Timout value has not been changed");
  }     
      
  DebugSerial.println("SNIPE LoRa LED Recv");

  // initialize digital pin LED_BUILTIN as an output.


  ser.attach(6);
  pinMode(Buzzer, OUTPUT);
  pinMode(MQ2Pin, INPUT);
  pinMode(Temperture, INPUT);
}

void loop() {
        String ver = SNIPE.lora_recv();
        
        if (ver == "123" )
        {
            tone(Buzzer,100);
            digitalWrite(13,HIGH);
            
            ser.write(90);
            /*DebugSerial.println("SNIPE LoRa LED on");
            DebugSerial.println(SNIPE.lora_getRssi());
            DebugSerial.println(SNIPE.lora_getSnr());  */           
            SNIPE.lora_send("123+Ack");
            Serial.print("화재 발생");
        }
        else if (ver == "456")
        {   
            noTone(Buzzer);
            digitalWrite(13,LOW);
            ser.write(0);

            /*DebugSerial.println("SNIPE LoRa LED off");
            DebugSerial.println(SNIPE.lora_getRssi());
            DebugSerial.println(SNIPE.lora_getSnr());    */         
            SNIPE.lora_send("456+Ack");      
        }
        else
        {
            DebugSerial.println("recv Error!!!");
            delay(1000);
        }
        delay(1000);
}
