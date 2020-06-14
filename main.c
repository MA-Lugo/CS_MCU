#include <16F887.h>
#device ADC = 10;
#device PASS_STRINGS = IN_RAM 
#use rtos(timer=1,minor_cycle=100ms)
#fuses HS, NOWDT, NOPROTECT, NOBROWNOUT, NOPUT, NOLVP 
#use delay(crystal=20M)
#use rs232(baud=115200,xmit=PIN_c6,rcv=PIN_c7,stream=GPS, parity=N, bits=8)
#use rs232(UART1,baud=57600,xmit=PIN_C0,rcv=PIN_C1,stream=GS, parity=N, bits=8,)
#use i2c (MASTER,SDA=PIN_C4, SCL=PIN_C3,force_SW,fast=400000,stream=BME280_STREAM)

#include <reg.h>
#include <GPS_Lib.c>
#include <INA219_lib.c>
#include <DHT_lib.c>
#include <BME280_Lib.c>

int t = 0,send_period = 2,p1 = 0;
int16 adc;
float PRESS1,PRESSURE,BME_TEMP;
float VOLTAGE,CURRENT;
unsigned char dht_state = 0;
signed int32 raw_temperature;
unsigned int32 raw_pressure;

//TASKS RTOS
#task(rate=1s,max=1us)
   void BLINK_LED();
#task(rate=1s,max=10ms)
   void GET_BME_P();
   #task(rate=1s,max=10ms)
   void GET_BME_T();
#task(rate=1s,max=100ms)
   void SEND_DATA();
   
//SERIAL INTERRUPT
#int_rda
void serial_isr() {
   GPSRead();
}

void INIT();

void main(){
   INIT();
   delay_ms(5000);
   Led_st = 1;
   RTOS_RUN();
}

void INIT(){
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
   setup_adc_ports(sAN5,VSS_VDD);
   setup_adc(ADC_CLOCK_INTERNAL);
   TRISA = 0xFF;
   TRISB = 0xDF;
   TRISC = 0x9E;
   TRISD = 0xFF;
   TRISE = 0xFF;
   ANSEL = 0X00;
   ANSELH = 0X00;
   RBPU = 0xFF;
   Led_st = 0;
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
   int i;
   for(i=0;i<6;i++){rawTime[i]=0;}
   for(i=0;i<12;i++){rawLatitude[i]=0;}
   for(i=0;i<12;i++){rawLongitude[i]=0;}
   for(i=0;i<2;i++){rawSatellites[i]=0;}
}
void SEND_DATA(){  
   int i;
   t++;
   GET_DATA();
   if(t == send_period){
      fprintf(GS,"#CAN,");
      if (rawLatitude[2] != 0){
         fprintf(GS,"%c%c,%c%c,%c%c,",rawTime[0],rawTime[1],rawTime[2],rawTime[3],rawTime[4],rawTime[5]);
         for(i=1;i<12;i++){ fprintf(GS,"%c",rawLatitude[i]); }
         fprintf(GS,",");
         for(i=1;i<12;i++){ fprintf(GS,"%c",rawLongitude[i]); }
         fprintf(GS,",");
         fprintf(GS,"%c%c",rawSatellites[0],rawSatellites[1]);
         fprintf(GS,"\n\r");
      }
      else{ fprintf(GS,",,,,,\n\r"); }
      
      fprintf(GS,"#SAT,");
      switch (dht_state){     
            case 1:{}
            case 2: {fprintf(GS,"Off,lin");break;}
            case 3: {fprintf(GS,"Err,Err");break;} 
            default:{fprintf(GS,"%2.1f,%2.1f,",dht22_Temp,dht22_Rh);}
      }
      fprintf(GS,"%2.2f,%3.2f,%3.2f,",BME_TEMP,PRESSURE,PRESS1 - PRESSURE);
      fprintf(GS,"%.2f,%.2f\n\r",VOLTAGE,CURRENT);
      CLEAR_DATA();
      t = 0;
   }
}

void GET_BME_P(){
   BME280_readPressure(&raw_pressure);
   PRESSURE = raw_pressure/100 + ((raw_pressure%100)*.01);
   if (p1 <= 3) {
      PRESS1 = PRESSURE;
   }
   p1++;
   if (p1 >= 3)p1 = 4;
}
void GET_BME_T(){
   BME280_readTemperature(&raw_temperature); 
   BME_TEMP = raw_temperature/100 + ((raw_temperature%100)*.01);
}
void BLINK_LED(){
   output_toggle(PIN_B5);
}

