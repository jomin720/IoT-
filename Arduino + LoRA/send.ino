#include <SoftwareSerial.h>
#include "SNIPE.h"


#define MQ2Pin A0 //가스센서
#define Buzzer 12 //부저
#define Temperature A1 //온도센서
#define num 10 //필터링


#define TXpin 11        //LoRa 통신
#define RXpin 10        //LoRa 통신
#define ATSerial Serial //LoRa 통신


float FilterGas = 0;
float FilterTem = 0;

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";  

int status = 0;

void setup() {
  ATSerial.begin(115200);
  Serial.begin(115200);
  
  pinMode(Buzzer, OUTPUT);
  pinMode(MQ2Pin, INPUT);
  pinMode(Temperature, INPUT);
  
  
  
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
        
  DebugSerial.println("SNIPE LoRa LED Send");
}

void loop() {
        String ver;

        int readValue = analogRead(Temperature);
        float t = ((readValue*5.0 /1024.0) -0.5)*100;
        for(int i = 0; i<num; i++){
          FilterTem += t;
          delay(100);
        }
        FilterTem /= num;

        int gas = analogRead(MQ2Pin);
        for(int i=0; i<num; i++){
          FilterGas += gas;
          delay(100);
        }
        FilterGas /= num;


        Serial.print("T : ");
        Serial.println(FilterTem);
        Serial.print("G : ");
        Serial.println(FilterGas);

        delay(1000);
        /* status == 0 : led Off 
         *  status  !=0 : led On 
        */
        if (FilterTem >= 36.0 || FilterGas >= 180){
            //Send Command : Buzzer ON    
            if (SNIPE.lora_send("123"))
            {
                DebugSerial.println("send command [123] success");
                ver = SNIPE.lora_recv();
                if(ver == "Ack")
                    status = !status;
            }
            else
            {
                DebugSerial.println("send fail");
                delay(500);
            }
        }else{
            //Send Command : Buzzer OFF     
            if (SNIPE.lora_send("456"))
            {
                DebugSerial.println("send command [456] success");
                ver = SNIPE.lora_recv();
                if(ver == "Ack")
                    status = !status;
            }
            else
            {
                DebugSerial.println("send fail");
                delay(500);
            }  
        }
        delay(1000);
       
}
