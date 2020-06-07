#include <16F887.h>
#device ADC = 10;
#device PASS_STRINGS = IN_RAM 
#include <reg.h>
#use rtos(timer=1,minor_cycle=100ms)
#fuses HS, NOWDT, NOPROTECT, NOBROWNOUT, NOPUT, NOLVP 
#use delay(crystal=20M)
#use rs232(baud=115200,xmit=PIN_c6,rcv=PIN_c7,stream=GPS, parity=N, bits=8)
#use rs232(UART1,baud=57600,xmit=PIN_C0,rcv=PIN_C1,stream=GS, parity=N, bits=8,)
#use i2c (MASTER,SDA=PIN_C4, SCL=PIN_C3,force_SW,fast=400000,stream=BME280_STREAM)

#include <GPS_Lib.c>
#include <INA219_lib.c>
#include <DHT_lib.c>
#include <BME280_Lib.c>

int t = 0,send_period = 2;
int p1 = 0;
float press1;
float PRESSURE;
int16 adc;
float VOLTAGE;
float CURRENT;
unsigned char dht_state = 0;
signed int32 raw_temperature;
unsigned int32 raw_pressure;


#task(rate=1s,max=1us)
   void BLINK_LED();
#task(rate=1s,max=10ms)
   void GET_BME_P();
   #task(rate=1s,max=10ms)
   void GET_BME_T();
#task(rate=1s,max=100ms)
   void SEND_DATA();

#int_rda
void serial_isr() {
GPSRead();
}

void inicializar();

void main(){
   
   inicializar();
   delay_ms(5000);
   pb5 = 1;
   RTOS_RUN();
}

void inicializar(){
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
   TRISA=0xFF;
   TRISB=0xDF;
   TRISC=0x9E;
   TRISD=0xFF;
   TRISE=0xFF;
   ANSEL = 0X00;
   ANSELH = 0X00;
   setup_adc_ports(sAN5,VSS_VDD);
   setup_adc(ADC_CLOCK_INTERNAL);//Configurando el convertidor ADC con reloj
   RBPU =1;
   pb5 = 0;
   INA219_init();
   dht_init();
   BME280_begin(MODE_NORMAL);
}

void GET_DATA(){
set_adc_channel(5);
adc = read_adc();
VOLTAGE = adc*5/1024.0;
CURRENT = get_current();

dht_state = get_dHT22();

}

void CLEAR_DATA(){
rawTime[0]=0;rawTime[1]=0;rawTime[2]=0;rawTime[3]=0;rawTime[4]=0;rawTime[5]=0;

rawLatitude[0]=0;rawLatitude[1]=0;rawLatitude[2]=0;rawLatitude[3]=0;
rawLatitude[4]=0;rawLatitude[5]=0;rawLatitude[6]=0;rawLatitude[7]=0;
rawLatitude[8]=0;rawLatitude[9]=0;rawLatitude[10]=0;rawLatitude[11]=0;

rawLongitude[0]=0;rawLongitude[1]=0;rawLongitude[2]=0;rawLongitude[3]=0;
rawLongitude[4]=0;rawLongitude[5]=0;rawLongitude[6]=0;rawLongitude[7]=0;
rawLongitude[8]=0;rawLongitude[9]=0;rawLongitude[10]=0;rawLongitude[11]=0;

rawSatellites[0]=0;rawSatellites[1]=0;
}
void SEND_DATA(){  
   
   t++;
   GET_DATA();
   if(t == send_period){
      fprintf(GS,"#CAN,");
      if (rawLatitude[2] == 0) fprintf(GS,",,,,,\n\r");
      else{
         fprintf(GS,"%c%c,",rawTime[0],rawTime[1]);
         fprintf(GS,"%c%c,",rawTime[2],rawTime[3]);
         fprintf(GS,"%c%c,",rawTime[4],rawTime[5]);
         fprintf(GS,"%c%c",rawLatitude[1],rawLatitude[2]);
         fprintf(GS,"%c%c%c",rawLatitude[3],rawLatitude[4],rawLatitude[5]);
         fprintf(GS,"%c%c%c",rawLatitude[6],rawLatitude[7],rawLatitude[8]);
         fprintf(GS,"%c%c%c,",rawLatitude[9],rawLatitude[10],rawLatitude[11]);
         fprintf(GS,"%c%c%c",rawLongitude[0],rawLongitude[1],rawLongitude[2]);
         fprintf(GS,"%c%c%c",rawLongitude[3],rawLongitude[4],rawLongitude[5]);
         fprintf(GS,"%c%c%c",rawLongitude[6],rawLongitude[7],rawLongitude[8]);
         fprintf(GS,"%c%c%c,",rawLongitude[9],rawLongitude[10],rawLongitude[11]);
         fprintf(GS,"%c%c",rawSatellites[0],rawSatellites[1]);
         fprintf(GS,"\n\r");
         }
      
      fprintf(GS,"#SAT,");
      switch (dht_state) 
      {     case 1:{}
            case 2: {fprintf(GS,"Off,lin");break;}
            case 3: {fprintf(GS,"Err,Err");break;} 
            default:{  
               fprintf(GS,"%2.1f,",dht22_Temp);
               fprintf(GS,"%2.1f", dht22_Rh); 
            }
      }
      fprintf(GS,",");
      fprintf(GS,"%02Lu.%02Lu,",raw_temperature/100,raw_temperature%100);
      fprintf(GS,"%3.2f,",PRESSURE);
      fprintf(GS,"%3.2f,",press1 - PRESSURE);
      fprintf(GS,"%.2f,",VOLTAGE);
      fprintf(GS,"%.2f",CURRENT);
      fprintf(GS,"\n\r");
      t=0;
      CLEAR_DATA();
   }
   
}

void GET_BME_P(){
   
   BME280_readPressure(&raw_pressure);
   PRESSURE = raw_pressure/100 + ((raw_pressure%100)*.01);
   if (p1 <= 3) {
   press1 = PRESSURE;
   }
   p1++;
   if (p1 >= 3)p1 = 4;
}
void GET_BME_T(){
   BME280_readTemperature(&raw_temperature); 
   //bme_temperature = raw_temperature/100;

}
void BLINK_LED(){
   output_toggle(PIN_B5);
}

